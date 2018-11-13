/*
 * Copyright (C) 2010-2014 BattleCore3 <http://www.battle.cl/>
 *
 * BattleCore es un proyecto privado basado en TrinityCore
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 *  ____        _   _   _         ____                
 * | __ )  __ _| |_| |_| | ___   / ___|___  _ __ ___  
 * |  _ \ / _` | __| __| |/ _ \ | |   / _ \| '__/ _ \
 * | |_) | (_| | |_| |_| |  __/ | |__| (_) | | |  __/ 
 * |____/ \__,_|\__|\__|_|\___|  \____\___/|_|  \___|
 *                                               _____
 *                                              |__  /
 *                                               /_ < 
 *                                             ___/ / 
 *                                            /____/  
 */

#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"

class item_change_rate : public ItemScript
{
public:
    item_change_rate() : ItemScript("item_change_rate"){}

    bool OnUse(Player * player, Item * /*item*/, SpellCastTargets const& targets)
    {
        if (player->HasBlizzLootRate())
        {
            player->SetBlizzLootRate(false);
            player->GetSession()->SendNotification("|CFF7BBEF7[Loot Especial]|r:Loot x1 Desactivado");
        }
        else
        {
            player->SetBlizzLootRate(true);
            player->GetSession()->SendNotification("|CFF7BBEF7[Loot Especial]|r:Loot x1 Activado");
        }
        return true;
    }
};

void AddSC_item_change_rates()
{
    new item_change_rate();
}