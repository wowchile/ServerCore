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
#include "ScriptedCreature.h"

enum Npcs
{
    NPC_ARENA_BATTLEMASTER = 29533
};

enum Spells
{
    SPELL_ARCANE_EXPLOSION_VISUAL = 63660
};

class item_arena_battlemaster_summoner : public ItemScript
{
public:
    item_arena_battlemaster_summoner() : ItemScript("item_arena_battlemaster_summoner") { }

    bool OnUse (Player* player, Item* /*Item*/, SpellCastTargets const& /*targets*/)
    {
        Creature* battleMaster = player->FindNearestCreature(NPC_ARENA_BATTLEMASTER, 30.f);

        if (player->IsInCombat())
            player->GetSession()->SendNotification("No puedes invocar al Maestro de batalla de arena mientras estas en combate");
        else if (player->isMoving() || player->IsFlying() || player->IsFalling() || player->IsInFlight())
            player->GetSession()->SendNotification("No puedes invocar al Maestro de batalla de arena mientras estas en movimiento");
        else if (player->GetMap()->Instanceable() || player->GetTransport())
            player->GetSession()->SendNotification("No puedes invocar al Maestro de batalla de arena en este lugar");
        else if (battleMaster && battleMaster->IsEventVisibleFor(player))
            player->GetSession()->SendNotification("Ya existe un Maestro de batalla de arena invocado");
        else
        {
            battleMaster = player->SummonCreature(NPC_ARENA_BATTLEMASTER, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, MINUTE * IN_MILLISECONDS);

            // Hacemos que la criatura sea visible solo para el jugador que lo invoca
            battleMaster->SetEventVisibleFor(player, true);

            battleMaster->CastSpell(battleMaster, SPELL_ARCANE_EXPLOSION_VISUAL);

            return false;
        }

        return false;
    }
};

void AddSC_item_arena_battlemaster_summoner()
{
    new item_arena_battlemaster_summoner();
}