/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#ifndef HOUSE_H_
#define HOUSE_H_

#include "ObjectAccessor.h"
#include "HouseRoom.h"

enum CreatureHouses
{
    NPC_HOUSE_WARDEN = 5000001
};

enum HouseTypeId
{
    HOUSE_TYPE_NONE     = 0,
    HOUSE_TYPE_GUILD    = 1,
    HOUSE_TYPE_SHIP     = 2
};

#define MAX_ROOMS 30

class HouseRoom;

class House
{
    public:
        House(uint8 id, HouseTypeId typeId, TeamId teamId, uint64 wardenGuid, uint16 mapId, float x, float y, float z, bool available = true, bool phaseable = false) :
              _id(id), _typeId(typeId), _teamId(teamId), _wardenGuid(wardenGuid), _mapId(mapId), _x(x), _y(y), _z(z), _available(available), _phaseable(phaseable) { }
        ~House();

        uint8 GetHouseId() const { return _id; }
        HouseTypeId GetTypeId() const { return _typeId; }
        TeamId GetTeamId() const { return _teamId; }
        uint64 GetWardenGUID() const { return _wardenGuid; }
        uint32 GetMapId() const { return _mapId; }
        float GetPositionX() const { return _x; }
        float GetPositionY() const { return _y; }
        float GetPositionZ() const { return _z; }

        void Renew(uint32 guildId);
        void AddRoom(uint32 guildId, uint32 startTime, uint32 endTime, uint32 phaseMask = 1);
        HouseRoom* GetRoom(uint32 guildId);
        uint8 GetRoomsInUseSize() const;

        typedef std::vector<HouseRoom*> RoomSet;
        RoomSet::const_iterator GetRoomSetBegin() const { return _rooms.begin(); }
        RoomSet::const_iterator GetRoomSetEnd() const { return _rooms.end(); }

        void SetTypeId(HouseTypeId typeId);
        void SetTeamId(TeamId teamId);
        void SetAvailable(bool available);
        void SetPhaseable(bool phaseable);
        bool IsInUse() const;
        bool IsAvailable() const { return _available; }
        bool IsPhaseable() const { return _phaseable; }

        void Init();
        void Delete();

        void AddNearWardens(uint64 guid);
        bool AddWarden(uint64 guid);
        bool DelWarden(uint64 guid);
        bool IsWarden(uint64 guid) { return _wardens.find(guid) != _wardens.end(); }

        void AddPlayer(Player* player, Creature* warden);
        void DelPlayer(uint64 guid);
        bool IsInHouse(uint64 guid) const { return _players.find(guid) != _players.end(); }

        void Update(uint32 diff);

    private:
        void DeleteFromDB();

        typedef std::set<uint64> GuidSet;
        typedef std::map<uint64,uint32> PhasesMap;
        typedef std::map<uint64,uint64> PlayersMap;

        uint8 _id;
        HouseTypeId _typeId;
        TeamId _teamId;
        uint64 _wardenGuid;
        uint16 _mapId;
        float _x, _y, _z;
        bool _available;
        bool _phaseable;

        GuidSet _wardens;
        PlayersMap _players;
        PhasesMap _phases;
        RoomSet _rooms;
};

#endif /*HOUSE_H_*/
