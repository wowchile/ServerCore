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

enum shirt_ilusion
{
    // Item template
    ITEM_ORC_FEMALE             = 1000025,
    ITEM_ORC_MALE               = 1000026,
    ITEM_TROLL_FEMALE           = 1000027,
    ITEM_TROLL_MALE             = 1000028,
    ITEM_NIGHT_ELF_FEMALE       = 1000029,
    ITEM_NIGHT_ELF_MALE         = 1000030,
    ITEM_DWARF_MALE             = 1000031,
    ITEM_DWARF_FEMALE           = 1000032,
    ITEM_DRAENEI_MALE           = 1000033,
    ITEM_DRAENEI_FEMALE         = 1000034,
    ITEM_GOBLIN_FEMALE          = 1000035,
    ITEM_GOBLIN_MALE            = 1000036,

    DISPLAY_ORC_FEMALE          = 6768,
    DISPLAY_ORC_MALE            = 6785,
    DISPLAY_TROLL_FEMALE        = 4358,
    DISPLAY_TROLL_MALE          = 11193,
    DISPLAY_NIGHT_ELF_FEMALE    = 2222,
    DISPLAY_NIGHT_ELF_MALE      = 2245,
    DISPLAY_DWARF_FEMALE        = 7001,
    DISPLAY_DWARF_MALE          = 5737,
    DISPLAY_DRAENEI_FEMALE      = 17155,
    DISPLAY_DRAENEI_MALE        = 19171,
    DISPLAY_GOBLIN_FEMALE       = 19161,
    DISPLAY_GOBLIN_MALE         = 19000,

};

class item_custom_shirt_ilusion : public ItemScript
{
public:
    item_custom_shirt_ilusion() : ItemScript("item_custom_shirt_ilusion"){}


    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
    {
        uint32 itemId = item->GetEntry();

        switch (itemId)
        {
        case ITEM_ORC_FEMALE:
            player->SetDisplayId(DISPLAY_ORC_FEMALE);
                break;
        case ITEM_ORC_MALE:
            player->SetDisplayId(DISPLAY_ORC_MALE);
                break;
        case ITEM_TROLL_FEMALE:
            player->SetDisplayId(DISPLAY_TROLL_FEMALE);
                break;
        case ITEM_TROLL_MALE:
            player->SetDisplayId(ITEM_TROLL_MALE);
                break;
        case ITEM_NIGHT_ELF_FEMALE:
            player->SetDisplayId(DISPLAY_NIGHT_ELF_FEMALE);
                break;
        case ITEM_NIGHT_ELF_MALE:
            player->SetDisplayId(DISPLAY_NIGHT_ELF_MALE);
            break;
        case ITEM_DWARF_MALE:
            player->SetDisplayId(DISPLAY_DWARF_MALE);
            break;
        case ITEM_DWARF_FEMALE:
            player->SetDisplayId(DISPLAY_DRAENEI_MALE);
            break;
        case ITEM_DRAENEI_MALE:
            player->SetDisplayId(DISPLAY_DRAENEI_MALE);
            break;
        case ITEM_DRAENEI_FEMALE:
            player->SetDisplayId(DISPLAY_DRAENEI_FEMALE);
            break;
        case ITEM_GOBLIN_FEMALE:
            player->SetDisplayId(DISPLAY_GOBLIN_FEMALE);
            break;
        case ITEM_GOBLIN_MALE:
            player->SetDisplayId(DISPLAY_GOBLIN_MALE);
            break;
        }
        return true;
    }
};

void AddSC_item_custom_shirt_ilusion()
{
    new item_custom_shirt_ilusion();
}