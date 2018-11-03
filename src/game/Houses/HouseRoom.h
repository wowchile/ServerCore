/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#ifndef HOUSE_ROOM_H_
#define HOUSE_ROOM_H_

#include "House.h"

class House;

class HouseRoom
{
    public:
        HouseRoom(uint8 houseId, uint32 guildId, uint32 startTime, uint32 endTime, uint32 phaseMask) :
              _houseId(houseId), _guildId(guildId), _startTime(startTime), _endTime(endTime), _phaseMask(phaseMask) { }
        ~HouseRoom() {}

        uint32 GetHouseId() const { return _houseId; }
        House* GetHouse() const;

        uint32 GetGuildId() const { return _guildId; }
        uint32 GetStartTime() const { return _startTime; }
        uint32 GetEndTime() const { return _endTime; }
        uint32 GetPhaseMask() const { return _phaseMask; }

        void SetEndTime(uint32 endTime) { _endTime = endTime; }

        bool IsInUse() const { return GetGuildId() != 0 && GetEndTime() > uint32(time(NULL)); }

    private:
        uint8 _houseId;
        uint32 _guildId;
        uint32 _startTime;
        uint32 _endTime;
        uint32 _phaseMask;
};

#endif /*HOUSE_ROOM_H_*/
