/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#include "EventMgr.h"
#include "Event.h"

void Event::SetName(const char* name)
{
    // Si existe un evento bajo ese nombre
    if (sEventMgr->GetEventByName(name))
        return;

    _name = name;

    SaveToDB();
}

void Event::SetPhaseMask(uint32 phaseMask)
{
    _phaseMask = phaseMask;

    for (GuidSet::iterator itr = _creatures.begin(); itr != _creatures.end(); itr++)
    {
        if (Creature* creature = HashMapHolder<Creature>::Find(*itr))
            creature->SetPhaseMask(phaseMask, true);
        if (CreatureData const* data = sObjectMgr->GetCreatureData(GUID_LOPART(*itr)))
            const_cast<CreatureData*>(data)->phaseMask = phaseMask;
    }

    for (GuidSet::iterator itr = _gobjects.begin(); itr != _gobjects.end(); itr++)
    {
        if (GameObject* gobject = HashMapHolder<GameObject>::Find(*itr))
            gobject->SetPhaseMask(phaseMask, true);
        if (GameObjectData const* data = sObjectMgr->GetGOData(GUID_LOPART(*itr)))
            const_cast<GameObjectData*>(data)->phaseMask = phaseMask;
    }

    SaveToDB();
}

void Event::SetVisible(bool visible)
{
    _visible = visible;

    SaveToDB();
}

void Event::SetTime(uint32 time)
{
    _time = time;

    SaveToDB();
}

void Event::InsertCreature(uint64 creatureGUID, uint32 playerAccount)
{
    if (Creature* creature = HashMapHolder<Creature>::Find(creatureGUID))
    {
        if (playerAccount != 0)
        {
            PreparedStatement* stmt = NULL;
            stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_EVENT_CREATURE);
            stmt->setUInt16(0, GetEventId());
            stmt->setUInt32(1, creature->GetDBTableGUIDLow());
            stmt->setUInt32(2, playerAccount);
            WorldDatabase.Execute(stmt);
        }
        creature->SetPhaseMask(GetPhaseMask(), true);
    }

    if (CreatureData const* data = sObjectMgr->GetCreatureData(GUID_LOPART(creatureGUID)))
        const_cast<CreatureData*>(data)->phaseMask = GetPhaseMask();

    _creatures.insert(creatureGUID);
}

void Event::RemoveCreature(Creature* creature)
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENT_CREATURE);
    stmt->setUInt32(0, creature->GetDBTableGUIDLow());
    WorldDatabase.Execute(stmt);
    _creatures.erase(creature->GetGUID());
}

uint32 Event::GetAccountOfCreature(uint64 guid)
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENT_ACCOUNT_CREATURE);
    stmt->setUInt32(0, GUID_LOPART(guid));
    PreparedQueryResult result = WorldDatabase.Query(stmt);
    if (result)
    {
        Field* fields = result->Fetch();
        return fields[0].GetUInt32();
    }

    return 0;
}

void Event::InsertGameObject(uint64 gobjectGUID, uint32 playerAccount)
{
    if (GameObject* gobject = HashMapHolder<GameObject>::Find(gobjectGUID))
    {
        if (playerAccount != 0)
        {
            PreparedStatement* stmt = NULL;
            stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_EVENT_GAMEOBJECT);
            stmt->setUInt16(0, GetEventId());
            stmt->setUInt32(1, gobject->GetDBTableGUIDLow());
            stmt->setUInt32(2, playerAccount);
            WorldDatabase.Execute(stmt);
        }
        gobject->SetPhaseMask(GetPhaseMask(), true);
    }

    if (GameObjectData const* data = sObjectMgr->GetGOData(GUID_LOPART(gobjectGUID)))
        const_cast<GameObjectData*>(data)->phaseMask = GetPhaseMask();

    _gobjects.insert(gobjectGUID);
}

void Event::RemoveGameObject(GameObject* gobject)
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENT_GAMEOBJECT2);
    stmt->setUInt32(0, gobject->GetDBTableGUIDLow());
    WorldDatabase.Execute(stmt);
    _gobjects.erase(gobject->GetGUID());
}

uint32 Event::GetAccountOfGameObject(uint64 guid)
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENT_ACCOUNT_GAMEOBJECT);
    stmt->setUInt32(0, GUID_LOPART(guid));
    PreparedQueryResult result = WorldDatabase.Query(stmt);
    if (result)
    {
        Field* fields = result->Fetch();
        return fields[0].GetUInt32();
    }

    return 0;
}

void Event::AddConstructor(Player* player)
{
    _constructors.insert(player->GetGUID());
}

void Event::DelConstructor(Player* player)
{
    _constructors.erase(player->GetGUID());
}

void Event::SaveToDB()
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENT);
    stmt->setUInt16(0, GetEventId());
    PreparedQueryResult result = WorldDatabase.Query(stmt);
    if (result)
        stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_EVENT);
    else
        stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_EVENT);
    stmt->setString(0, GetName().c_str());
    stmt->setUInt32(1, GetMapId());
    stmt->setFloat(2, GetPositionX());
    stmt->setFloat(3, GetPositionY());
    stmt->setFloat(4, GetPositionZ());
    stmt->setUInt32(5, GetPhaseMask());
    stmt->setUInt32(6, (uint32)IsVisible());
    stmt->setUInt32(7, GetTime());
    stmt->setUInt16(8, GetEventId());
    WorldDatabase.Execute(stmt);
}

void Event::LoadCreatures()
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENT_CREATURES);
    stmt->setUInt16(0, GetEventId());
    PreparedQueryResult result = WorldDatabase.Query(stmt);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 lowGUID = fields[0].GetUInt32();
            if (CreatureData const* data = sObjectMgr->GetCreatureData(lowGUID))
                InsertCreature(MAKE_NEW_GUID(lowGUID, data->id, HIGHGUID_UNIT));
            else
                sLog->outError("No se pudo obtener Creature GUID: %u", lowGUID);
        } while (result->NextRow());
    }
    else
        sLog->outError("No se pudieron obtener Creatures en el evento %u", GetEventId());
}

void Event::LoadGameObjects()
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENT_GAMEOBJECTS);
    stmt->setUInt16(0, GetEventId());
    PreparedQueryResult result = WorldDatabase.Query(stmt);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 lowGUID = fields[0].GetUInt32();
            if (GameObjectData const* data = sObjectMgr->GetGOData(lowGUID))
                InsertGameObject(MAKE_NEW_GUID(lowGUID, data->id, HIGHGUID_GAMEOBJECT));
            else
                sLog->outError("No se pudo obtener el GObject GUID: %u", lowGUID);
        } while (result->NextRow());
    }
    else
        sLog->outError("No se pudieron obtener GObjects en el evento %u", GetEventId());
}

// Pero... si tratamos de borrar un Evento que no este en el grid?
void Event::RemoveFromDB()
{
    SetVisible(false);

    // Borramos cada criatura y gobject
    RemoveCreatures();
    RemoveGameObjects();

    // Borramos el evento
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENT);
    stmt->setUInt16(0, GetEventId());
    WorldDatabase.Execute(stmt);
}

// Pero... si tratamos de borrar un Evento que no este en el grid?
void Event::RemoveCreatures()
{
    for (GuidSet::iterator itr = _creatures.begin(); itr != _creatures.end(); itr++)
    {
        if (Creature* creature = HashMapHolder<Creature>::Find(*itr))
        {
            if (creature->GetGUID() < 10000001) // ONLY GM Spawns
                continue;

            // .npc del
            creature->CombatStop();
            creature->DeleteFromDB();
            creature->AddObjectToRemoveList();
        }
    }

    // Los removemos de la DB
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENT_CREATURES);
    stmt->setUInt16(0, GetEventId());
    WorldDatabase.Execute(stmt);
}

// Pero... si tratamos de borrar un Evento que no este en el grid?
void Event::RemoveGameObjects()
{
    for (GuidSet::iterator itr = _gobjects.begin(); itr != _gobjects.end(); itr++)
    {
        if (GameObject* gobject = HashMapHolder<GameObject>::Find(*itr))
        {
            if (gobject->GetGUID() < 10000001) // ONLY GM Spawns
                continue;

            // .gobject del
            gobject->SetRespawnTime(0);                                 // not save respawn time
            gobject->Delete();
            gobject->DeleteFromDB();
        }
    }

    // Los removemos de la DB
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENT_GAMEOBJECTS);
    stmt->setUInt16(0, GetEventId());
    WorldDatabase.Execute(stmt);
}
