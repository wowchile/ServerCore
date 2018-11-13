#ifndef WORLDQUERYCALLBACK_H_
#define WORLDQUERYCALLBACK_H_

#include "QueryResult.h"
#include "Callback.h"
#include "Language.h"
#include "Chat.h"
#include "World.h"
#include "IpCountryMgr.h"

// Proxy structure to contain data passed to callback function
// only to prevent bloating the parameter list
class PlayerInfoCommand
{
public:
    PlayerInfoCommand(uint32 accId, uint32 lowguid, const char* eMail, uint32 latency, int64 muteTime, uint8 raceid, uint8 classid, uint8 gender, uint32 totalPlayerTime, uint8 level, const char* alive, uint32 money, uint32 mapId, uint32 areaId, uint32 phase, WorldSession* session, Player* target, const char* targetName)
    : _accId(accId), _lowguid(lowguid), _eMail(eMail), _security(0), _latency(latency), _muteTime(muteTime), _muteReason("unknown"), _muteBy("unknown"), _raceid(raceid), _classid(classid), _gender(gender), _totalPlayerTime(totalPlayerTime), _level(level), _alive(alive), _money(money), _mapId(mapId), _areaId(areaId), _phase(phase), _session(session), _target(target), _targetName(targetName)
    {
        _userName = session->GetTrinityString(LANG_ERROR);
        _lastIp = session->GetTrinityString(LANG_ERROR);
        _lastLogin = session->GetTrinityString(LANG_ERROR);
    }

    // Account data print variables
    std::string _userName;
    uint32 _accId;
    uint32 _lowguid;
    std::string _eMail;
    uint32 _security;
    std::string _lastIp;
    std::string _lastLogin;
    uint32 _latency;

    // Mute data print variables
    int64 _muteTime;
    std::string _muteReason;
    std::string _muteBy;

    // Character data print variables
    uint8 _raceid, _classid; //RACE_NONE, CLASS_NONE
    uint8 _gender;
    uint32 _totalPlayerTime;
    uint8 _level;
    std::string _alive;
    uint32 _money;

    // Position data print
    uint32 _mapId;
    uint32 _areaId;
    uint32 _phase;

    WorldSession* _session;

    Player* _target;
    std::string _targetName;
};

class BcAccountInfo
{
public:
    BcAccountInfo(WorldSession* session) : _session(session) {}

    WorldSession* _session;
};

class WorldCallbackScript
{
public:
    QueryCallback<PreparedQueryResult, PlayerInfoCommand*, true> _pinfoCallback;
    QueryCallback<PreparedQueryResult, BcAccountInfo*, true> _bcAccInfoCallback;
};

#define sWorldCallbackScript ACE_Singleton<WorldCallbackScript, ACE_Null_Mutex>::instance()

#endif
