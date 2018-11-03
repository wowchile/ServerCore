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
#include "Chat.h"
#include "Player.h"
#include "Pet.h"
#include "World.h"
#include "ArenaTeamMgr.h"
#include "GuildMgr.h"
#include "HouseMgr.h"
#include "EventMgr.h"
#include "AccountMgr.h"
#include "Language.h"
#include "IpCountryMgr.h"
#include "../world_query_callback.h"

class command_bc : public CommandScript
{
public:
    command_bc() : CommandScript("command_bc") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> houseCommandTable =
        {
            { "add",                    RBAC_PERM_COMMAND_BC_HOUSE_ADD,         false, &HandleHouseAddCommand,          ""},
            { "del",                    RBAC_PERM_COMMAND_BC_HOUSE_DEL,         false, &HandleHouseDelCommand,          ""},
            { "available",              RBAC_PERM_COMMAND_BC_HOUSE_AVAILABLE,   false, &HandleHouseAvailableCommand,    ""},
            { "phaseable",              RBAC_PERM_COMMAND_BC_HOUSE_PHASEABLE,   false, &HandleHousePhaseableCommand,    ""},
            { "info",                   RBAC_PERM_COMMAND_BC_HOUSE_INFO,        false, &HandleHouseInfoCommand,         ""},
            { "list",                   RBAC_PERM_COMMAND_BC_HOUSE_LIST,        false, &HandleHouseListCommand,         ""}
        };

        static std::vector<ChatCommand> eventCommandTable =
        {
            { "add",                    RBAC_PERM_COMMAND_BC_EVENT_ADD,         false, &HandleEventAddCommand,          ""},
            { "del",                    RBAC_PERM_COMMAND_BC_EVENT_DEL,         false, &HandleEventDelCommand,          ""},
            { "rename",                 RBAC_PERM_COMMAND_BC_EVENT_RENAME,      false, &HandleEventRenameCommand,       ""},
            { "info",                   RBAC_PERM_COMMAND_BC_EVENT_INFO,        false, &HandleEventInfoCommand,         ""},
            { "appear",                 RBAC_PERM_COMMAND_BC_EVENT_APPEAR,      false, &HandleEventAppearCommand,       ""},
            { "list",                   RBAC_PERM_COMMAND_BC_EVENT_LIST,        false, &HandleEventListCommand,         ""},
            { "visible",                RBAC_PERM_COMMAND_BC_EVENT_VISIBLE,     false, &HandleEventVisibleCommand,      ""},
            { "phasemask",              RBAC_PERM_COMMAND_BC_EVENT_PHASEMASK,   false, &HandleEventPhaseMaskCommand,    ""},
            { "build",                  RBAC_PERM_COMMAND_BC_EVENT_BUILD,       false, &HandleEventBuildCommand,        ""}
        };

        static std::vector<ChatCommand> debugCommandTable =
        {
            { "worldstate",             RBAC_PERM_COMMAND_BC_DEBUG_WORLDSTATE,  false, &HandleDebugWorldStateCommand,   ""},
            { "isinrange",              RBAC_PERM_COMMAND_BC_DEBUG_ISINRANGE,   false, &HandleDebugIsInRangeCommand,    ""},
            { "worldpacket",            RBAC_PERM_COMMAND_BC_DEBUG_WORLDPACKET, false, &HandleDebugWorldPacketCommand,  ""}
        };

        static std::vector<ChatCommand> serverCommandTable =
        {
            { "lock",                   RBAC_PERM_COMMAND_BC_SERVER_LOCK,        true, &HandleServerLockCommand,        ""},
        };

        static std::vector<ChatCommand> instanceCommandTable =
        {
            { "freeze",                 RBAC_PERM_COMMAND_BC_INSTANCE_FREEZE,   false, &HandleInstanceFreezeCommand,    ""},
            { "unfreeze",               RBAC_PERM_COMMAND_BC_INSTANCE_UNFREEZE, false, &HandleInstanceUnFreezeCommand,  ""}
        };

        static std::vector<ChatCommand> reloadCommandTable =
        {
            { "ipcountry",               RBAC_PERM_COMMAND_BC_RELOAD_IPCOUNTRY,  true, &HandleReloadIpCountryHandleCommand, ""},
            { "account_access",          RBAC_PERM_COMMAND_BC_RELOAD_ACCESS,     true, &HandleReloadAccessHandleCommand,""}
        };

        static std::vector<ChatCommand> bcCommandTable =
        {
            { "server",                 RBAC_PERM_COMMAND_BC_SERVER,             true, NULL,              "", serverCommandTable},
            { "debug",                  RBAC_PERM_COMMAND_BC_DEBUG,             false, NULL,               "", debugCommandTable},
            { "house",                  RBAC_PERM_COMMAND_BC_HOUSE,             false, NULL,               "", houseCommandTable},
            { "event",                  RBAC_PERM_COMMAND_BC_EVENT,             false, NULL,               "", eventCommandTable},
            { "instance",               RBAC_PERM_COMMAND_BC_INSTANCE,          false, NULL,            "", instanceCommandTable},
            { "reload",                 RBAC_PERM_COMMAND_BC_RELOAD,             true, NULL,              "", reloadCommandTable},
            { "resetcd",                RBAC_PERM_COMMAND_BC_RESETCD,           false, &HandleResetCdCommand,           ""},
            { "accinfo",                RBAC_PERM_COMMAND_BC_ACCINFO,           false, &HandleAccountInfoCommand,       ""}
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "bc",                     RBAC_PERM_COMMAND_BC,                    true,  NULL,                 "", bcCommandTable}
        };

        return commandTable;
    }

    // .bc server lock $level
    static bool HandleServerLockCommand(ChatHandler* handler,const char*args)
    {
        if (!*args)
            return false;

        char* command = strtok((char*)args, " ");
        if(!command || !isNumeric(command))
        {
            handler->PSendSysMessage("El servidor esta abierto para cuentas con seguridad: '%u'",sWorld->GetPlayerSecurityLimit());
            return true;
        }

        int type = atoi(command);
        if (type < SEC_PLAYER || type > SEC_ADMINISTRATOR)
        {
            handler->SetSentErrorMessage(true);
            handler->PSendSysMessage("Nivel no encontrado, el valor debe estar entre %u y %u", uint32(SEC_PLAYER), uint32(SEC_ADMINISTRATOR));
            return false;
        }

        sWorld->SetPlayerSecurityLimit(AccountTypes(type));

        handler->SendSysMessage("Nivel minimo de acceso al servidor cambiado");
        return true;
    }

    // .bc debug worldstate (index) (value)
    static bool HandleDebugWorldStateCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // Obtenemos los valores dados por los comandos
        char *cIndex = strtok((char*)args," ");
        char *cValue = strtok(NULL," ");

        if (!cIndex || !cValue)
            return false;
        if (!isNumeric(cValue) || !isNumeric(cIndex))
            return false;

        uint32 Index = (uint32)strtoul(cIndex, NULL, 0);
        uint64 Value = ACE_OS::strtoull(cValue,NULL,10);

        sWorld->setWorldState(Index,Value);

        handler->PSendSysMessage("Se ha cambiado satisfactioriamente el World State (%u)", Index);
        return true;
    }

    // .bc debug worldpacket $size $opcode $val1 $val2...
    static bool HandleDebugWorldPacketCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cValueCount = strtok ((char*)args," ");
        char* cOpCode    = strtok((char*)args," ");

        if (!cValueCount || !isNumeric(cValueCount) || !cOpCode || !isNumeric(cOpCode))
            return false;

        uint32 ValueCount = (uint32)strtoul(cValueCount, NULL, 0);
        uint32 opcode = (uint32)strtoul(cOpCode, NULL, 0);

        WorldPacket data(opcode);
        while(ValueCount-- > 0)
        {
            char* value = strtok((char*)args," ");
            if(!value || !isNumeric(value))
                break;
            uint32 uIntValue = (uint32)strtoul(value, NULL, 0);
            data << uIntValue;
        }
        handler->GetSession()->SendPacket(&data);
        handler->SendSysMessage("OPCODE ENVIADO");
        return true;
    }

    // .bc accinfo
    static bool HandleAccountInfoCommand(ChatHandler* handler, const char* args)
    {
        uint32 accountId;
        uint64 playerGuid;

        if (!handler->GetSession())
            return false;

        if (!*args)
        {
            if (Player* target = handler->GetSession()->GetPlayer()->GetSelectedPlayer())
            {
                accountId = target->GetSession()->GetAccountId();
                playerGuid = target->GetGUID();
            }
            else
            {
                handler->PSendSysMessage("Seleccione un jugador o coloque su nombre!");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            std::string name = strtok((char*)args, " ");;
            normalizePlayerName(name);

            accountId = sObjectMgr->GetPlayerAccountIdByPlayerName(name);
            playerGuid = sObjectMgr->GetPlayerGUIDByName(name);
            if (accountId == 0 || playerGuid == 0)
            {
                handler->PSendSysMessage("Jugador no encontrado");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (handler->HasLowerSecurity(NULL, playerGuid))
            return false;

        if (sWorldCallbackScript->_bcAccInfoCallback.GetParam())
        {
            handler->SendSysMessage("Alguien mas puso bc accinfo, y esta en prcoeso. Solo podemos procesar uno a la vez.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        sWorldCallbackScript->_bcAccInfoCallback.SetParam(new BcAccountInfo(handler->GetSession()));

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNTS);
        stmt->setUInt32(0, accountId);

        sWorldCallbackScript->_bcAccInfoCallback.SetFutureResult(LoginDatabase.AsyncQuery(stmt));
        return true;
    }

    // .bc debug isinrange guid range yardDiff
    static bool HandleDebugIsInRangeCommand(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (!*args)
            return false;

        char* cGuid = strtok((char*)args," ");
        if (!cGuid || !isNumeric(cGuid))
        {
            handler->SendSysMessage("No se pudo reconocer 'guid'");
            handler->SetSentErrorMessage(true);
            return false;
        }
        uint32 guid = atoi(cGuid);

        char* cRange = strtok(NULL, " ");
        if (!cRange || !isNumeric(cRange))
        {
            handler->SendSysMessage("No se pudo reconocer 'range'");
            handler->SetSentErrorMessage(true);
            return false;
        }
        uint32 range = atoi(cRange);

        char* cDiff = strtok(NULL, "");
        if (!cDiff || !isNumeric(cDiff))
        {
            handler->SendSysMessage("No se pudo reconocer 'yardDiff'");
            handler->SetSentErrorMessage(true);
            return false;
        }
        uint32 yardDiff = atoi(cDiff);

        GameObject* go = NULL;

        if (GameObjectData const* gameObjectData = sObjectMgr->GetGOData(guid))
            go = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(guid, gameObjectData->id);

        if (!go)
        {
            handler->PSendSysMessage("No existe el GameObject (GUID: %u)", guid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        float z;
        float x1,y1,x2;
        player->GetPosition(x1,y1,z);
        x2 = x1+range; x1 -= range;
        y1 -= range;
        for (; x1 < x2; x1 += yardDiff)
            for (float y = y1; y-y1 < 2 * range; y += yardDiff)
                if (!go->IsInRange(x1, y, z, 1.0f))
                    player->SummonCreature(23472, x1, y, z, 0,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10 * IN_MILLISECONDS);
        return true;
    }

    // .bc resetcd $mapId $difficulty(10n || 10h || 25n || 25h) $backtime(2d3h5m)
    static bool HandleResetCdCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        if (sWorld->GetPlayerSecurityLimit() == SEC_PLAYER)
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("No se pueden reiniciar los cd en caso de que el servidor este abierto,"
                                    " por favor cierrelo con .bc minseclogin");
            return false;
        }

        uint32 MapId = 0;
        uint8 Difficulty = 0;
        uint32 BackTime = 0;

        // MapId
        char* cMap = strtok((char*)args," ");
        if (!cMap)
            return false;
        if (!isNumeric(cMap))
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("MapId invalido");
            return false;
        }
        MapId = atoi(cMap);

        // Diffuculty
        char* cDifficulty = strtok(NULL, " ");
        if (!cDifficulty)
            return false;

        std::string SDifficulty = cDifficulty;
        if (SDifficulty.compare("10n") == 0)
            Difficulty = RAID_DIFFICULTY_10MAN_NORMAL;
        else if (SDifficulty.compare("10h") == 0)
            Difficulty = RAID_DIFFICULTY_10MAN_HEROIC;
        else if (SDifficulty.compare("25n") == 0)
            Difficulty = RAID_DIFFICULTY_25MAN_NORMAL;
        else if (SDifficulty.compare("25h") == 0)
            Difficulty = RAID_DIFFICULTY_25MAN_HEROIC;
        else
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("Difficulty invalido");
            return false;
        }

        // Tiempo
        char* cBackTime = strtok(NULL, " ");
        if (!cBackTime)
            return false;
        std::string sBackTime = cBackTime;
        BackTime = TimeStringToSecs(sBackTime);
        if (BackTime == 0)
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("Tiempo invalido");
            return false;
        }

        // Comenzamos a procesar
        CharacterDatabase.PExecute("DELETE FROM character_instance WHERE instance IN(SELECT id FROM instance WHERE map  = %u and difficulty = %u and createTime <= UNIX_TIMESTAMP() - %u)", MapId, Difficulty, BackTime);
        CharacterDatabase.PExecute("DELETE FROM group_member WHERE guid IN(SELECT guid FROM group_instance WHERE instance IN(SELECT id FROM instance WHERE map  = %u and difficulty = %u and createTime <= UNIX_TIMESTAMP() - %u))", MapId, Difficulty, BackTime);
        CharacterDatabase.PExecute("DELETE FROM group_instance WHERE instance IN(SELECT id FROM instance WHERE map  = %u and difficulty = %u and createTime <= UNIX_TIMESTAMP() - %u)", MapId, Difficulty, BackTime);
        CharacterDatabase.PExecute("DELETE FROM instance WHERE map  = %u and difficulty = %u and createTime <= UNIX_TIMESTAMP() - %u", MapId, Difficulty, BackTime);

        handler->SendSysMessage("CD de instancia reiniciada!");
        return true;
    }

    // .bc instance freeze
    static bool HandleInstanceFreezeCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession()->GetPlayer()->GetMap()->Instanceable())
        {
            handler->SendSysMessage("Este comando solo puede ser usado en instancias");
            handler->SetSentErrorMessage(true);
            return false;
        }
        Map::PlayerList const& players = handler->GetSession()->GetPlayer()->GetMap()->GetPlayers();

        std::string name;
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* player = itr->GetSource())
            {
                if (player->IsGameMaster() || player == handler->GetSession()->GetPlayer())
                    continue;
                name = player->GetName();
                normalizePlayerName(name);

                // effect
                handler->PSendSysMessage(LANG_COMMAND_FREEZE, name.c_str());

                // stop combat + make player unattackable + duel stop + stop some spells
                player->setFaction(35);
                player->CombatStop();
                // TODO: Implement this
                //if (player->IsNonMeleeSpellCasted(true))
                //    player->InterruptNonMeleeSpells(true);
                player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                // if player class = hunter || warlock remove pet if alive
                if ((player->getClass() == CLASS_HUNTER) || (player->getClass() == CLASS_WARLOCK))
                {
                    if (Pet* pet = player->GetPet())
                    {
                        pet->SavePetToDB(PET_SAVE_AS_CURRENT, false);
                        // not let dismiss dead pet
                        if (pet->IsAlive())
                            player->RemovePet(pet, PET_SAVE_NOT_IN_SLOT);
                    }
                }

                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(9454))
                    Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, player, player);

                // save player
                player->SaveToDB(false, false);
            }
        }
        return true;
    }

    // .bc instance unfreeze
    static bool HandleInstanceUnFreezeCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession()->GetPlayer()->GetMap()->Instanceable())
        {
            handler->SendSysMessage("Este comando solo puede ser usado en instancias");
            handler->SetSentErrorMessage(true);
            return false;
        }
        Map::PlayerList const& players = handler->GetSession()->GetPlayer()->GetMap()->GetPlayers();

        std::string name;
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* player = itr->GetSource())
            {
                name = player->GetName();
                normalizePlayerName(name);

                // effect
                handler->PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());

                // Reset player faction + allow combat + allow duels
                player->setFactionForRace(player->getRace());
                player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                // Remove Freeze spell (allowing movement and spells)
                player->RemoveAurasDueToSpell(9454);

                // Save player
                player->SaveToDB(false, false);
            }
        }

        return true;
    }

    // .bc house add $type(g || s) $faction(a || h || n) $npcTarget
    static bool HandleHouseAddCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* type = strtok((char*)args, " ");
        std::string sType = type;
        if (!type || (sType.compare("g") != 0 && sType.compare("s") != 0))
            return false;

        char* faction = strtok(NULL, " ");
        std::string sFaction = faction;
        if (!faction || (sFaction.compare("a") != 0 && sFaction.compare("h") != 0 && sFaction.compare("n") != 0))
            return false;

        handler->SetSentErrorMessage(true);

        Creature* warden = handler->getSelectedCreature();
        if (!warden || warden->GetEntry() != NPC_HOUSE_WARDEN)
        {
            handler->SendSysMessage("Debe seleccionar a un Warden");
            return false;
        }

        if (House* house = sHouseMgr->GetHouseBy(warden))
        {
            handler->SendSysMessage("Este Warden ya tiene House");
            return false;
        }

        sHouseMgr->Create(sType.compare("g") == 0 ? HOUSE_TYPE_GUILD : HOUSE_TYPE_SHIP,
            sFaction.compare("a") == 0 ? TEAM_ALLIANCE : (sFaction.compare("h") == 0 ? TEAM_HORDE :  TEAM_NEUTRAL),
            warden);

        return true;
    }

    // .bc house del $npcTarget
    static bool HandleHouseDelCommand(ChatHandler* handler, const char* args)
    {
        handler->SetSentErrorMessage(true);

        Creature* warden = handler->getSelectedCreature();
        if (!warden || warden->GetEntry() != NPC_HOUSE_WARDEN)
        {
            handler->SendSysMessage("Debe seleccionar a un Warden");
            return false;
        }

        House* house = sHouseMgr->GetHouseBy(warden);
        if (!house)
        {
            handler->SendSysMessage("Este Warden no posee House");
            return false;
        }

        if (house->IsInUse())
        {
            handler->SendSysMessage("No se puede borrar una House en uso");
            return false;
        }

        if (!house->IsAvailable())
        {
            handler->SendSysMessage("No se puede borrar una House bloqueada");
            return false;
        }


        house->Delete();

        return true;
    }

    // .bc house available $state(on || off) $npcTarget
    static bool HandleHouseAvailableCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string sArg = args;
        if (sArg.compare("on") != 0 && sArg.compare("off") != 0)
            return false;

        handler->SetSentErrorMessage(true);

        Creature* warden = handler->getSelectedCreature();
        if (!warden || warden->GetEntry() != NPC_HOUSE_WARDEN)
        {
            handler->SendSysMessage("Debe seleccionar a un Warden");
            return false;
        }

        House* house = sHouseMgr->GetHouseBy(warden);
        if (!house)
        {
            handler->SendSysMessage("Este Warden no posee House");
            return false;
        }

        if (house->IsInUse())
        {
            handler->SendSysMessage("Esta House se encuentra en uso, sera actualizada automaticamente en cuanto finalice el tiempo");
        }

        house->SetAvailable(sArg.compare("on") == 0);

        return true;
    }

    // .bc house phaseable $state(on || off) $npcTarget
    static bool HandleHousePhaseableCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string sArg = args;
        if (sArg.compare("on") != 0 && sArg.compare("off") != 0)
            return false;

        handler->SetSentErrorMessage(true);

        Creature* warden = handler->getSelectedCreature();
        if (!warden || warden->GetEntry() != NPC_HOUSE_WARDEN)
        {
            handler->SendSysMessage("Debe seleccionar a un Warden");
            return false;
        }

        House* house = sHouseMgr->GetHouseBy(warden);
        if (!house)
        {
            handler->SendSysMessage("Este Warden no posee House");
            return false;
        }

        house->SetPhaseable(sArg.compare("on") == 0);

        return true;
    }

    // .bc house info ($npcTarget || $houseId || $playerName)
    static bool HandleHouseInfoCommand(ChatHandler* handler, const char* args)
    {
        House* house = NULL;
        Creature* warden = handler->getSelectedCreature();
        if (!warden && !*args)
            return false;

        handler->SetSentErrorMessage(true);

        if (warden && warden->GetEntry() == NPC_HOUSE_WARDEN)
        {
            house = sHouseMgr->GetHouseBy(warden);

            if (!house)
            {
                handler->SendSysMessage("Este Warden no posee House");
                return false;
            }
        }

        if (!house && *args)
        {
            char* str = strtok((char*)args, " ");

            if (!str)
                return false;

            if (!isNumeric(str))
            {
                std::string name = handler->extractPlayerNameFromLink(str);
                if (name.empty())
                {
                    handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                // Buscamos Jugador
                Player* player = ObjectAccessor::FindPlayer(sObjectMgr->GetPlayerGUIDByName(name));
                if (!player)
                {
                    handler->SendSysMessage("No se ha podido encontrar al jugador (Debe estar conectado)");
                    return false;
                }

                // Buscamos House para el Jugador
                house = sHouseMgr->GetHouseBy(player);
                if (!house)
                {
                    handler->SendSysMessage("Este jugador no tiene House");
                    return false;
                }
            }
            else
            {
                // Buscamos el House a travez del HouseId
                house = sHouseMgr->GetHouseBy(atoi(str));
                if (!house)
                {
                    handler->SendSysMessage("House no encontrada (HouseId invalida)");
                    return false;
                }
            }
        }

        if (house)
        {
            uint8 inUse = 0;
            std::string housePhaseable = house->IsPhaseable() ? "Comun" : "Exclusiva";
            std::string houseType = house->GetTypeId() == HOUSE_TYPE_GUILD ? "Guild House" : " Ship House";
            std::string houseFaction = house->GetTeamId() == TEAM_ALLIANCE ? "Alianza" : " Horda";
            std::string houseState = house->IsAvailable() ? (house->IsInUse() ? "Ocupada" : "Libre") : "Bloqueada";
            handler->PSendSysMessage("│ House (Id : %u) (%s)", house->GetHouseId(), housePhaseable.c_str());
            handler->PSendSysMessage("├─ Tipo: %s", houseType.c_str());
            handler->PSendSysMessage("├─ Faccion: %s", houseFaction.c_str());
            handler->PSendSysMessage("├─ Estado: %s", houseState.c_str());
            for (House::RoomSet::const_iterator itr = house->GetRoomSetBegin(); itr != house->GetRoomSetEnd(); itr++)
            {
                if ((*itr)->IsInUse())
                {
                    handler->PSendSysMessage("│======================|");
                    float houseTime = ((*itr)->GetEndTime() - uint32(time(NULL))) / DAY;
                    handler->PSendSysMessage("├─ Guild: %s (Id: %u)", sGuildMgr->GetGuildNameById((*itr)->GetGuildId()).c_str(), (*itr)->GetGuildId());
                    handler->PSendSysMessage("├─ Tiempo: %f", houseTime);
                    handler->PSendSysMessage("├─ Fase: %u", (*itr)->GetPhaseMask());
                    inUse++;
                }
            }
            handler->PSendSysMessage("│=====================================================│");
            handler->PSendSysMessage("│ Hay %u habitaciones ocupadas y %u habitaciones libres. │", inUse, 30 - inUse);
            handler->PSendSysMessage("│=====================================================│");
        }

        return true;
    }

    // .bc house list (g || s) (a || h)
    static bool HandleHouseListCommand(ChatHandler* handler, const char* args)
    {
        uint32 housesInUse = 0;
        uint32 housesNotInUse = 0;

        if (!*args)
            return false;

        char* ctype = strtok((char*)args, " ");
        if (!ctype)
            return false;

        std::string sType = ctype;
        if (sType.compare("g") != 0 && sType.compare("s") != 0)
            return false;

        HouseTypeId type = sType.compare("g") == 0 ? HOUSE_TYPE_GUILD : HOUSE_TYPE_SHIP;

        char* cfaction = strtok(NULL, " ");
        if (!cfaction)
            return false;

        std::string sFaction = cfaction;
        if (sFaction.compare("a") != 0 && sFaction.compare("h") != 0 && sFaction.compare("n") != 0)
            return false;

        TeamId teamId = sFaction.compare("a") == 0 ? TEAM_ALLIANCE : (sFaction.compare("h") == 0 ? TEAM_HORDE :  TEAM_NEUTRAL);

        for (HouseMgr::HouseSet::const_iterator itr = sHouseMgr->GetHouseSetBegin(); itr != sHouseMgr->GetHouseSetEnd(); itr++)
        {
            ASSERT(*itr);

            House* house = (*itr);

            if (house->GetTypeId() != type || house->GetTeamId() != teamId)
                continue;

            if (house->IsInUse())
                housesInUse++;
            else
                housesNotInUse++;

            std::string housePhaseable = house->IsPhaseable() ? "Comun" : "Exclusiva";
            std::string houseState = house->IsAvailable() ? (house->IsInUse() ? "Ocupada" : "Libre") : "Bloqueada";
            handler->PSendSysMessage("│======================|");
            handler->PSendSysMessage("│ House (Id : %u) (%s)", house->GetHouseId(), housePhaseable.c_str());
            handler->PSendSysMessage("├─ Estado: %s", houseState.c_str());
        }

        handler->PSendSysMessage("│=====================================│");
        handler->PSendSysMessage("│ En total hay %u ocupadas y %u libres. │", housesInUse, housesNotInUse);
        handler->PSendSysMessage("│=====================================│");

        return true;
    }

    // .bc event add $name
    static bool HandleEventAddCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        if (std::string(args).find(" ") != std::string::npos)
        {
            handler->SendSysMessage("El evento no puede contener espacios en el nombre");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        Event* e = sEventMgr->Create(args, player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
        if (!e)
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("El evento ya existe");
            return false;
        }

        handler->SendSysMessage("Se a agregado el nuevo evento");
        return true;
    }

    // .bc event del $name|$id
    static bool HandleEventDelCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Event* e = isNumeric(args) ? sEventMgr->GetEventById(atoi(args)) : sEventMgr->GetEventByName(args);
        if (!e)
        {
            handler->SendSysMessage("El evento no existe");
            handler->SetSentErrorMessage(true);
            return false;
        }

        sEventMgr->DelEvent(e);

        handler->SendSysMessage("Evento eliminado");
        return true;
    }

    // .bc event rename $oldName $newName
    static bool HandleEventRenameCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        char* oldName = strtok((char*)args, " ");
        if (!oldName)
            return false;

        char* newName = strtok(NULL, "");
        if (!newName)
            return false;

        handler->SetSentErrorMessage(true);
        if (std::string(newName).find(" ") != std::string::npos)
        {
            handler->SendSysMessage("El evento no puede contener espacios en el nombre");
            return false;
        }

        Event* e = sEventMgr->GetEventByName(oldName);
        if (!e)
        {
            handler->SendSysMessage("Evento no encontrado");
            return false;
        }
        Event* newNameEvent = sEventMgr->GetEventByName(newName);
        if (newNameEvent)
        {
            handler->SendSysMessage("El nuevo nombre ya esta usado por otro evento");
            return false;
        }

        e->SetName(newName);
        handler->SendSysMessage("El evento a sido renombrado exitosamente!");
        return true;
    }

    // .bc event info $name|$id
    static bool HandleEventInfoCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Event* e = isNumeric(args) ? sEventMgr->GetEventById(atoi(args)) : sEventMgr->GetEventByName(args);
        if (!e)
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("Evento no encontrado");
            return false;
        }

        std::string eventVisible = e->IsVisible() ? "si" : "no";
        handler->PSendSysMessage("│ Event (Id: %u)", e->GetEventId());
        handler->PSendSysMessage("├─ Nombre: %s", e->GetName().c_str());
        handler->PSendSysMessage("├─ Visible: %s", eventVisible.c_str());

        return true;
    }

    // .bc event appear $name|$id
    static bool HandleEventAppearCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        Event* e = isNumeric(args) ? sEventMgr->GetEventById(atoi(args)) : sEventMgr->GetEventByName(args);
        if (!e)
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("Evento no encontrado");
            return false;
        }

        handler->SendSysMessage("Teletransportandose a evento");
        if (handler->GetSession()->GetPlayer()->TeleportTo(e->GetMapId(), e->GetPositionX(), e->GetPositionY(), e->GetPositionZ(), 0))
            return true;
        handler->SendSysMessage("Error teletransportandolo a el evento");
        handler->SetSentErrorMessage(true);
        return false;
    }

    // .bc event list
    static bool HandleEventListCommand(ChatHandler* handler, const char* args)
    {
        EventMgr::EventVector const& events = sEventMgr->GetEventVector();
        for (EventMgr::EventVector::const_iterator itr = events.begin(); itr != events.end(); ++itr)
            handler->PSendSysMessage("%s (Id: %u)", (*itr)->GetName().c_str(), (*itr)->GetEventId());

        return true;
    }

    // .bc event visible ($name|$id) $(on|off)
    static bool HandleEventVisibleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        const char* cEvent = (const char*)strtok((char*)args, " ");
        if (!cEvent)
            return false;

        const char* cValue = (const char*)strtok(NULL, "");
        if (!cValue)
            return false;

        std::string value = cValue;
        bool toggle;
        if (value.compare("on") == 0)
            toggle = true;
        else if (value.compare("off") == 0)
            toggle = false;
        else
            return false;


        Event* e = isNumeric(cEvent) ? sEventMgr->GetEventById(atoi(cEvent)) : sEventMgr->GetEventByName(cEvent);
        if (e)
        {
            handler->SendSysMessage("Cambiando la visibilidad del evento");
            e->SetVisible(toggle);
            return true;
        }
        handler->SetSentErrorMessage(true);
        handler->SendSysMessage("Evento no encontrado");

        return false;
    }

    // .bc event phasemask ($name|$id) $phase
    static bool HandleEventPhaseMaskCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        const char* cEvent = (const char*)strtok((char*)args, " ");
        if (!cEvent)
            return false;

        const char* cValue = (const char*)strtok(NULL, "");
        if (!cValue || !isNumeric(cValue))
            return false;

        Event* e = isNumeric(cEvent) ? sEventMgr->GetEventById(atoi(cEvent)) : sEventMgr->GetEventByName(cEvent);
        if (e)
        {
            handler->SendSysMessage("PhaseMask del evento cambiada.");
            e->SetPhaseMask(atoi(cValue));
            return true;
        }

        handler->SetSentErrorMessage(true);
        handler->SendSysMessage("Evento no encontrado");

        return false;
    }

    // .bc event build $(off | ($name|$id))
    static bool HandleEventBuildCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string arg = args;
        if (arg.compare("off") == 0)
        {
            Event* e = sEventMgr->GetEventByConstructorGUID(handler->GetSession()->GetPlayer()->GetGUID());
            if (!e)
            {
                handler->SetSentErrorMessage(true);
                handler->SendSysMessage("Usted no se encuentra construyendo ningun evento");
                return false;
            }

            handler->SendSysMessage("Terminando de construir evento actual");
            e->DelConstructor(handler->GetSession()->GetPlayer());
            return true;
        }

        handler->SetSentErrorMessage(true);
        Event* e = sEventMgr->GetEventByConstructorGUID(handler->GetSession()->GetPlayer()->GetGUID());
        if (e)
        {
            handler->SetSentErrorMessage(true);
            handler->PSendSysMessage("Ya se encuentra construyendo el evento: '%s'", e->GetName().c_str());
            return false;
        }

        e = isNumeric(args) ? sEventMgr->GetEventById(atoi(args)) : sEventMgr->GetEventByName(args);
        if (!e)
        {
            handler->SetSentErrorMessage(true);
            handler->SendSysMessage("Evento no encontrado");
            return false;
        }

        handler->SendSysMessage("Comenzando a construir en el evento seleccionado");
        e->AddConstructor(handler->GetSession()->GetPlayer());
        return true;
    }

    static bool HandleReloadIpCountryHandleCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog->outString("Re-Loading ipcountry...");
        sIpCountryMgr->Reload();
        handler->SendGlobalGMSysMessage("DB table `ipcountry` reloaded.");
        return true;
    }

    static bool HandleReloadAccessHandleCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog->outString("Re-Loading Account Access...");
        sAccountMgr->LoadAccountAccess();
        handler->SendGlobalGMSysMessage("DB table `account_access` reloaded.");
        return true;
    }
};

void AddSC_command_bc()
{
    new command_bc();
}
