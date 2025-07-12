#ifndef _INCLUDE_GAME_OFFSETS_
#define _INCLUDE_GAME_OFFSETS_

#if SH_SYS == SH_SYS_WIN32
	const int maxplayers_offs = 0x228; // m_nMaxClientsLimit (in CGameServer::SetMaxClients)
	#if SOURCE_ENGINE == SE_LEFT4DEAD
		const int sv_offs = 6; // IServer pointer (in IVEngineServer::CreateFakeClient)
		const int maxhuman_idx = 131; // CTerrorGameRules::GetMaxHumanPlayers vtable
	#else
		const int sv_offs = 8; // IServer pointer (in IVEngineServer::CreateFakeClient)
		const int maxhuman_idx = 136; // CTerrorGameRules::GetMaxHumanPlayers vtable
	#endif
#else
	const int maxplayers_offs = 0x220; // m_nMaxClientsLimit (in CGameServer::SetMaxClients)
	#if SOURCE_ENGINE == SE_LEFT4DEAD
		const char* engine_dll = "engine.so";
		const int maxhuman_idx = 132; // CTerrorGameRules::GetMaxHumanPlayers vtable
	#else
		const char* engine_dll = "engine_srv.so";
		const int maxhuman_idx = 137; // CTerrorGameRules::GetMaxHumanPlayers vtable
	#endif
#endif

#endif //_INCLUDE_GAME_OFFSETS_
