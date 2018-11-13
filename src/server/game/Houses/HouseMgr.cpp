/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#include "ScriptPCH.h"
#include "HouseMgr.h"

HouseMgr::~HouseMgr()
{
    for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
        delete *itr;
}

void HouseMgr::Load()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    House* house;
    uint32 houseId;
    HouseTypeId typeId;
    TeamId teamId;
    uint64 wardenGuid;
    uint16 mapId;
    float x, y, z;
    bool available;
    bool phaseable;

    uint32 guildId;
    uint32 startTime;
    uint32 endTime;

    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_HOUSES);
    PreparedQueryResult result = WorldDatabase.Query(stmt);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            houseId = fields[0].GetUInt8();
            typeId = HouseTypeId(fields[1].GetUInt8());
            teamId = TeamId(fields[2].GetUInt8());
            wardenGuid = MAKE_NEW_GUID(fields[3].GetUInt64(), NPC_HOUSE_WARDEN, HIGHGUID_UNIT);
            mapId = fields[4].GetUInt16();
            x = fields[5].GetFloat();
            y = fields[6].GetFloat();
            z = fields[7].GetFloat();
            available = fields[8].GetBool();
            phaseable = fields[9].GetBool();

            house = new House(houseId, typeId, teamId, wardenGuid, mapId, x, y, z, available, phaseable);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_HOUSE_ROOM);
            stmt->setUInt8(0, houseId);
            PreparedQueryResult result2 = CharacterDatabase.Query(stmt);
            if (result2)
            {
                do
                {
                    fields = result2->Fetch();
                    guildId = fields[0].GetUInt32();
                    startTime = fields[1].GetUInt32();
                    endTime = fields[2].GetUInt32();

                    house->AddRoom(guildId, startTime, endTime);

                    sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::Load] House Id: %u Rooom(GuildId: %u), cargada", houseId, guildId);

                }
                while (result2->NextRow());
            }

            Register(house);

            sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::Load] House Id: %u, cargada", houseId);

            count++;
        }
        while (result->NextRow());
    }
    else
    {
        sLog->outString(">> Loaded 0 houses. DB table `houses` is empty!");
        return;
    }

    sLog->outString(">> Loaded %u houses in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void HouseMgr::Create(HouseTypeId type, TeamId team, Creature* warden)
{
    House* house = new House(GetUnusedId(), type, team, warden->GetGUID(), warden->GetMapId(), warden->GetPositionX(), warden->GetPositionY(), warden->GetPositionZ());

    Register(house);

    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_HOUSE);
    stmt->setUInt8(0, house->GetHouseId());
    stmt->setUInt8(1, (uint8)house->GetTypeId());
    stmt->setUInt8(2, (uint8)house->GetTeamId());
    stmt->setUInt32(3, uint32(warden->GetGUIDLow()));
    stmt->setUInt16(4, warden->GetMapId());
    stmt->setFloat(5, warden->GetPositionX());
    stmt->setFloat(6, warden->GetPositionY());
    stmt->setFloat(7, warden->GetPositionZ());
    stmt->setUInt8(8, (uint8)house->IsAvailable());
    WorldDatabase.Execute(stmt);
}

void HouseMgr::Register(House* house)
{
    if (!house)
    {
        sLog->outError("Se trato de registrar una House invalida");
        return;
    }

    _Houses.push_back(house);
    house->Init();

    sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::Register] House (Id: %u) registrada", house->GetHouseId());
}

void HouseMgr::Unregister(House* house)
{
    if (!house)
    {
        sLog->outError("Se trato de eliminar del registro una House invalida");
        return;
    }

    for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
        if ((*itr)->GetHouseId() == house->GetHouseId())
        {
            sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::Load] House (Id: %u) eliminada del registro", house->GetHouseId());
            _Houses.erase(itr);
            return;
        }

    sLog->outError("Se trato de eliminar del registro una House (Id: %u) que no se encuentra registrada", house->GetHouseId());
}

void HouseMgr::InitializeWarden(uint64 guid)
{
    Creature* creature = HashMapHolder<Creature>::Find(guid);
    if (!creature)
        return;

    std::list<Creature*> wardens;
    WardenHouseCheck check;
    Trinity::CreatureListSearcher<WardenHouseCheck> searcher(creature, wardens, check);
    creature->VisitNearbyGridObject(100.0f, searcher);
    for (std::list<Creature*>::iterator itr = wardens.begin(); itr != wardens.end(); ++itr)
        if (House* house = GetHouseBy(*itr))
        {
            house->AddWarden(guid);
            break;
        }
}

House* HouseMgr::GetHouseBy(uint32 id)
{
    for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
    {
        ASSERT(*itr);

        if ((*itr)->GetHouseId() == id)
        {
            sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] House de ID: %u, encontrada", id);
            return (*itr);
        }
    }

    sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] No se pudo encontrar la House de ID: %u", id);

    return NULL;
}

House* HouseMgr::GetHouseBy(Guild* guild)
{
    if (guild)
    {
        for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
        {
            ASSERT(*itr);

            if ((*itr)->GetRoom(guild->GetId()))
            {
                sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] House ID: %u, encontrada para la guild ID: %u", (*itr)->GetHouseId(), guild->GetId());
                return (*itr);
            }
        }

        sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] No se pudo encontrar una House para la guild ID: %u", guild->GetId());
    }
    else
        sLog->outError("Se trato de obtener una House con una guild invalida");

    return NULL;
}

House* HouseMgr::GetHouseBy(Player* player)
{
    if (player)
    {
        for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
        {
            ASSERT(*itr);

            if ((*itr)->GetRoom(player->GetGuildId()))
            {
                sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] House ID: %u, encontrada para el jugador GUID: %u", (*itr)->GetHouseId(), player->GetGUIDLow());
                return (*itr);
            }
        }

        sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] No se pudo encontrar una House para el jugador GUID: %u", player->GetGUIDLow());
    }
    else
        sLog->outError("Se trato de obtener una House con un jugador invalido");

    return NULL;
}

House* HouseMgr::GetHouseBy(Creature* creature)
{
    if (creature)
    {
        for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
        {
            ASSERT(*itr);

            sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] Buscando criatura GUID: %u en House ID: %u", creature->GetGUIDLow(), (*itr)->GetHouseId());
            if ((*itr)->IsWarden(creature->GetGUID()))
            {
                sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] House ID: %u, encontrada para la criatura GUID: %u", (*itr)->GetHouseId(), creature->GetGUIDLow());
                return (*itr);
            }
        }

        sLog->outDebug(LOG_FILTER_HOUSE, "[HouseMgr::GetHouseBy] No se pudo encontrar una House para la criatura GUID: %u", creature->GetGUIDLow());
    }
    else
        sLog->outError("Se trato de obtener una House con una criatura invalida");

    return NULL;
}

uint32 HouseMgr::GetHouseIdBy(Guild* guild)
{
    if (House* house = GetHouseBy(guild))
        return house->GetHouseId();
    return 0;
}

uint32 HouseMgr::GetHouseIdBy(Player* player)
{
    if (House* house = GetHouseBy(player))
        return house->GetHouseId();
    return 0;
}

uint32 HouseMgr::GetHouseIdBy(Creature* creature)
{
    if (House* house = GetHouseBy(creature))
        return house->GetHouseId();
    return 0;
}

uint32 HouseMgr::GetUnusedId()
{
    uint32 id = 1;
    while (GetHouseBy(id))
        id++;

    return id;
}

uint32 HouseMgr::GetMaxId()
{
    uint32 id = 0;
    for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
    {
        if (id < (*itr)->GetHouseId())
            id = (*itr)->GetHouseId();
    }

    return id;
}

void HouseMgr::Update(uint32 diff)
{
    _updateTimer += diff;
    if (_updateTimer > HOUSE_UPDATE_INTERVAL)
    {
        for (HouseSet::iterator itr = _Houses.begin(); itr != _Houses.end(); itr++)
            if ((*itr)->IsInUse() || (*itr)->IsPhaseable())
                (*itr)->Update(diff);

        _updateTimer = 0;
    }
}
