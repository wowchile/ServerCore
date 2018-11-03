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

#include "ScriptPCH.h"
#include "Guild.h"
#include "HouseMgr.h"

enum SpecialSpells
{
    SPELL_TELEPORT_TO_DALARAN   = 53360,
    SPELL_GHOST_NIGHT_ELF       = 20584,
    SPELL_GHOST                 = 8326
};

class npc_house_trigger : public CreatureScript
{
public:
    npc_house_trigger() : CreatureScript("npc_house_trigger") { }

    struct npc_house_triggerAI : public ScriptedAI
    {
        npc_house_triggerAI(Creature* creature) : ScriptedAI(creature)
        {
            tempSpawn = false;
            timeClear = 5 * MINUTE * IN_MILLISECONDS;
        }

        void Reset()
        {
            // Si el GUID de DB es igual a cero, fue spawneado o sumoneado de manera temporal
            if (me->GetDBTableGUIDLow() == 0)
            {
                tempSpawn = true;
                me->MonsterYell("No debo ser usado en modo temporal, desapareciendo en 5 segundos", LANG_UNIVERSAL, 0);
                me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                return;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (tempSpawn)
                return;

            // Verificar si se encuentra seleccionado
            House* house = sHouseMgr->GetHouseBy(me);
            if (!house)
            {
                sHouseMgr->InitializeWarden(me->GetGUID());
                return;
            }

            Player* player = who->ToPlayer();
            if (!player || player->GetExactDistSq(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()) > 50.0f * 50.0f)
                return;

            // Este protector de Houses ignora a GameMasters, jugadores con auras de fantasma, o teletransportandose.
            if (player->IsGameMaster() || player->HasAura(SPELL_GHOST_NIGHT_ELF) || player->HasAura(SPELL_GHOST) || player->IsBeingTeleported())
                return;

            if (sHouseMgr->GetHouseBy(player) == house)
            {
                house->AddPlayer(player, me);
                return;
            }

            bool alreadyTeleported = teleportedPlayers.find(player->GetGUID()) != teleportedPlayers.end();

            // Lo retornamos a la ultima posicion dependiendo de la lejania de esta y su nivel
            if (!alreadyTeleported && (me->GetMapId() != player->m_recallMap || me->GetDistance(player->m_recallX, player->m_recallY, player->m_recallZ) > 600.0f))
                player->TeleportTo(player->m_recallMap, player->m_recallX, player->m_recallY, player->m_recallZ, player->m_recallO);
            else if (!alreadyTeleported && (me->GetMapId() != player->m_homebindMapId || me->GetDistance(player->m_homebindX, player->m_homebindY, player->m_homebindZ) > 600.0f))
                player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, 0);
            else if (player->getLevel() > 70)
                player->CastSpell(player, SPELL_TELEPORT_TO_DALARAN, true);
            else
                player->TeleportTo(player->GetStartPosition());

            teleportedPlayers.insert(player->GetGUID());
        }

        void UpdateAI(uint32 diff)
        {
            if (diff > timeClear)
            {
                teleportedPlayers.clear();
                timeClear = 5 * MINUTE * IN_MILLISECONDS;
            }
            else
                timeClear -= diff;
        }

    private:
        bool tempSpawn;

        uint32 timeClear;

        typedef std::set<uint64> GuidSet;
        GuidSet teleportedPlayers;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_house_triggerAI(creature);
    }
};

void AddSC_house()
{
    new npc_house_trigger();
}
