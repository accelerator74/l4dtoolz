#include "l4dtoolz_mm.h"
#include "game_offsets.h"
#include "memutils.h"
#include "icommandline.h"
#include "server_class.h"
#include "khook.hpp"

l4dtoolz g_l4dtoolz;
IServerGameDLL* gamedll = NULL;
IServerGameClients* gameclients = NULL;
IVEngineServer* engine = NULL;
IMatchFramework* g_pMatchFramework = NULL;
ICvar* g_pCVar = NULL;
CBaseServer* g_pGameIServer = NULL;
CGameRules* g_pGameRules = nullptr;
int g_nGameSlots = -1;

ConVar sv_maxplayers("sv_maxplayers", "-1", FCVAR_SPONLY|FCVAR_NOTIFY, "Max Human Players", true, -1, true, 32, l4dtoolz::OnChangeMaxplayers);
ConVar sv_force_unreserved("sv_force_unreserved", "0", FCVAR_SPONLY|FCVAR_NOTIFY, "Disallow lobby reservation cookie", true, 0, true, 1, l4dtoolz::OnChangeUnreserved);


l4dtoolz::l4dtoolz() : 
	m_ApplyGameSettings(&IServerGameDLL::ApplyGameSettings, this, nullptr, &l4dtoolz::ApplyGameSettings),
	m_GetTotalNumPlayersSupported(&IMatchTitle::GetTotalNumPlayersSupported, this, &l4dtoolz::GetTotalNumPlayersSupported, nullptr),
	m_LevelInit(&IServerGameDLL::LevelInit, this, nullptr, &l4dtoolz::LevelInit),
	m_LevelShutdown(&IServerGameDLL::LevelShutdown, this, &l4dtoolz::LevelShutdown, nullptr),
	m_ReplyReservationRequest(&CBaseServer::ReplyReservationRequest, this, &l4dtoolz::ReplyReservationRequest, nullptr)
{
	g_HookGetMaxHumanPlayers.Configure(maxhuman_idx);
	g_HookGetMaxHumanPlayers.AddContext(this, &l4dtoolz::GetMaxHumanPlayers, nullptr);
}

void l4dtoolz::OnChangeMaxplayers(IConVar *var, const char *pOldValue, float flOldValue)
{
	int new_value = ((ConVar*)var)->GetInt();
	int old_value = atoi(pOldValue);
	if (g_pGameIServer == NULL) {
		Msg("g_pGameIServer pointer is not available\n");
		return;
	}
	if (new_value != old_value) {
		if (new_value >= 0) {
			g_nGameSlots = new_value;
			g_pGameIServer->m_numGameSlots = g_nGameSlots;
		} else {
			g_nGameSlots = -1;
		}
	}
}

void l4dtoolz::OnChangeUnreserved(IConVar *var, const char *pOldValue, float flOldValue)
{
	int new_value = ((ConVar*)var)->GetInt();
	int old_value = atoi(pOldValue);
	if (g_pGameIServer == NULL) {
		Msg("g_pGameIServer pointer is not available\n");
		return;
	}
	if (new_value != old_value) {
		if (new_value == 1) {
			g_pCVar->FindVar("sv_allow_lobby_connect_only")->SetValue(0);
		}
	}
}

KHook::Return<void> l4dtoolz::ApplyGameSettings(IServerGameDLL*, KeyValues *pKV)
{
	if (!pKV) {
		return { KHook::Action::Ignore };
	}
	g_nGameSlots = sv_maxplayers.GetInt();
	if (g_nGameSlots == -1) {
		return { KHook::Action::Ignore };
	}
	pKV->SetInt("members/numSlots", g_nGameSlots);
	return { KHook::Action::Ignore };
}

KHook::Return<void> l4dtoolz::ReplyReservationRequest(CBaseServer*, netadr_t& adr, bf_read& inmsg)
{
	if (sv_force_unreserved.GetInt()) {
		if (g_pGameIServer != NULL) {
			if (g_pGameIServer->m_nReservationCookie != 0)
				return { KHook::Action::Ignore };
		}
		return { KHook::Action::Supersede };
	}
	return { KHook::Action::Ignore };
}

KHook::Return<int> l4dtoolz::GetTotalNumPlayersSupported(IMatchTitle*)
{
	if (g_nGameSlots > 0) {
		return { KHook::Action::Supersede, g_nGameSlots };
	}
	return { KHook::Action::Ignore, g_nGameSlots };
}

KHook::Return<int> l4dtoolz::GetMaxHumanPlayers(CGameRules*)
{
	if (g_nGameSlots > 0) {
		return { KHook::Action::Supersede, g_nGameSlots };
	}
	return { KHook::Action::Ignore, g_nGameSlots };
}

PLUGIN_EXPOSE(l4dtoolz, g_l4dtoolz);

bool l4dtoolz::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_CURRENT(GetServerFactory, gamedll, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_CURRENT(GetServerFactory, gameclients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pMatchFramework, IMatchFramework, IMATCHFRAMEWORK_VERSION_STRING);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);

	void* handle = NULL;
#if defined WIN32
	if (!(handle=KHook::FindOriginalVirtual(*(void***)engine, KHook::GetVtableIndex(&IVEngineServer::CreateFakeClient)))) {
		Warning("Failed to get address 'IVEngineServer::CreateFakeClient'\n");
	} else {
		g_pGameIServer = *reinterpret_cast<CBaseServer **>(reinterpret_cast<unsigned char *>(handle)+sv_offs);
#else
	if (!(handle=dlopen(engine_dll, RTLD_LAZY))) {
		Warning("Could't open library '%s'\n", engine_dll);
	} else {
		g_pGameIServer = (CBaseServer *)g_MemUtils.ResolveSymbol(handle, "sv");
		dlclose(handle);
#endif
		const char *pszCmdLineMax;
		if (CommandLine()->CheckParm("-maxplayers", &pszCmdLineMax)) {
			g_pGameIServer->m_nMaxclients = clamp(atoi(pszCmdLineMax), 1, 32);
		} else {
			g_pGameIServer->m_nMaxclients = 31;
		}
	}

	m_ApplyGameSettings.Add(gamedll);
	m_GetTotalNumPlayersSupported.Add(g_pMatchFramework->GetMatchTitle());
	m_LevelInit.Add(gamedll);
	m_LevelShutdown.Add(gamedll);

	if (g_pGameIServer) {
		m_ReplyReservationRequest.Add(g_pGameIServer);
	} else {
		Warning("g_pGameIServer pointer is not available\n");
	}

	ConVar_Register(0, this);

	return true;
}

bool l4dtoolz::Unload(char *error, size_t maxlen)
{
	m_ApplyGameSettings.Remove(gamedll);
	m_GetTotalNumPlayersSupported.Remove(g_pMatchFramework->GetMatchTitle());
	m_LevelInit.Remove(gamedll);
	m_LevelShutdown.Remove(gamedll);

	if (g_pGameIServer) {
		m_ReplyReservationRequest.Remove(g_pGameIServer);
	}

	LevelShutdown(gamedll);
	ConVar_Unregister();

	return true;
}

KHook::Return<bool> l4dtoolz::LevelInit(IServerGameDLL*, const char *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background)
{
	g_pGameRules = nullptr;

	ServerClass *pServerClass = UTIL_FindServerClass("CTerrorGameRulesProxy");
	if (pServerClass) {
		int i, iCount;
		iCount = pServerClass->m_pTable->GetNumProps();
		for (i = 0; i < iCount; i++) {
			if (stricmp(pServerClass->m_pTable->GetProp(i)->GetName(), "terror_gamerules_data") == 0) {
				g_pGameRules = reinterpret_cast<CGameRules*>((*pServerClass->m_pTable->GetProp(i)->GetDataTableProxyFn())(NULL, NULL, NULL, NULL, 0));
				break;
			}
		}	
	}

	if (g_pGameRules) {
		g_HookGetMaxHumanPlayers.Add(g_pGameRules);
	} else {
		Warning("g_pGameRules pointer is not available\n");
	}

	return { KHook::Action::Ignore, true };
}

KHook::Return<void> l4dtoolz::LevelShutdown(IServerGameDLL*)
{
	if (g_pGameRules) {
		g_HookGetMaxHumanPlayers.Remove(g_pGameRules);
		g_pGameRules = nullptr;
	}

	return { KHook::Action::Ignore };
}

ServerClass *UTIL_FindServerClass(const char *classname)
{
	ServerClass *sc = gamedll->GetAllServerClasses();
	while (sc)
	{
		if (strcmp(classname, sc->GetName()) == 0)
		{
			return sc;
		}
		sc = sc->m_pNext;
	}

	return NULL;
}

const char *l4dtoolz::GetLicense()
{
	return "GPLv3";
}

const char *l4dtoolz::GetVersion()
{
	return "2.3.0";
}

const char *l4dtoolz::GetDate()
{
	return __DATE__;
}

const char *l4dtoolz::GetLogTag()
{
	return "L4DToolZ";
}

const char *l4dtoolz::GetAuthor()
{
	return "Accelerator, Ivailosp";
}

const char *l4dtoolz::GetDescription()
{
	return "Unlock the max player limit on L4D and L4D2";
}

const char *l4dtoolz::GetName()
{
	return "L4DToolZ";
}

const char *l4dtoolz::GetURL()
{
	return "https://github.com/Accelerator74/l4dtoolz";
}

bool l4dtoolz::RegisterConCommandBase(ConCommandBase *pVar)
{
	return META_REGCVAR(pVar);
}
