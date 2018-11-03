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
#include <stdarg.h>
#include "GameObject.h"
#include "PoolMgr.h"
#include "ObjectAccessor.h"
#include "Transport.h"

using namespace std;

void CreatePhase(Player * player, bool IsMember, uint32 phase)
{
    ostringstream ss;

    if (IsMember == false)
    {
        ss << "INSERT INTO battlecl_phase VALUES("
            << "'" << player->GetGUID() << "',"
            << "'" << player->GetName() << "',"
            << "'" << phase << "',"
            << "'" << phase << "',"
            << "'" << phase << "',"
            << "'" << "0" << "');";
    }
    else
    {
        ss << "INSERT INTO battlecl_phase_members VALUES("
            << "'" << player->GetGUID() << "',"
            << "'" << player->GetName() << "',"
            << "'" << phase << "',"
            << "'" << phase << "');";
    }

    SQLTransaction transat = CharacterDatabase.BeginTransaction();
    transat->Append(ss.str().c_str());
    CharacterDatabase.CommitTransaction(transat);
}

class command_phasing_system : public CommandScript
{
public:
    command_phasing_system() : CommandScript("command_phasing_system") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> phaseCmdTable =
        {
            { "create",     RBAC_PERM_COMMAND_PHASE_CREATE,     false, &HandlePhaseCreateCommand,       "" },
            { "join",       RBAC_PERM_COMMAND_PHASE_JOIN,       false, &HandlePhaseJoinCommand,         "" },
            { "go",         RBAC_PERM_COMMAND_PHASE_GO,         false, &HandlePhaseGoCommand,           "" },
            { "gotarget",   RBAC_PERM_COMMAND_PHASE_GOTARGET,   false, &HandlePhaseGoTargetCommand,     "" },
            { "godelete",   RBAC_PERM_COMMAND_PHASE_GODELETE,   false, &HandlePhaseGoDeleteCommand,     "" },
            { "help",       RBAC_PERM_COMMAND_PHASE_HELP,       false, &HandlePhaseHelpCommand,         "" },
            { "delete",     RBAC_PERM_COMMAND_PHASE_DELETE,     false, &HandlePhaseDeleteCommand,       "" },
            { "kick",       RBAC_PERM_COMMAND_PHASE_KICK,       false, &HandlePhaseKickCommand,         "" },
            { "get",        RBAC_PERM_COMMAND_PHASE_GET,        false, &HandlePhaseGetCommand,          "" },
            { "complete",   RBAC_PERM_COMMAND_PHASE_COMPELETE,  false, &HandlePhaseCompleteCommand,     "" },
            { "addmember",  RBAC_PERM_COMMAND_PHASE_ADDMEMBER,  false, &HandlePhaseAddMemberCommand,    "" },
            { "npcadd",     RBAC_PERM_COMMAND_PHASE_ADDNPC,     false, &HandlePhaseAddNpcCommand,       "" },
            { "npcdelete",  RBAC_PERM_COMMAND_PHASE_DELETENPC,  false, &HandlePhaseDeleteNpcCommand,    "" }
        };

        static std::vector<ChatCommand> phasecommandTable =
        {
            { "phase",      RBAC_PERM_COMMAND_PHASE,            false, nullptr, "", phaseCmdTable }
        };
        return phasecommandTable;
    }

    static bool HandlePhaseCreateCommand(ChatHandler * chat, const char * args)
    {
        if (!*args)
            return false;

        Player * player = chat->GetSession()->GetPlayer();

        uint32 phase = atoi((char*)args);

        if (phase == 0) // If someone even joins on 0, then it will crash the server
            return false;
        else if (phase == 1) // Default is 1, if someone owned that, then everyone would be screwed xD
            return false;

        QueryResult get_phase = CharacterDatabase.PQuery("SELECT phase_owned FROM battlecl_phase WHERE phase='%u'", phase);
        QueryResult check = CharacterDatabase.PQuery("SELECT phase_owned FROM battlecl_phase WHERE guid='%u'", player->GetGUID());

        if (check)
        {
            do
            {
                Field * mfields = check->Fetch();
                if (mfields[0].GetInt32() != 0)
                {
                    chat->SendSysMessage("|cffFF0000You already have a phase!|r");
                    return false;
                }
                else
                {
                    return true;
                }
            } while (check->NextRow());
        }

        if (get_phase)
        {
            do
            {
                Field * fields = get_phase->Fetch();
                if (phase == fields[0].GetUInt32())
                {
                    chat->SendSysMessage("|cffFF0000This phase has already been taken!|r");
                    return false;
                }
                else
                {
                    return true;
                }
            } while (get_phase->NextRow());
        }

        CreatePhase(player, false, phase);
        CreatePhase(player, true, phase);
        player->SetPhaseMask(phase, true);
        chat->SendSysMessage("|cff4169E1You have created your phase!|r \n |cffbbbbbb.phase join #|r - |cff00FF00is to join a phase.|r \n |cffbbbbbb.phase complete|r - |cff00FF00completes your phase for the public to join and see.|r");
        return true;
    };

    static bool HandlePhaseJoinCommand(ChatHandler * chat, const char * args)
    {
        if (!*args)
            return false;

        Player * player = chat->GetSession()->GetPlayer();
        uint32 phase = (uint32)atoi((char*)args);
        if (phase == 0)
            return false;
        else if (phase == 1)
        {
            player->SetPhaseMask(1, true);
            chat->PSendSysMessage("|cff4169E1You are now entering phase 1.|r (Default Phase)");
        }

        QueryResult hasPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase WHERE guid='%u'", player->GetGUID());
        if (hasPhase)
        {
            do
            {
                Field * fields = hasPhase->Fetch();
                if (fields[0].GetInt32() == 0)
                {
                    chat->PSendSysMessage("|cffFF0000You need to create a phase before joining one!|r");
                    return false;
                }
            } while (hasPhase->NextRow());
        }

        QueryResult myPhase = CharacterDatabase.PQuery("SELECT get_phase FROM battlecl_phase WHERE guid='%u'", player->GetGUID());

        if (myPhase)
        {
            do
            {
                Field * mfield = myPhase->Fetch();
                if (mfield[0].GetUInt32() == phase)
                {
                    chat->PSendSysMessage("|cffFF0000You are already in phase %u!|r", phase);
                    return false;
                }
            } while (myPhase->NextRow());
        }

        QueryResult isCompleted = CharacterDatabase.PQuery("SELECT has_completed,guid,phase FROM battlecl_phase WHERE phase='%u'", phase);
        if (!isCompleted)
            return false;

        if (isCompleted)
        {
            do
            {
                Field * fields = isCompleted->Fetch();
                if (fields[0].GetUInt16() == 1) // if the phase is completed
                {
                    player->SetPhaseMask(phase, true);
                    CharacterDatabase.PExecute("UPDATE battlecl_phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
                    chat->PSendSysMessage("|cff4169E1You are now entering phase %u.|r", phase);
                }
                else if (player->GetGUID() == fields[1].GetUInt64() && fields[2].GetUInt32() == phase)
                {
                    player->SetPhaseMask(phase, true);
                    CharacterDatabase.PExecute("UPDATE battlecl_phase SET get_phase='%u' WHERE guid='%u'", phase, player->GetGUID());
                    chat->PSendSysMessage("|cffffffffYou are now entering your own phase %u.|r", phase);
                }
                else // if the phase isn't completed
                {
                    chat->PSendSysMessage("|cffFF0000This phase isn't completed yet!|r \n Phase: %u", phase);
                    return false;
                }
            } while (isCompleted->NextRow());
        }
        return true;
    };

    static bool HandlePhaseHelpCommand(ChatHandler * chat, const char * args)
    {
        Player * player = chat->GetSession()->GetPlayer();
        char msg[1024];
        snprintf(msg, 1024, "\n |cff00FF00 ~ Phase commands and information is below ~|r \n |cff008000.phase npcadd:|r |cff4169E1 Adds a npc.|r \n |cff008000.phase npcdelete:|r |cff4169E1 Deletes a npc.|r \n |cff008000.phase godelete:|r |cff4169E1 Deletes a gobject.|r \n |cff008000.phase gotarget:|r |cff4169E1 Targets a gobject.|r \n |cff008000.phase go:|r |cff4169E1 Spawns a gobject if you're allowed to.|r \n |cff008000.phase changeowner:|r |cff4169E1 Changes the phase's owner.|r \n |cff008000.phase addmember:|r |cff4169E1 Enables that player to build in your phase.|r \n |cff008000.phase create:|r |cff4169E1 Allows you to create a phase.|r \n |cff008000.phase join:|r |cff4169E1 Join a specified/completed phase.|r \n |cff008000.phase delete:|r |cff4169E1 Delete your created phase.|r \n |cff008000.phase kick:|r |cff4169E1 Kick a player from your phase.|r \n |cff008000.phase leave:|r |cff4169E1 Leave any phase to the default 0 phase.|r \n |cff008000.phase help:|r |cff4169E1 Shows you the commands and what they do.|r \n |cff008000.phase get:|r |cff4169E1 Gets the current phase you're in.|r \n |cff008000.phase kick:|r |cff4169E1 Kick a player from your phase.|r \n |cff008000.phase complete:|r |cff4169E1 After you finish your phase, completing it will set your phase to public.|r \n");
        chat->SendSysMessage(msg);
        return true;
    };

    static bool HandlePhaseDeleteCommand(ChatHandler * chat, const char * args)
    {
        Player * player = chat->GetSession()->GetPlayer();

        player->SetPhaseMask(0, true);
        QueryResult res = CharacterDatabase.PQuery("SELECT * FROM battlecl_phase WHERE guid='%u' LIMIT 1", player->GetGUID());
        if (!res)
            return false;

        CharacterDatabase.PExecute("DELETE FROM battlecl_phase WHERE (guid='%u')", player->GetGUID());
        CharacterDatabase.PExecute("DELETE FROM battlecl_phase_members WHERE (guid='%u')", player->GetGUID());
        chat->SendSysMessage("|cffFFFF00Your phase has now been deleted.|r");
        return true;
    };

    static bool HandlePhaseKickCommand(ChatHandler * chat, const char * args)
    {
        if (!*args)
            return false;

        Player * player = chat->GetSession()->GetPlayer();

        QueryResult phase = CharacterDatabase.PQuery("SELECT phase FROM battlecl_phase WHERE guid='%u'", player->GetGUID());
        if (!phase)
            return false;

        if (phase)
        {
            do
            {
                Field * fields = phase->Fetch();
                QueryResult getplayer = CharacterDatabase.PQuery("SELECT get_phase FROM battlecl_phase WHERE player_name='%s'", args);
                if (!getplayer)
                    return false;

                char msg[255];

                Field * mfields = getplayer->Fetch();
                if (mfields[0].GetInt32() == fields[0].GetInt32())
                {
                    Player * target = ObjectAccessor::FindPlayerByName(args);
                    target->SetPhaseMask(0, true);
                    snprintf(msg, 255, "|cffFF0000You were kicked from phase %u by %s!|r", fields[0].GetInt32(), player->GetName().c_str());
                    player->Whisper(msg, LANG_UNIVERSAL, player->GetGUID());
                    CharacterDatabase.PExecute("UPDATE battlecl_phase SET get_phase='0' AND get_phase='0' WHERE (guid='%u')", player->GetGUID());
                }
                else
                {
                    snprintf(msg, 255, "|cffFF0000%s is not currently in your phase!|r", args);
                    chat->SendSysMessage(msg);
                    return false;
                }
            } while (phase->NextRow());
        }
        return true;
    };

    static bool HandlePhaseGetCommand(ChatHandler * chat, const char * args)
    {
        Player * player = chat->GetSession()->GetPlayer();
        QueryResult getPhaseAndOwnedPhase = CharacterDatabase.PQuery("SELECT COUNT(*),get_phase,phase FROM battlecl_phase WHERE guid='%u'", player->GetGUID());
        if (!getPhaseAndOwnedPhase)
            return false;

        if (getPhaseAndOwnedPhase)
        {
            do
            {
                Field * fields = getPhaseAndOwnedPhase->Fetch();
                if (fields[0].GetInt32() == 0)
                {
                    chat->SendSysMessage("|cffFF0000Could not get phase or owned phase.|r");
                    return false;
                }
                chat->PSendSysMessage("|cffADD8E6Current Phase: %u \n Owned Phase: %u|r", fields[1].GetUInt32(), fields[2].GetUInt32());
            } while (getPhaseAndOwnedPhase->NextRow());
        }
        return true;
    };

    static bool HandlePhaseCompleteCommand(ChatHandler * chat, const char * args)
    {
        if (!*args)
            return false;

        Player * player = chat->GetSession()->GetPlayer();
        uint8 yesorno = (uint8)atoi((char*)args);
        if (!yesorno)
            return false;

        if (yesorno > 1)
            return false;

        QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase WHERE guid='%u'", player->GetGUID());
        if (isOwnerOfAPhase)
        {
            do
            {
                Field * fields = isOwnerOfAPhase->Fetch();
                if (fields[0].GetInt32() == 0)
                {
                    chat->SendSysMessage("|cffFF0000Could not complete phase.|r");
                    return false;
                }
            } while (isOwnerOfAPhase->NextRow());
        }
        if (yesorno == 1)
        {
            chat->SendSysMessage("|cffFFA500You have now completed your phase! It is open for public.|r");
            CharacterDatabase.PExecute("UPDATE battlecl_phase SET has_completed='1' WHERE guid='%u'", player->GetGUID());
        }
        else if (yesorno == 0)
        {
            chat->SendSysMessage("|cffADD8E6Please take your time.");
        }
        return true;
    };

    static bool HandlePhaseAddMemberCommand(ChatHandler * chat, const char * args)
    {
        if (!*args)
            return false;

        uint32 ph = atoi((char*)args);
        Player * player = chat->GetSession()->GetPlayer();
        Player * target = chat->getSelectedPlayer();

        if (!target)
        {
            chat->SendSysMessage("Shoma Nemitavanid Khodetan Ra Target Konid");
            chat->SetSentErrorMessage(true);
            return false;
        }

        if (target == player)
            return false;

        QueryResult isOwnerOfAPhase = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase WHERE player_name='%s'", target->GetName().c_str());
        if (isOwnerOfAPhase)
        {
            do
            {
                Field * fields = isOwnerOfAPhase->Fetch();
                if (fields[0].GetInt32() == 0)
                {
                    chat->PSendSysMessage("|cffFF0000 %s does not own a phase; therefore, he/she cannot be added to your phase.|r", target->GetName().c_str());
                    return false;
                }
            } while (isOwnerOfAPhase->NextRow());
        }
        QueryResult isMember = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase_members WHERE player_name='%s' AND phase='%u'", target->GetName().c_str(), ph);
        if (isMember)
        {
            do
            {
                Field * field = isMember->Fetch();
                if (field[0].GetInt32() == 1)
                {
                    chat->PSendSysMessage("|cffFF0000 %s is already a member of your phase!|r", target->GetName().c_str());
                    return false;
                }
            } while (isMember->NextRow());
        }
        QueryResult isOwner = CharacterDatabase.PQuery("SELECT phase_owned FROM battlecl_phase WHERE guid='%u'", player->GetGUID());
        if (isOwner)
        {
            do
            {
                Field * mfield = isOwner->Fetch();
                if (ph != mfield[0].GetUInt32())
                {
                    chat->PSendSysMessage("|cffFF0000You cannot add yourself or anyone else to someone elses phase.|r");
                    return false;
                }
            } while (isOwner->NextRow());
        }
        chat->PSendSysMessage("|cffFFA500You successfully added %s to your phase %u.|r", target->GetName().c_str(), ph);
        CreatePhase(target, true, ph);
        return true;
    };

#ifndef _PHASE_GO_COMMANDS_
    static bool HandlePhaseGoCommand(ChatHandler * handler, const char * args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
        char* id = handler->extractKeyFromLink((char*)args, "Hgameobject_entry");
        if (!id)
            return false;

        uint32 objectId = atol(id);
        if (!objectId)
            return false;

        char* phasing = strtok(nullptr, " ");

        const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(objectId);

        if (!objectInfo)
        {
            handler->PSendSysMessage("GameOBject %u Not Exited.", objectId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
        {
            // report to DB errors log as in loading case
            sLog->outError("Gameobject (Entry %u GoType: %u) has invalid displayId (%u), not spawned.", objectId, objectInfo->type, objectInfo->displayId);
            handler->PSendSysMessage("GameObject %u has invalid data.", objectId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        float x = float(player->GetPositionX());
        float y = float(player->GetPositionY());
        float z = float(player->GetPositionZ());
        float o = float(player->GetOrientation());
        Map* map = player->GetMap();

        GameObject* object = new GameObject;
        uint32 guidLow = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT);

        if (!object->Create(guidLow, objectInfo->entry, map, player->GetPhaseMaskForSpawn(), x, y, z, o, G3D::Quat(), 0, GO_STATE_READY))
        {
            delete object;
            return false;
        }

        if (phasing)
        {
            uint32 value = atoi((char*)phasing);

            if (value == 0)
                return false;

            QueryResult res;
            res = CharacterDatabase.PQuery("SELECT get_phase FROM battlecl_phase WHERE guid='%u'", player->GetGUID());
            if (res)
            {
                do
                {
                    Field * fields = res->Fetch();
                    uint32 val = fields[0].GetUInt32();
                    if (val != value)
                    {
                        handler->SendSysMessage("You are not in this phase!");
                        return false;
                    }

                    QueryResult result;
                    result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase_members WHERE guid='%u' AND phase='1' LIMIT 1",
                        player->GetGUID(), (uint32)val);

                    if (result)
                    {
                        do
                        {
                            Field * fields = result->Fetch();
                            if (fields[0].GetInt32() == 0)
                            {
                                handler->SendSysMessage("You must be added to this phase before you can build.");
                                return false;
                            }
                        } while (result->NextRow());
                    }
                } while (res->NextRow());
            }
        }

        // fill the gameobject data and save to the db
        object->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), player->GetPhaseMaskForSpawn());
        delete object;

        object = new GameObject();
        // this will generate a new guid if the object is in an instance
        if (!object->LoadGameObjectFromDB(guidLow, map))
        {
            delete object;
            return false;
        }

        // TODO: is it really necessary to add both the real and DB table guid here ?
        sObjectMgr->AddGameobjectToGrid(guidLow, sObjectMgr->GetGOData(guidLow));

        handler->PSendSysMessage("GameObject Added (%u) %s - GUID: %u at x %f y %f z %f", objectId, objectInfo->name.c_str(), guidLow, x, y, z);
        return true;
    }

    static bool HandlePhaseGoTargetCommand(ChatHandler * handler, const char * args)
    {
        Player* pl = handler->GetSession()->GetPlayer();

        QueryResult result;
        GameEventMgr::ActiveEvents const& activeEventsList = sGameEventMgr->GetActiveEventList();
        if (*args)
        {
            char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject_entry");
            if (!cId)
                return false;

            uint32 id = atol(cId);
            char* phasing = strtok(nullptr, " ");
            if (!phasing)
                return false;

            if (id)
                result = WorldDatabase.PQuery("SELECT guid, id, position_x, position_y, position_z, orientation, map, phaseMask, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM gameobject WHERE map = '%i' AND id = '%u' ORDER BY order_ ASC LIMIT 1",
                pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), pl->GetMapId(), id);
            else
            {
                std::string name = cId;
                WorldDatabase.EscapeString(name);
                result = WorldDatabase.PQuery(
                    "SELECT guid, id, position_x, position_y, position_z, orientation, map, phaseMask, (POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2)) AS order_ "
                    "FROM gameobject, gameobject_template WHERE gameobject_template.entry = gameobject.id AND map = %i AND name " _LIKE_" " _CONCAT3_("'%%'", "'%s'", "'%%'")" ORDER BY order_ ASC LIMIT 1",
                    pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), pl->GetMapId(), name.c_str());
            }

            if (phasing)
            {
                uint32 value = atoi((char*)phasing);

                if (value == 0)
                    return false;

                QueryResult res;
                res = CharacterDatabase.PQuery("SELECT get_phase FROM battlecl_phase WHERE guid='%u'", pl->GetGUID());
                if (res)
                {
                    do
                    {
                        Field * fields = res->Fetch();
                        uint32 val = fields[0].GetUInt32();
                        if (val != value)
                        {
                            handler->SendSysMessage("You are not in this phase!");
                            return false;
                        }

                        QueryResult result;
                        result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase_members WHERE guid='%u' AND phase='1' LIMIT 1",
                            pl->GetGUID(), (uint32)val);

                        if (result)
                        {
                            do
                            {
                                Field * fields = result->Fetch();
                                if (fields[0].GetInt32() == 0)
                                {
                                    handler->SendSysMessage("You must be added to this phase before you can target a go.");
                                    return false;
                                }
                            } while (result->NextRow());
                        }
                    } while (res->NextRow());
                }
            }
        }
        else
        {
            std::ostringstream eventFilter;
            eventFilter << " AND (eventEntry IS NULL ";
            bool initString = true;

            for (GameEventMgr::ActiveEvents::const_iterator itr = activeEventsList.begin(); itr != activeEventsList.end(); ++itr)
            {
                if (initString)
                {
                    eventFilter << "OR eventEntry IN (" << *itr;
                    initString = false;
                }
                else
                    eventFilter << ", " << *itr;
            }

            if (!initString)
                eventFilter << "))";
            else
                eventFilter << ")";

            result = WorldDatabase.PQuery("SELECT gameobject.guid, id, position_x, position_y, position_z, orientation, map, phaseMask, "
                "(POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2)) AS order_ FROM gameobject "
                "LEFT OUTER JOIN game_event_gameobject on gameobject.guid = game_event_gameobject.guid WHERE map = '%i' %s ORDER BY order_ ASC LIMIT 10",
                handler->GetSession()->GetPlayer()->GetPositionX(), handler->GetSession()->GetPlayer()->GetPositionY(), handler->GetSession()->GetPlayer()->GetPositionZ(),
                handler->GetSession()->GetPlayer()->GetMapId(), eventFilter.str().c_str());
        }

        if (!result)
        {
            handler->SendSysMessage("Target not found");
            return true;
        }

        bool found = false;
        float x, y, z, o;
        uint32 lowguid, id;
        uint16 mapid, phase;
        uint32 pool_id;

        do
        {
            Field *fields = result->Fetch();
            lowguid = fields[0].GetUInt32();
            id = fields[1].GetUInt32();
            x = fields[2].GetFloat();
            y = fields[3].GetFloat();
            z = fields[4].GetFloat();
            o = fields[5].GetFloat();
            mapid = fields[6].GetUInt16();
            phase = fields[7].GetUInt16();
            pool_id = sPoolMgr->IsPartOfAPool<GameObject>(lowguid);
            if (!pool_id || sPoolMgr->IsSpawnedObject<GameObject>(lowguid))
                found = true;
        } while (result->NextRow() && (!found));

        if (!found)
        {
            handler->PSendSysMessage("GameObject %u Not Exist", id);
            return false;
        }

        GameObjectTemplate const* goI = sObjectMgr->GetGameObjectTemplate(id);

        if (!goI)
        {
            handler->PSendSysMessage("GameObject %u Not Exist", id);
            return false;
        }

        GameObject* target = handler->GetSession()->GetPlayer()->GetMap()->GetGameObject(MAKE_NEW_GUID(lowguid, id, HIGHGUID_GAMEOBJECT));

        handler->PSendSysMessage("GameObject Retail: (%u) %s - GUID %u at (map %u) x %f y %f z %f phase %u", id, goI->name.c_str(), lowguid, mapid, x, y, z, phase);

        if (target)
        {
            int32 curRespawnDelay = int32(target->GetRespawnTimeEx() - time(nullptr));
            if (curRespawnDelay < 0)
                curRespawnDelay = 0;

            std::string curRespawnDelayStr = secsToTimeString(curRespawnDelay, true);
            std::string defRespawnDelayStr = secsToTimeString(target->GetRespawnDelay(), true);

            handler->PSendSysMessage("Respawn times: %s (default) - %s (current)", defRespawnDelayStr.c_str(), curRespawnDelayStr.c_str());
        }
        return true;
    };

    static bool HandlePhaseGoDeleteCommand(ChatHandler * handler, const char * args)
    {
        if (!*args)
            return false;

        Player * player = handler->GetSession()->GetPlayer();

        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!cId)
            return false;

        uint32 lowguid = atoi(cId);
        if (!lowguid)
            return false;

        char* phasing = strtok(nullptr, " ");
        if (!phasing)
            return false;

        GameObject* obj = nullptr;

        if (GameObjectData const* go_data = sObjectMgr->GetGOData(lowguid))
            obj = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(lowguid, go_data->id);

        if (!obj)
        {
            handler->PSendSysMessage("Object %u Not Found", lowguid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (phasing)
        {
            uint32 value = atoi((char*)phasing);

            if (value == 0)
                return false;

            QueryResult res;
            res = CharacterDatabase.PQuery("SELECT get_phase FROM battlecl_phase WHERE guid='%u'", player->GetGUID());
            if (res)
            {
                do
                {
                    Field * fields = res->Fetch();
                    uint32 val = fields[0].GetUInt32();
                    if (val != value)
                    {
                        handler->SendSysMessage("You are not in this phase!");
                        return false;
                    }

                    QueryResult result;
                    result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase_members WHERE guid='%u' AND phase='1' LIMIT 1",
                        player->GetGUID(), (uint32)val);

                    if (result)
                    {
                        do
                        {
                            Field * fields = result->Fetch();
                            if (fields[0].GetInt32() == 0)
                            {
                                handler->SendSysMessage("You must be added to this phase before you can delete an object.");
                                return false;
                            }
                        } while (result->NextRow());
                    }
                } while (res->NextRow());
            }
        }

        uint64 owner_guid = obj->GetOwnerGUID();
        if (owner_guid)
        {
            Unit* owner = ObjectAccessor::GetUnit(*handler->GetSession()->GetPlayer(), owner_guid);
            if (!owner || !IS_PLAYER_GUID(owner_guid))
            {
                handler->PSendSysMessage("Object %u (owner %u) deleted", obj->GetGUIDLow(), GUID_LOPART(owner_guid));
                handler->SetSentErrorMessage(true);
                return false;
            }
            owner->RemoveGameObject(obj, false);
        }

        obj->SetRespawnTime(0);                                 // not save respawn time
        obj->Delete();
        obj->DeleteFromDB();
        handler->PSendSysMessage("Object %u deleted", obj->GetGUIDLow());
        return true;
    };
#endif

#ifndef _PHASE_NPC_COMMANDS_
    static bool HandlePhaseAddNpcCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        char* charID = handler->extractKeyFromLink((char*)args, "Hcreature_entry");
        if (!charID)
            return false;

        char * phase = strtok(nullptr, " ");

        char* team = strtok(nullptr, " ");
        int32 teamval = 0;
        if (team) { teamval = atoi(team); }
        if (teamval < 0) { teamval = 0; }

        uint32 id = atoi(charID);

        Player* chr = handler->GetSession()->GetPlayer();
        float x = chr->GetPositionX();
        float y = chr->GetPositionY();
        float z = chr->GetPositionZ();
        float o = chr->GetOrientation();
        Map* map = chr->GetMap();

        /*if (chr->GetTransport())
        {
            uint32 tguid = chr->GetTransport()->AddNPCPassenger(0, id, chr->GetTransOffsetX(), chr->GetTransOffsetY(), chr->GetTransOffsetZ(), chr->GetTransOffsetO());
            if (tguid > 0)
                WorldDatabase.PQuery("INSERT INTO creature_transport (guid, npc_entry, transport_entry,  TransOffsetX, TransOffsetY, TransOffsetZ, TransOffsetO) values (%u, %u, %f, %f, %f, %f, %u)", tguid, id, chr->GetTransport()->GetEntry(), chr->GetTransOffsetX(), chr->GetTransOffsetY(), chr->GetTransOffsetZ(), chr->GetTransOffsetO());

            return true;
        }*/

        Creature* creature = new Creature;
        if (!creature->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), map, chr->GetPhaseMaskForSpawn(), id, 0, x, y, z, o))
        {
            delete creature;
            return false;
        }

        if (phase)
        {
            uint32 value = atoi((char*)phase);

            if (value == 0)
                return false;

            QueryResult res;
            res = CharacterDatabase.PQuery("SELECT get_phase FROM battlecl_phase WHERE guid='%u'", chr->GetGUID());
            if (res)
            {
                do
                {
                    Field * fields = res->Fetch();
                    uint32 val = fields[0].GetUInt32();
                    if (val != value)
                    {
                        handler->SendSysMessage("You are not in this phase!");
                        return false;
                    }

                    QueryResult result;
                    result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase_members WHERE guid='%u' AND phase='1' LIMIT 1",
                        chr->GetGUID(), (uint32)val);

                    if (result)
                    {
                        do
                        {
                            Field * fields = result->Fetch();
                            if (fields[0].GetInt32() == 0)
                            {
                                handler->SendSysMessage("You must be added to this phase before you can build.");
                                return false;
                            }
                        } while (result->NextRow());
                    }
                } while (res->NextRow());
            }
        }

        creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());

        uint32 db_guid = creature->GetDBTableGUIDLow();

        // To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells();
        if (!creature->LoadCreatureFromDB(db_guid, map))
        {
            delete creature;
            return false;
        }

        sObjectMgr->AddCreatureToGrid(db_guid, sObjectMgr->GetCreatureData(db_guid));
        return true;
    }

    static bool HandlePhaseDeleteNpcCommand(ChatHandler * handler, const char * args)
    {
        Creature* unit = nullptr;
        Player * pl = handler->GetSession()->GetPlayer();
        if (*args)
        {
            char * phase = strtok(nullptr, " ");
            if (!phase)
                return false;

            if (phase)
            {
                uint32 value = atoi((char*)phase);
                if (value == 0)
                    return false;

                QueryResult res;
                res = CharacterDatabase.PQuery("SELECT get_phase FROM battlecl_phase WHERE guid='%u'", pl->GetGUID());
                if (res)
                {
                    do
                    {
                        Field * fields = res->Fetch();
                        uint32 val = fields[0].GetUInt32();
                        if (val != value)
                        {
                            handler->SendSysMessage("You are not in this phase!");
                            return false;
                        }

                        QueryResult result;
                        result = CharacterDatabase.PQuery("SELECT COUNT(*) FROM battlecl_phase_members WHERE guid='%u' AND phase='1' LIMIT 1",
                            pl->GetGUID(), (uint32)val);

                        if (result)
                        {
                            do
                            {
                                Field * fields = result->Fetch();
                                if (fields[0].GetInt32() == 0)
                                {
                                    handler->SendSysMessage("You must be added to this phase before you can target a go.");
                                    return false;
                                }
                            } while (result->NextRow());
                        }
                    } while (res->NextRow());
                }
            }
        }
        else
            unit = handler->getSelectedCreature();

        if (!unit || unit->IsPet() || unit->IsTotem())
        {
            handler->SendSysMessage("You Must Select Creature");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Delete the creature
        unit->CombatStop();
        unit->DeleteFromDB();
        unit->AddObjectToRemoveList();

        handler->SendSysMessage("Creature Delete Shod .");

        return true;
    }
#endif
};

void AddSC_command_system_phase()
{
    new command_phasing_system;
}
