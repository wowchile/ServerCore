/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#ifndef EVENTMGR_H_
#define EVENTMGR_H_

#include "Common.h"
#include "Event.h"
#include <ace/Singleton.h>

class EventMgr
{
    friend class ACE_Singleton<EventMgr, ACE_Null_Mutex>;

    public:
        typedef std::vector<Event*> EventVector;

        EventMgr() { }
        ~EventMgr();

        void Load();

        Event* Create(const char* name, uint32 mapId, float x, float y, float z, uint32 phaseMask = 1, bool visible = false, uint32 time = 0, uint32 id = 0);

        void AddEvent(Event* e);
        void DelEvent(Event* e);

        Event* GetEventById(uint32 id);
        Event* GetEventByName(const char* name);
        Event* GetEventByConstructorGUID(uint64 guid, bool onlyInvisible = false);
        Event* GetEventByCreatureGUID(uint64 guid, bool onlyInvisible = false);
        Event* GetEventByGameObjectGUID(uint64 guid, bool onlyInvisible = false);

        const EventVector& GetEventVector() const { return _events; }

        bool IsValidCreature(uint32 rank, uint32 entry);
        bool IsValidGameObject(uint32 type, uint32 entry);

        uint32 GetUnusedId();
        uint32 GetMaxId();

    private:

        EventVector _events;
};

#define sEventMgr ACE_Singleton<EventMgr, ACE_Null_Mutex>::instance()

#endif /*EVENTMGR_H_*/
