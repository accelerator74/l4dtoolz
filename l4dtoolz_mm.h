#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <ISmmPlugin.h>

class l4dtoolz : public ISmmPlugin, public IConCommandBaseAccessor
{
public:
	bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	bool Unload(char *error, size_t maxlen);
	bool LevelInit(const char *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background);
	void LevelShutdown();
public:
	const char *GetAuthor();
	const char *GetName();
	const char *GetDescription();
	const char *GetURL();
	const char *GetLicense();
	const char *GetVersion();
	const char *GetDate();
	const char *GetLogTag();
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
