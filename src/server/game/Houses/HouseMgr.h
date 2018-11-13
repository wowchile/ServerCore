/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#ifndef HOUSEMGR_H_
#define HOUSEMGR_H_

#include "Common.h"
#include "Creature.h"
#include "GuildMgr.h"
#include "Object.h"
#include "House.h"
#include <ace/Singleton.h>

#define HOUSE_UPDATE_INTERVAL 1000

class WardenHouseCheck
{
    public:
        bool operator()(Creature* creature)
        {
            return creature && creature->IsAlive() && creature->GetEntry() == NPC_HOUSE_WARDEN;
        }
};

class HouseMgr
{
    friend class ACE_Singleton<HouseMgr, ACE_Null_Mutex>;

    public:
        HouseMgr() : _updateTimer(0) {}
        ~HouseMgr();

        void Load();
        void Create(HouseTypeId type, TeamId team, Creature* warden);

        void Register(House* house);
        void Unregister(House* house);

        void InitializeWarden(uint64 guid);

        typedef std::vector<House*> HouseSet;
        HouseSet::const_iterator GetHouseSetBegin() const { return _Houses.begin(); }
        HouseSet::const_iterator GetHouseSetEnd() const { return _Houses.end(); }

        House* GetHouseBy(uint32 id);
        House* GetHouseBy(Guild* guild);
        House* GetHouseBy(Player* player);
        House* GetHouseBy(Creature* creature);

        uint32 GetHouseIdBy(Guild* guild);
        uint32 GetHouseIdBy(Player* player);
        uint32 GetHouseIdBy(Creature* creature);

        uint32 GetUnusedId();
        uint32 GetMaxId();

        void Update(uint32 diff);

    protected:
        HouseSet _Houses;
        uint32 _updateTimer;
};

#define sHouseMgr ACE_Singleton<HouseMgr, ACE_Null_Mutex>::instance()

#endif /*HOUSEMGR_H_*/
