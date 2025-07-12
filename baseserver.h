// CBaseServer vtable for l4d1 and l4d2 games

/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown SourceMod Extension
 * Copyright (C) 2021-2023 A1m`;
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#ifndef BASESERVER_H
#define BASESERVER_H
#ifdef _WIN32
#pragma once
#endif

#include <iserver.h>
#include <netadr.h>
#include <irecipientfilter.h>
#include <bitbuf.h>  
#include <utlvector.h>
#include <checksum_crc.h>

class IClient;
class CBaseClient;
class CClientFrame;
class CFrameSnapshot;
class CLC_SplitPlayerConnect;
class CNetworkStringTableContainer;

enum server_state_t
{
	ss_dead = 0,	// Dead
	ss_loading,		// Spawning
	ss_active,		// Running
	ss_paused,		// Running, but paused
};

typedef struct
{
	netadr_t  adr;       // Address where challenge value was sent to.
	int			challenge; // To connect, adr IP address must respond with this #
	float		time;      // # is valid for only a short duration.
} challenge_t;

class CBaseServer :
	public IServer
{
public:
	
	virtual ~CBaseServer() = 0;
	
public: // IConnectionlessPacketHandler implementation
	
	virtual bool ProcessConnectionlessPacket( netpacket_t *packet ) = 0;

public: // IServer implementation

	virtual int GetNumClients( void ) const = 0;
	virtual int GetNumProxies( void ) const = 0;
	virtual int GetNumFakeClients( void ) const = 0;
	virtual int GetMaxClients( void ) const = 0;
	virtual IClient* GetClient( int index ) = 0;
	virtual int GetClientCount( void ) const = 0;
	virtual int GetUDPPort( void ) const = 0;
	virtual float GetTime( void ) const = 0;
	virtual int GetTick( void ) const = 0;
	virtual float GetTickInterval( void ) const = 0;

#if SOURCE_ENGINE == SE_LEFT4DEAD2
	virtual float GetTimescale( void ) const = 0; // l4d2 only
#endif

	virtual const char* GetName( void ) const = 0;
	virtual const char* GetMapName( void ) const = 0;
	virtual int GetSpawnCount( void ) const = 0;
	virtual int GetNumClasses( void ) const = 0;
	virtual int GetClassBits( void ) const = 0;
	virtual void GetNetStats( float &avgIn, float &avgOut ) = 0;
	virtual int GetNumPlayers( void ) = 0;
	virtual bool GetPlayerInfo( int nClientIndex, player_info_t *pinfo ) = 0;
	virtual bool IsActive( void ) const = 0;
	virtual bool IsLoading( void ) const = 0;
	virtual bool IsDedicated( void ) const = 0;
	virtual bool IsPaused( void ) const = 0;
	virtual bool IsMultiplayer( void ) const = 0;
	virtual bool IsPausable( void ) const = 0;
	virtual bool IsHLTV( void ) const = 0;
	virtual const char* GetPassword( void ) const = 0;
	virtual void SetPaused( bool paused ) = 0;

#if SOURCE_ENGINE == SE_LEFT4DEAD2
	virtual void SetTimescale( float flTimescale ) = 0; // l4d2 only
#endif

	virtual void SetPassword( const char *password ) = 0;
	virtual void BroadcastMessage( INetMessage &msg, bool onlyActive = false, bool reliable = false ) = 0;
	virtual void BroadcastMessage( INetMessage &msg, IRecipientFilter &filter ) = 0;
	virtual void DisconnectClient( IClient *client, const char *reason ) = 0;
	virtual float GetCPUUsage( void ) = 0;

public: // Other

	virtual void BroadcastPrintf( const char *fmt, ... ) = 0;
	virtual void SetMaxClients( int number ) = 0;
	virtual void WriteDeltaEntities( CBaseClient *client, CClientFrame *to, CClientFrame *from, bf_write &pBuf ) = 0;
	virtual void WriteTempEntities( CBaseClient *client, CFrameSnapshot *to, CFrameSnapshot *from, bf_write &pBuf, int nMaxEnts ) = 0;
	virtual void Init( bool isDedicated ) = 0;
	virtual void Clear( void ) = 0;
	virtual void Shutdown( void ) = 0;
	virtual CBaseClient* CreateFakeClient( const char *name ) = 0;
	virtual void RemoveClientFromGame( CBaseClient* client ) = 0;
	virtual void SendClientMessages( bool bSendSnapshots ) = 0;
	virtual void FillServerInfo( SVC_ServerInfo &serverinfo ) = 0;
	virtual void UserInfoChanged( int nClientIndex ) = 0;
	virtual void RejectConnection( const netadr_t &adr, char *fmt, ... ) = 0;
	virtual bool CheckIPRestrictions( const netadr_t &adr, int nAuthProtocol ) = 0;
	
	virtual IClient* ConnectClient( netadr_t &adr, int protocol, int challenge, int authProtocol, 
									const char *name, const char *password, const char *hashedCDkey, int cdKeyLen,
									CUtlVector< CLC_SplitPlayerConnect * > &splitScreenClients, bool isClientLowViolence ) = 0;

	virtual CBaseClient* CreateNewClient( int slot ) = 0;
	virtual bool FinishCertificateCheck( netadr_t &adr, int nAuthProtocol, const char *szRawCertificate ) = 0;
	virtual int GetChallengeNr( netadr_t &adr ) = 0;
	virtual int GetChallengeType( netadr_t &adr ) = 0;

#if SOURCE_ENGINE == SE_LEFT4DEAD2
	virtual bool CheckHostVersion( netadr_t &adr, int checkVersion ) = 0; // l4d2 only
#else
	virtual bool CheckProtocol( netadr_t& adr, int nProtocol ) = 0; // l4d1
#endif

	virtual bool CheckChallengeNr( netadr_t &adr, int nChallengeValue ) = 0;
	virtual bool CheckChallengeType( CBaseClient *client, int nNewUserID, netadr_t &adr, int nAuthProtocol, const char *pchLogonCookie, int cbCookie ) = 0;
	virtual bool CheckPassword(netadr_t &adr, const char *password, const char *name ) = 0;
	virtual void ReplyChallenge(netadr_t &adr, bf_read &msg ) = 0;
	virtual void ReplyServerChallenge( netadr_t &adr ) = 0;
	virtual void ReplyReservationRequest( netadr_t &adr, bf_read &msg ) = 0;
	virtual void CalculateCPUUsage( void ) = 0;
	virtual bool ShouldUpdateMasterServer( void ) = 0;
	virtual void UpdateMasterServerPlayers( void ) = 0;
	
	// vtable offset - 0
	server_state_t	m_State;		// 4, some actions are only valid during load (CBaseServer::IsActive)
	int				m_Socket;	// 8, network socket (CBaseServer::GetUDPPort)
	int				m_nTickCount;	// 12, current server tick (CBaseServer::GetTick)
	char			m_szMapname[64]; // 16, map name without path and extension (CBaseServer::Clear)
	char			m_szSkyname[64]; // 80, skybox name (CBaseServer::Clear)
	char			m_Password[32];	// 144, server password (CBaseServer::SetPassword)
	
	CRC32_t			worldmapCRC;  // 176, For detecting that client has a hacked local copy of map, the client will be dropped if this occurs. (CGameServer::SpawnServer)
	CRC32_t			clientDllCRC; // 180, The dll that this server is expecting clients to be using. (CGameServer::SpawnServer)
	CRC32_t			stringTableCRC; // 184, (CGameServer::SpawnServer)
	
	CNetworkStringTableContainer* m_StringTables; // 188, newtork string table container (CBaseServer::GetUserInfoTable)
	
	INetworkStringTable* m_pInstanceBaselineTable; // 192, (CBaseServer::GetInstanceBaselineTable)
	INetworkStringTable* m_pLightStyleTable; // 196, (CBaseServer::GetLightStyleTable)
	INetworkStringTable* m_pUserInfoTable; // 200, (CBaseServer::GetUserInfoTable)
	INetworkStringTable* m_pServerStartupTable; // 204, (CGameServer::SetQueryPortFromSteamServer)
	INetworkStringTable* m_pDownloadableFileTable; // 208, (CGameServer::CreateEngineStringTables)
	
	bf_write			m_Signon; // 212, (24 byte) (CBaseServer::Clear)
	CUtlMemory<byte>	m_SignonBuffer; // 236, (12 byte) (CBaseServer::Clear)

	int			serverclasses; // 248, number of unique server classes (CBaseServer::Clear)
	int			serverclassbits; // 252, log2 of serverclasses (CBaseServer::Clear)
	
	int			m_nUserid; // 256, increases by one with every new client (CBaseServer::GetNextUserID)

	int			m_nMaxclients; // 260, Current max # (CBaseServer::SetMaxClients)
	int			m_nSpawnCount; // 264, Number of servers spawned since start, used to check late spawns (e.g., when d/l'ing lots of data) (CBaseServer::GetSpawnCount)
	
	float		m_flTickInterval; // 268, time for 1 tick in seconds (CBaseServer::GetTickInterval)

#if SOURCE_ENGINE == SE_LEFT4DEAD2
	float		m_flTimescale; // 272, the game time scale (multiplied in conjunction with host_timescale) (CBaseServer::GetTimescale)
#endif

	CUtlVector<CBaseClient*>	m_Clients; // l4d2 - 276 (20 byte), l4d1 - 272, array of up to [maxclients] client slots. (CBaseServer::GetClient)

	bool		m_bIsDedicated; // l4d2 - 296, l4d1 - 292, (CBaseServer::IsDedicated)
	// 3 byte alignment
	
	CUtlVector<challenge_t> m_ServerQueryChallenges; // l4d2 - 300 (20 byte), l4d1 - 296, prevent spoofed IP's from server queries/connecting (CBaseServer::GetChallengeNr)

	float		m_fCPUPercent; // l4d2 - 320, l4d1 - 316, (CBaseServer::GetCPUUsage)
	float		m_fStartTime; // l4d2 - 324, l4d1 - 320, (CBaseServer::CalculateCPUUsage)
	float		m_fLastCPUCheckTime; // l4d2 - 328, l4d1 - 324, (CBaseServer::CalculateCPUUsage)
	
	// This is only used for Steam's master server updater to refer to this server uniquely.
	bool		m_bRestartOnLevelChange; // l4d2 - 332, l4d1 - 328, (CBaseServer::CheckMasterServerRequestRestart)
	bool		m_bMasterServerRulesDirty; // l4d2 - 333, l4d1 - 329, (CBaseServer::UpdateMasterServerRules)
	
	// 3 byte alignment
	double		m_flLastMasterServerUpdateTime; // l4d2 - 336 (8 byte), l4d1 - 332, (CBaseServer::UpdateMasterServer)

	struct SplitDisconnect_t
	{
		CBaseClient* m_pUser;
		CBaseClient* m_pSplit;
	};

	CUtlVector< SplitDisconnect_t >	m_QueuedForDisconnect; // l4d2 - 344 (20 byte), l4d1 - 340, (CBaseServer::QueueSplitScreenDisconnect)

	uint64		m_nReservationCookie; // l4d2 - 364 (8 byte), l4d1 - 360, if this server has been reserved, cookie that connecting clients must present (CBaseServer::GetReservationCookie)
	float		m_flReservationExpiryTime; // l4d2 - 372, l4d1 - 368, time at which reservation expires (CBaseServer::UpdateReservedState)
	float		m_flTimeLastClientLeft; // l4d2 - 376, l4d1 - 372, time when last client left server (CGameServer::UpdateHibernationState)
	int			m_numGameSlots;	// l4d2 - 380, l4d1 - 376, number of game slots allocated (CBaseServer::ConnectClient)
	CUtlString	m_GameType; // l4d2 - 384 (16 byte ?), l4d1 - 380, (CBaseServer::ClearTagStrings)

	CUtlVector<char> m_GameData; // l4d2 - 400 (20 byte), l4d1 - 396, (CBaseServer::GetGameData)
	int			m_GameDataVersion; // l4d2 - 420, l4d1 - 416, (CBaseServer::GetGameDataVersion)

#if SOURCE_ENGINE == SE_LEFT4DEAD // l4d1
	bool		m_bSteamMasterHeartbeatsEnabled; // l4d1 - 420 (CBaseServer::UpdateMasterServer)
	// 3 byte alignment
#endif	

	float		m_flTimeReservationGraceStarted; // l4d2 - 424, l4d1 - 424, time when client attempted to connect and was granted a reservation grace period (CBaseServer::CanAcceptChallengesFrom)
	netadr_t	m_adrReservationGraceStarted; // l4d2 - 428 (12 byte), l4d1 - 428, netadr of the client for whom reservation grace has been given (CBaseServer::CanAcceptChallengesFrom)
	
	struct ReservationStatus_t
	{
		ReservationStatus_t() : m_bActive(false), m_bSuccess(false)
		{
		}

		bool		m_bActive;
		bool		m_bSuccess;
		// 2 byte alignment
		netadr_t	m_Remote;
	};

	ReservationStatus_t m_ReservationStatus; // l4d2 - 440 (16 byte?), l4d1 - 440, (CBaseServer::ClearReservationStatus)
};
// size 456 l4d2 and l4d1

#endif // BASESERVER_H
