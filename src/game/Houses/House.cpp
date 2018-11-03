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
#include "House.h"

House::~House()
{
    for (RoomSet::iterator itr = _rooms.begin(); itr != _rooms.end(); itr++)
        delete *itr;

    _wardens.clear();
    _players.clear();
    _phases.clear();
}

void House::Init()
{
    _wardens.insert(GetWardenGUID());
}

void House::SetTypeId(HouseTypeId typeId)
{
    _typeId = typeId;
    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_HOUSE_TYPE);
    stmt->setUInt8(0, typeId);
    stmt->setUInt8(1, GetHouseId());
    WorldDatabase.Execute(stmt);
}

void House::SetTeamId(TeamId teamId)
{
    _teamId = teamId;
    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_HOUSE_TEAM);
    stmt->setUInt8(0, teamId);
    stmt->setUInt8(1, GetHouseId());
    WorldDatabase.Execute(stmt);
}

void House::SetAvailable(bool available)
{
    _available = available;
    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_HOUSE_AVAILABLE);
    stmt->setUInt8(0, (uint8)available);
    stmt->setUInt8(1, GetHouseId());
    WorldDatabase.Execute(stmt);
}

void House::SetPhaseable(bool phaseable)
{
    _phaseable = phaseable;
    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_HOUSE_PHASEABLE);
    stmt->setUInt8(0, (uint8)phaseable);
    stmt->setUInt8(1, GetHouseId());
    WorldDatabase.Execute(stmt);
}


void House::DeleteFromDB()
{
    PreparedStatement* stmt = NULL;
    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_HOUSE);
    stmt->setUInt32(0, GetHouseId());
    WorldDatabase.Execute(stmt);
}

void House::Delete()
{
    DeleteFromDB();

    sHouseMgr->Unregister(this);

    delete this;
}

bool House::AddWarden(uint64 guid)
{
    Creature* creature = HashMapHolder<Creature>::Find(guid);

    if (!creature || creature->GetEntry() != NPC_HOUSE_WARDEN || IsWarden(guid))
    {
        sLog->outDebug(LOG_FILTER_HOUSE, "[House::AddWarden] Guid: %u, descartado", uint32(GUID_LOPART(guid)));
        return false;
    }

    _wardens.insert(guid);

    return true;
}

bool House::DelWarden(uint64 guid)
{
    Creature* creature = HashMapHolder<Creature>::Find(guid);

    if (!creature || creature->GetEntry() != NPC_HOUSE_WARDEN || !IsWarden(guid) || creature->GetGUID() == GetWardenGUID())
    {
        sLog->outDebug(LOG_FILTER_HOUSE, "[House::DelWarden] Guid: %u, descartado", uint32(GUID_LOPART(guid)));
        return false;
    }

    _wardens.erase(guid);

    return true;
}

uint8 House::GetRoomsInUseSize() const
{
    uint8 count = 0;
    for (RoomSet::const_iterator itr = _rooms.begin(); itr != _rooms.end(); itr++)
        if ((*itr)->IsInUse())
            count++;

    return count;
}

bool House::IsInUse() const
{
    uint8 count = 0;
    for (RoomSet::const_iterator itr = _rooms.begin(); itr != _rooms.end(); itr++)
        if ((*itr)->IsInUse())
        {
            if (IsPhaseable())
                count++;
            else
                return true;
        }

    return count == MAX_ROOMS;
}

void House::Renew(uint32 guildId)
{
    PreparedStatement* stmt = NULL;

    uint32 startTime;
    uint32 endTime;

    HouseRoom* room = GetRoom(guildId);
    if (room)
    {
        endTime = room->GetEndTime() + 30 * DAY;

        if (IsPhaseable())
            endTime += 15 * DAY;

        room->SetEndTime(endTime);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_HOUSE_ROOM_END_TIME);
        stmt->setUInt32(0, endTime);
        stmt->setUInt8(1, GetHouseId());
        stmt->setUInt32(2, room->GetGuildId());
        stmt->setUInt32(3, room->GetStartTime());
    }
    else
    {
        startTime = time(NULL);
        endTime = time(NULL) + 30 * DAY;

        if (IsPhaseable())
            endTime += 15 * DAY;

        AddRoom(guildId, startTime, endTime);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_HOUSE_ROOM);
        stmt->setUInt8(0, GetHouseId());
        stmt->setUInt32(1, guildId);
        stmt->setUInt32(2, endTime);
    }

    CharacterDatabase.Execute(stmt);
}

void House::AddRoom(uint32 guildId, uint32 startTime, uint32 endTime, uint32 phaseMask)
{
    HouseRoom* room = GetRoom(guildId);
    if (room)
    {
        sLog->outDebug(LOG_FILTER_HOUSE, "[House::AddRoom] GuildId: %u, ya existe", guildId);
        return;
    }

    if (IsInUse())
    {
        sLog->outDebug(LOG_FILTER_HOUSE, "[House::AddRoom] House no tiene disponibilidad");
        return;
    }

    for (RoomSet::iterator itr = _rooms.begin(); itr != _rooms.end(); itr++)
        if ((*itr)->GetPhaseMask() == phaseMask && (*itr)->IsInUse())
            phaseMask *= 2;

    sLog->outDebug(LOG_FILTER_HOUSE, "[House::AddRoom] Se ha añadido Room para GuildId: %u, endTime: %u, startTime: %u, phaseMask: %u", guildId, endTime, startTime, phaseMask);

    _rooms.push_back(new HouseRoom(GetHouseId(), guildId, startTime, endTime, phaseMask));
}

HouseRoom* House::GetRoom(uint32 guildId)
{
    for (RoomSet::iterator itr = _rooms.begin(); itr != _rooms.end(); itr++)
        if ((*itr)->GetGuildId() == guildId && (*itr)->IsInUse())
        {
            sLog->outDebug(LOG_FILTER_HOUSE, "[House::GetRoom] Se ha encontrado Room para GuildId: %u", guildId);
            return (*itr);
        }
        else
            sLog->outDebug(LOG_FILTER_HOUSE, "[House::GetRoom] Se ha saltado Room de GuiildId: %u, IsInUse: %u.", (*itr)->GetGuildId(), (*itr)->IsInUse() ? 1 : 0);

    sLog->outDebug(LOG_FILTER_HOUSE, "[House::GetRoom] No se ha encontrado Room para GuildId: %u", guildId);

    return NULL;
}

void House::AddPlayer(Player* player, Creature* warden)
{
    bool inHouse = IsInHouse(player->GetGUID());

    _players[player->GetGUID()] = warden->GetGUID();

    // Si GetRoomsInUseSize() > 1, se le ha desactivado el sistema de fases
    // Si llega a encontrarlo en la fases, seguramente es la ultima guild en proceso de desactivacion de fase, asique solo hay que limpiar
    bool hasPhase = _phases.find(player->GetGUID()) != _phases.end();
    bool disablingPhase = !IsPhaseable() && (GetRoomsInUseSize() > 1 || hasPhase);

    if (inHouse && !disablingPhase)
        return;

     // Si GetRoomsInUseSize() > 1, se le ha desactivado el sistema de fases
    if (IsPhaseable() || GetRoomsInUseSize() > 1)
    {
        HouseRoom* room = GetRoom(player->GetGuildId());
        if (player->GetPhaseMask() != room->GetPhaseMask())
        {
            _phases[player->GetGUID()] = player->GetPhaseMask();
            player->SetPhaseMask(room->GetPhaseMask(), true);
        }
    }
    // Si llega a encontrarlo en la fases, seguramente es la ultima guild en proceso de desactivacion de fase, asique solo hay que limpiar
    else if (hasPhase)
        return DelPlayer(player->GetGUID());
}

void House::DelPlayer(uint64 guid)
{
    if (!IsInHouse(guid))
        return;

    _players.erase(guid);

    // Si GetRoomsInUseSize() > 1, se le ha desactivado el sistema de fases
    // Si llega a encontrarlo en la fases, seguramente es la ultima guild en proceso de desactivacion de fase, asique solo hay que limpiar
    bool hasPhase = _phases.find(guid) != _phases.end();
    bool disablingPhase = !IsPhaseable() && (GetRoomsInUseSize() > 1 || hasPhase);

    if (IsPhaseable() || disablingPhase)
    {
        if (hasPhase)
        {
            if (Player* player = ObjectAccessor::FindConnectedPlayer(guid))
                player->SetPhaseMask(_phases[guid], true);
            _phases.erase(guid);
        }
    }
}

void House::Update(uint32 /*diff*/)
{
    PlayersMap::iterator next;
    for (PlayersMap::iterator itr = _players.begin(); itr != _players.end(); itr = next)
    {
        next = itr;
        next++;

        Player* player = ObjectAccessor::FindConnectedPlayer((*itr).first);
        Creature* warden = HashMapHolder<Creature>::Find((*itr).second);

        if (warden && player)
        {
            // Si GetRoomsInUseSize() > 1, se le ha desactivado el sistema de fases
            if (IsPhaseable() || GetRoomsInUseSize() > 1)
            {
                HouseRoom* room = GetRoom(player->GetGuildId());

                // Si por alguna razon perdio la fase :S
                if (room && player->GetPhaseMask() != room->GetPhaseMask())
                {
                    _phases[player->GetGUID()] = player->GetPhaseMask();
                    player->SetPhaseMask(room->GetPhaseMask(), true);
                }
            }

            if (warden->IsInMap(player) && warden->GetDistance(player) <= 100.0f)
                continue;
        }

        DelPlayer((*itr).first);
    }
}
