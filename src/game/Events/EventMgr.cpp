/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#include "Common.h"
#include "Creature.h"
#include "EventMgr.h"

EventMgr::~EventMgr()
{
    for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
        delete *itr;
}

void EventMgr::Load()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    uint32 id;
    const char* name;
    uint32 mapId;
    float x, y, z;
    uint32 phaseMask;
    bool visible;
    uint32 time;

    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENTS);
    PreparedQueryResult result = WorldDatabase.Query(stmt);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            id = fields[0].GetUInt16();
            name = fields[1].GetCString();
            mapId = fields[2].GetUInt16();
            x = fields[3].GetFloat();
            y = fields[4].GetFloat();
            z = fields[5].GetFloat();
            phaseMask = fields[6].GetUInt32();
            visible = fields[7].GetBool();
            time = fields[8].GetUInt32();

            Create(name, mapId, x, y, z, phaseMask, visible, time, id);

            count++;
        }
        while (result->NextRow());
    }
    else
    {
        sLog->outString(">> Loaded 0 events. DB table `event` is empty!");
        return;
    }

    sLog->outString(">> Loaded %u events in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

Event* EventMgr::Create(const char* name, uint32 mapId, float x, float y, float z, uint32 phaseMask, bool visible, uint32 time, uint32 id)
{
    if (GetEventByName(name))
        return NULL;

    bool fromDB = id != 0;

    Event* e = new Event(name, mapId, x, y, z, phaseMask, visible, time, fromDB ? id : sEventMgr->GetUnusedId());
    if (fromDB)
    {
        e->LoadCreatures();
        e->LoadGameObjects();
    }
    else
       e->SaveToDB();

    AddEvent(e);

    return e;
}

void EventMgr::AddEvent(Event* e)
{
    _events.push_back(e);
}

void EventMgr::DelEvent(Event* e)
{
    for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
        if ((*itr)->GetEventId() == e->GetEventId())
        {
            (*itr)->RemoveFromDB();
            delete (*itr);
            _events.erase(itr);
            return;
        }
}

Event* EventMgr::GetEventById(uint32 id)
{
    for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
        if ((*itr)->GetEventId() == id)
            return (*itr);

    return NULL;
}

Event* EventMgr::GetEventByName(const char* name)
{
    for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
        if ((*itr)->GetName().compare(name) == 0)
            return (*itr);

    return NULL;
}

Event* EventMgr::GetEventByConstructorGUID(uint64 guid, bool onlyInvisible)
{
    for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
    {
        if (onlyInvisible && (*itr)->IsVisible())
            continue;

        if ((*itr)->IsConstructor(guid))
            return (*itr);
    }

    return NULL;
}

uint32 EventMgr::GetUnusedId()
{
    uint32 id = 1;
    while (GetEventById(id))
        id++;

    return id;
}

uint32 EventMgr::GetMaxId()
{
    uint32 id = 0;
    for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
        if (id < (*itr)->GetEventId())
            id = (*itr)->GetEventId();

    return id;
}

bool EventMgr::IsValidCreature(uint32 rank, uint32 entry)
{
    switch (entry)
    {
        case 31146: // Raider's Training Dummy
            return true;
    }

    switch (rank)
    {
        case CREATURE_ELITE_WORLDBOSS:
            return false;
        default:
            return true;
    }
}

bool EventMgr::IsValidGameObject(uint32 type, uint32 entry)
{
    switch (entry)
    {
        case 1617: // Silverleaf
        case 1618: // Peacebloom
        case 1731: // Copper Vein
        case 1733: // Silver Vein
        case 1735: // Iron Deposit
        case 181556: // Adamantite Deposit
        case 181557: // Khorium Vein
        case 189973: // Goldclover
        case 189978: // Cobalt Deposit
        case 189979: // Rich Cobalt Deposit
        case 189980: // Saronite Deposit
        case 189981: // Rich Saronite Deposit
        case 190169: // Tiger Lily
        case 190170: // Talandra's Rose
        case 190171: // Lichbloom
        case 190172: // Icethorn
        case 191019: // Adder's Tongue
        case 191133: // Titanium Vein
        case 192046: // Musselback Sculpin School
        case 192050: // Glacial Salmon School
        case 192053: // Deep Sea Monsterbelly School
        case 192057: // Nettlefish School
        case 202083: // Razormaw Matriarch's Nest
            return true;
    }

    switch (type)
    {
        case GAMEOBJECT_TYPE_CHEST:
            return false;
        default:
            return true;
    }
}

Event* EventMgr::GetEventByCreatureGUID(uint64 guid, bool onlyInvisible)
{
    if (guid >= EVENT_CREATURE_BASE_GUID)
        for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
        {
            if (onlyInvisible && (*itr)->IsVisible())
                continue;

            if ((*itr)->IsEventCreature(guid))
                return (*itr);
        }

    return NULL;
}

Event* EventMgr::GetEventByGameObjectGUID(uint64 guid, bool onlyInvisible)
{
    if (guid >= EVENT_GOBJECT_BASE_GUID)
        for (EventVector::iterator itr = _events.begin(); itr != _events.end(); itr++)
        {
            if (onlyInvisible && (*itr)->IsVisible())
                continue;

            if ((*itr)->IsEventGameObject(guid))
                return (*itr);
        }

    return NULL;
}
