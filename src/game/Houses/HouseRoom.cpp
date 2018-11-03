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
#include "HouseRoom.h"

House* HouseRoom::GetHouse() const
{
    return sHouseMgr->GetHouseBy(_houseId);
}
