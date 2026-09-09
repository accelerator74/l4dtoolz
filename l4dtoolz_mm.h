#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <ISmmPlugin.h>
#include "baseserver.h"
#include "matchmaking/imatchframework.h"

class CGameRules;

class l4dtoolz : public ISmmPlugin, public IConCommandBaseAccessor
{
public:
	l4dtoolz();
	bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	bool Unload(char *error, size_t maxlen);
	KHook::Return<void> ApplyGameSettings(IServerGameDLL*, KeyValues *pKV);
	KHook::Return<int> GetTotalNumPlayersSupported(IMatchTitle*);
	KHook::Return<bool> LevelInit(IServerGameDLL*, const char *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background);
	KHook::Return<void> LevelShutdown(IServerGameDLL*);
	KHook::Return<void> ReplyReservationRequest(CBaseServer*, netadr_t& adr, bf_read& inmsg);
	KHook::Return<int> GetMaxHumanPlayers(CGameRules*);
public:
	const char *GetAuthor();
	const char *GetName();
	const char *GetDescription();
	const char *GetURL();
	const char *GetLicense();
	const char *GetVersion();
	const char *GetDate();
	const char *GetLogTag();

	KHook::Virtual<IServerGameDLL, void, KeyValues*> m_ApplyGameSettings;
	KHook::Virtual<IMatchTitle, int> m_GetTotalNumPlayersSupported;
	KHook::Virtual<IServerGameDLL, bool, char const *, char const *, char const *, char const *, bool, bool> m_LevelInit;
	KHook::Virtual<IServerGameDLL, void> m_LevelShutdown;
	KHook::Virtual<CBaseServer, void, netadr_t&, bf_read&> m_ReplyReservationRequest;
	KHook::Virtual<CGameRules, int> g_HookGetMaxHumanPlayers;
public:    //IConCommandBaseAccessor
	bool RegisterConCommandBase(ConCommandBase *pVar);
public:
	static void OnChangeMaxplayers ( IConVar *var, const char *pOldValue, float flOldValue );
	static void OnChangeUnreserved ( IConVar *var, const char *pOldValue, float flOldValue );
};

ServerClass *UTIL_FindServerClass(const char *classname);

extern l4dtoolz g_l4dtoolz;

PLUGIN_GLOBALVARS();

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
