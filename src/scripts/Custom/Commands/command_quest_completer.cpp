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

#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "MapManager.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "UnorderedSet.h"

// Read only! unless in a command, otherwise unsafe
static UNORDERED_SET<uint32> autocompletequests;

// HandleQuestComplete copy paste from cs_quest.cpp
static bool QuestCompleteHelper(Player* player, uint32 entry)
{
    Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

    // If player doesn't have the quest
    if (!quest || player->GetQuestStatus(entry) == QUEST_STATUS_NONE)
    {
        return false;
    }

    // Add quest items for quests that require items
    for (uint8 x = 0; x < QUEST_ITEM_OBJECTIVES_COUNT; ++x)
    {
        uint32 id = quest->RequiredItemId[x];
        uint32 count = quest->RequiredItemCount[x];
        if (!id || !count)
            continue;

        uint32 curItemCount = player->GetItemCount(id, true);

        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
        if (msg == EQUIP_ERR_OK)
        {
            Item* item = player->StoreNewItem(dest, id, true);
            player->SendNewItem(item, count - curItemCount, true, false);
        }
    }

    // All creature/GO slain/cast (not required, but otherwise it will display "Creature slain 0/10")
    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        int32 creature = quest->RequiredNpcOrGo[i];
        uint32 creatureCount = quest->RequiredNpcOrGoCount[i];

        if (creature > 0)
        {
            if (CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(creature))
                for (uint16 z = 0; z < creatureCount; ++z)
                    player->KilledMonster(creatureInfo, 0);
        }
        else if (creature < 0)
            for (uint16 z = 0; z < creatureCount; ++z)
                player->KillCreditGO(creature);
    }

    // If the quest requires reputation to complete
    if (uint32 repFaction = quest->GetRepObjectiveFaction())
    {
        uint32 repValue = quest->GetRepObjectiveValue();
        uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
        if (curRep < repValue)
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
                player->GetReputationMgr().SetReputation(factionEntry, repValue);
    }

    // If the quest requires a SECOND reputation to complete
    if (uint32 repFaction = quest->GetRepObjectiveFaction2())
    {
        uint32 repValue2 = quest->GetRepObjectiveValue2();
        uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
        if (curRep < repValue2)
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
                player->GetReputationMgr().SetReputation(factionEntry, repValue2);
    }

    // If the quest requires money
    int32 ReqOrRewMoney = quest->GetRewOrReqMoney();
    if (ReqOrRewMoney < 0)
        player->ModifyMoney(-ReqOrRewMoney);

    if (sWorld->getBoolConfig(CONFIG_QUEST_ENABLE_QUEST_TRACKER)) // check if Quest Tracker is enabled
    {
        // prepare Quest Tracker datas
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_QUEST_TRACK_GM_COMPLETE);
        stmt->setUInt32(0, quest->GetQuestId());
        stmt->setUInt32(1, player->GetGUIDLow());

        // add to Quest Tracker
        CharacterDatabase.Execute(stmt);
    }

    player->CompleteQuest(entry);

    ChatHandler(player->GetSession()).PSendSysMessage(LANG_COMMAND_QC_COMPLETE, quest->GetTitle().c_str());
    return true;
}

bool AutoCompleteQuest(Player* player, uint32 entry)
{
    if (autocompletequests.find(entry) != autocompletequests.end())
        return QuestCompleteHelper(player, entry);
    return false;
}

class command_quest_completer : public CommandScript
{
public:
    command_quest_completer() : CommandScript("command_quest_completer") 
    { 
        ReloadAutocompleteQuests();
    }

    static void ReloadAutocompleteQuests()
    {
        autocompletequests.clear();

        QueryResult result = LoginDatabase.Query("SELECT `id` from quest_completer");
        if (!result)
            return;

        do
        {
            Field* fields = result->Fetch();
            autocompletequests.insert(fields[0].GetUInt32());
        } while (result->NextRow());
    }

    ChatCommand* GetCommands() const OVERRIDE
    {
        static ChatCommand questcompleterCommandTable[] =
        {
            { "add",        RBAC_PERM_COMMAND_QUESTCOMPLETER_ADD,       true,   &HandleQuestCompleterAddCommand,        "", NULL },
            { "del",        RBAC_PERM_COMMAND_QUESTCOMPLETER_DEL,       true,   &HandleQuestCompleterDelCommand,        "", NULL },
            { "reload",     RBAC_PERM_COMMAND_QUESTCOMPLETER_RELOAD,    true,   &HandleQuestCompleterReloadCommand,     "", NULL },
            { "",           RBAC_PERM_COMMAND_QUESTCOMPLETER_STATUS,    true,   &HandleQuestCompleterStatusCommand,     "", NULL },            
            { NULL,         0,                                          false,  NULL,                                   "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "qc",         RBAC_PERM_COMMAND_QUESTCOMPLETER,           true,   NULL,                                   "", questcompleterCommandTable },
            { NULL,         0,                                          false,  NULL,                                   "", NULL }
        };
        return commandTable;
    }

    static bool HandleQuestCompleterReloadCommand(ChatHandler* handler, char const* args)
    {
        sLog->outInfo(LOG_FILTER_GENERAL, "Reloading battlecl_questcompleter table..");
        ReloadAutocompleteQuests();
        handler->SendGlobalGMSysMessage("Autocomplete quests reloaded");
        return true;
    }

    static bool HandleQuestCompleterStatusCommand(ChatHandler* handler, char const* args)
    {
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 entry = atol(cId);
        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_ERROR);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!AutoCompleteQuest(handler->GetSession()->GetPlayer(), entry))
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_FOUND, quest->GetTitle().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleQuestCompleterAddCommand(ChatHandler* handler, char const* args)
    {
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_ADD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 entry = atol(cId);
        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_ADD_ERROR);
            handler->SetSentErrorMessage(true);
            return false;
        }

        autocompletequests.insert(entry);
        PreparedStatement* stmt = NULL;
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_QUESTCOMPLETER);
        stmt->setUInt32(0, entry);
        LoginDatabase.Execute(stmt);
        handler->PSendSysMessage(LANG_COMMAND_QC_ADD_SUCCESS, quest->GetTitle().c_str());
        return true;
    }

    static bool HandleQuestCompleterDelCommand(ChatHandler* handler, char const* args)
    {
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_DEL);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 entry = atol(cId);
        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_DEL_ERROR);
            handler->SetSentErrorMessage(true);
            return false;
        }

        autocompletequests.erase(entry);
        PreparedStatement* stmt = NULL;
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_QUESTCOMPLETER);
        stmt->setUInt32(0, entry);
        LoginDatabase.Execute(stmt);
        handler->PSendSysMessage(LANG_COMMAND_QC_DEL_SUCCESS, quest->GetTitle().c_str());
        return true;
    }        
};

void AddSC_command_quest_completer()
{
    new command_quest_completer();
}
