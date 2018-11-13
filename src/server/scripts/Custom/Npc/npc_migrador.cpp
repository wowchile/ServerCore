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

#include "Config.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptPCH.h"

enum Professions
{
    ALCHEMY         = 51304,
    TAILORING       = 51309,
    SKINNING        = 50305,
    MINING          = 50310,
    LEATHERWORKING  = 51302,
    JEWELCRAFTING   = 51311,
    INSCRIPTION     = 45363,
    HERB_GATHERING  = 50300,
    ENGINEERING     = 51306,
    ENCHANTING      = 51313,
    BLACKSMITHING   = 51300
};

enum GossipsActions
{
    ACTION_ALCHEMY,
    ACTION_TAILORING,
    ACTION_SKINNING,
    ACTION_MINING,
    ACTION_LEATHERWORKING,
    ACTION_JEWELCRAFTING,
    ACTION_INSCRIPTION,
    ACTION_HERB_GATHERING,
    ACTION_ENGINEERING,
    ACTION_ENCHANTING,
    ACTION_BLACKSMITHING
};

enum Quests
{
    QUEST_TIER_1           = 6000001,
    QUEST_TIER_2           = 6000002,
    QUEST_TIER_3           = 6000003,
    QUEST_TIER_4           = 6000004,
    QUEST_TIER_5           = 6000005,
    QUEST_TIER_6           = 6000006,
    QUEST_PROFESSIONS      = 6000007,
    QUEST_FINISH_MIGRATION = 6000008
};

enum FactionRestriction
{
    PROF_NONE            = 0,
    PROF_ALLIANCE_ONLY   = 1,
    PROF_HORDE_ONLY      = 2,
    PROF_EVERYONE        = 3
};

enum Items
{
    ITEM_GIGANTIQUE_BAG = 38082
};

class npc_migrador : public CreatureScript
{
public:
    npc_migrador() : CreatureScript("npc_migrador") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (!player || !creature)
            return true;

        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        QuestStatus questStatus = player->GetQuestStatus(QUEST_PROFESSIONS);

        if ((questStatus == QUEST_STATUS_INCOMPLETE || questStatus == QUEST_STATUS_COMPLETE) && !HasTwoProfessions(player))
        {
            if (!player->HasSpell(ALCHEMY))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Alquimia",      GOSSIP_SENDER_MAIN, ACTION_ALCHEMY);
            if (!player->HasSpell(TAILORING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Sastreria",     GOSSIP_SENDER_MAIN, ACTION_TAILORING);
            if (!player->HasSpell(SKINNING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Desuello",      GOSSIP_SENDER_MAIN, ACTION_SKINNING);
            if (!player->HasSpell(MINING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Mineria",       GOSSIP_SENDER_MAIN, ACTION_MINING);
            if (!player->HasSpell(LEATHERWORKING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Peleteria",     GOSSIP_SENDER_MAIN, ACTION_LEATHERWORKING);
            if (!player->HasSpell(JEWELCRAFTING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Joyeria",       GOSSIP_SENDER_MAIN, ACTION_JEWELCRAFTING);
            if (!player->HasSpell(INSCRIPTION))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Inscripcion",   GOSSIP_SENDER_MAIN, ACTION_INSCRIPTION);
            if (!player->HasSpell(HERB_GATHERING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Herboristeria", GOSSIP_SENDER_MAIN, ACTION_HERB_GATHERING);
            if (!player->HasSpell(ENGINEERING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Ingenieria",    GOSSIP_SENDER_MAIN, ACTION_ENGINEERING);
            if (!player->HasSpell(ENCHANTING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Encantamiento", GOSSIP_SENDER_MAIN, ACTION_ENCHANTING);
            if (!player->HasSpell(BLACKSMITHING))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Aprender: Herreria",      GOSSIP_SENDER_MAIN, ACTION_BLACKSMITHING);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        if (!player)
            return true;

        switch (action)
        {
            case ACTION_ALCHEMY:
                player->learnSpell(ALCHEMY, false);
                MaxSkill(player, 171);
                break;
            case ACTION_TAILORING:
                player->learnSpell(TAILORING, false);
                MaxSkill(player, 197);
                break;
            case ACTION_SKINNING:
                player->learnSpell(SKINNING, false);
                MaxSkill(player, 393);
                break;
            case ACTION_MINING:
                player->learnSpell(MINING, false);
                MaxSkill(player, 186);
                break;
            case ACTION_LEATHERWORKING:
                player->learnSpell(LEATHERWORKING, false);
                MaxSkill(player, 165);
                break;
            case ACTION_JEWELCRAFTING:
                player->learnSpell(JEWELCRAFTING, false);
                MaxSkill(player, 755);
                break;
            case ACTION_INSCRIPTION:
                player->learnSpell(INSCRIPTION, false);
                MaxSkill(player, 773);
                break;
            case ACTION_HERB_GATHERING:
                player->learnSpell(HERB_GATHERING, false);
                MaxSkill(player, 182);
                break;
            case ACTION_ENGINEERING:
                player->learnSpell(ENGINEERING, false);
                MaxSkill(player, 202);
                break;
            case ACTION_ENCHANTING:
                player->learnSpell(ENCHANTING, false);
                MaxSkill(player, 333);
                break;
            case ACTION_BLACKSMITHING:
                player->learnSpell(BLACKSMITHING, false);
                MaxSkill(player, 164);
                break;
        }

        if (HasTwoProfessions(player))
        {
            Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_PROFESSIONS);

            if (quest)
            {
                player->CompleteQuest(QUEST_PROFESSIONS);
                player->RewardQuest(quest, 0, player);
            }
        }

        player->CLOSE_GOSSIP_MENU();

        return true;
    }

    bool OnQuestReward(Player* player, Creature* /*creature*/, Quest const* quest, uint32 /*opt*/)
    {
        if (quest->GetQuestId() >= QUEST_TIER_1 && quest->GetQuestId() <= QUEST_TIER_6)
        {
            player->GiveLevel(80);
            player->InitTalentForLevel();
            player->SetUInt32Value(PLAYER_XP, 0);
            
            switch (player->getClass())
            {
                case CLASS_DEATH_KNIGHT:
                    if (player->GetTeam() == ALLIANCE)
                    {
                        RewardClassQuest(player, 13188); // Where Kings Walk
                    } else {
                        RewardClassQuest(player, 13189); // Warchief's Blessing
                    }
                    
                    RewardClassQuest(player, 12680); // Death knight horse mount
                    RewardClassQuest(player, 12687);
                    RewardClassQuest(player, 12801); // Death gate
                    RewardClassQuest(player, 13165);
                    RewardClassQuest(player, 13166);
                    RewardClassQuest(player, 12593); // Rune forge
                    RewardClassQuest(player, 12619);
                    RewardClassQuest(player, 12842);
                    RewardClassQuest(player, 12848);
                    RewardClassQuest(player, 12636);
                    RewardClassQuest(player, 12641);
                    RewardClassQuest(player, 12657);
                    break;
                case CLASS_DRUID:
                    if (player->GetTeam() == ALLIANCE)
                    {
                        // Night elf - Bear form
                        RewardClassQuest(player, 5923); // Heeding the Call                        
                        RewardClassQuest(player, 5921); // Moonglade
                        RewardClassQuest(player, 5929); // Great Bear Spirit
                        RewardClassQuest(player, 5931); // Back to Darnassus
                        RewardClassQuest(player, 6001); // Body and Heart
                    } else {
                        // Tauren - Bear form
                        RewardClassQuest(player, 5926); // Heeding the Call
                        RewardClassQuest(player, 5922); // Moonglade
                        RewardClassQuest(player, 5930); // Great Bear Spirit
                        RewardClassQuest(player, 5932); // Back to Thunder Bluff
                        RewardClassQuest(player, 6002); // Body and Heart
                    }
                    break;
                case CLASS_HUNTER:
                    if (player->GetTeam() == ALLIANCE)
                    {
                        if (player->getORace() == RACE_DWARF)
                        {
                            RewardClassQuest(player, 6075);
                            RewardClassQuest(player, 6064);
                            RewardClassQuest(player, 6084);
                            RewardClassQuest(player, 6085);
                            RewardClassQuest(player, 6086);

                        } else if (player->getORace() == RACE_NIGHTELF) {
                            RewardClassQuest(player, 6071);
                            RewardClassQuest(player, 6063);
                            RewardClassQuest(player, 6101);
                            RewardClassQuest(player, 6102);
                            RewardClassQuest(player, 6103);

                        } else if (player->getORace() == RACE_DRAENEI) {
                            RewardClassQuest(player, 9757);
                            RewardClassQuest(player, 9591);
                            RewardClassQuest(player, 9592);
                            RewardClassQuest(player, 9593);
                            RewardClassQuest(player, 9675);
                        }
                    } else {
                        if (player->getORace() == RACE_ORC)
                        {
                            RewardClassQuest(player, 6069);
                            RewardClassQuest(player, 6062);
                            RewardClassQuest(player, 6083);
                            RewardClassQuest(player, 6082);
                            RewardClassQuest(player, 6081);
                        } else if (player->getORace() == RACE_TAUREN) {
                            RewardClassQuest(player, 6067);
                            RewardClassQuest(player, 6061);
                            RewardClassQuest(player, 6087);
                            RewardClassQuest(player, 6088);
                            RewardClassQuest(player, 6089);
                        } else if (player->getORace() == RACE_TROLL) {
                            RewardClassQuest(player, 6069);
                            RewardClassQuest(player, 6062);
                            RewardClassQuest(player, 6083);
                            RewardClassQuest(player, 6082);
                            RewardClassQuest(player, 6081);
                        } else if (player->getORace() == RACE_BLOODELF) {
                            RewardClassQuest(player, 9617);
                            RewardClassQuest(player, 9484);
                            RewardClassQuest(player, 9486);
                            RewardClassQuest(player, 9485);
                            RewardClassQuest(player, 9673);
                        }
                    }
                    break;
                case CLASS_PALADIN:
                    if (player->GetTeam() == HORDE)
                    {
                        // Blood elf - Redemption
                        RewardClassQuest(player, 9677); // Summons from Knight-Lord Bloodvalor
                        RewardClassQuest(player, 9678); // The First Trial
                        RewardClassQuest(player, 9681); // A Study in Power
                        RewardClassQuest(player, 9684); // Claiming the Light
                        RewardClassQuest(player, 9685); // Redeeming the Dead
                    } else {
                        if (player->getORace() == RACE_HUMAN)
                        {
                            RewardClassQuest(player, 2998); // The Tome of Divinity
                            RewardClassQuest(player, 1641);
                            RewardClassQuest(player, 1642);
                            RewardClassQuest(player, 1643);
                            RewardClassQuest(player, 1644);
                            RewardClassQuest(player, 1780);
                            RewardClassQuest(player, 1781);
                            RewardClassQuest(player, 1786);
                            RewardClassQuest(player, 1787);
                            RewardClassQuest(player, 1788);                            
                        } else if (player->getORace() == RACE_DWARF)
                        {
                            RewardClassQuest(player, 2997); // The Tome of Divinity
                            RewardClassQuest(player, 1645);
                            RewardClassQuest(player, 1646);
                            RewardClassQuest(player, 1647);
                            RewardClassQuest(player, 1648);
                            RewardClassQuest(player, 1778);
                            RewardClassQuest(player, 1779);
                            RewardClassQuest(player, 1783);
                            RewardClassQuest(player, 1784);
                            RewardClassQuest(player, 1785);
                        } else if (player->getORace() == RACE_DRAENEI)
                        {
                            RewardClassQuest(player, 10366); // Redemption
                            RewardClassQuest(player, 9598);
                            RewardClassQuest(player, 9600);                            
                        }
                    }
                    break;
                case CLASS_SHAMAN:
                    if (player->GetTeam() == ALLIANCE)
                    {
                        // Draenei
                        RewardClassQuest(player, 9449); // Call of Earth
                        RewardClassQuest(player, 9450);
                        RewardClassQuest(player, 9451);
                        RewardClassQuest(player, 9462); // Call of Fire
                        RewardClassQuest(player, 9464);
                        RewardClassQuest(player, 9465);
                        RewardClassQuest(player, 9467);
                        RewardClassQuest(player, 9468);
                        RewardClassQuest(player, 9461);
                        RewardClassQuest(player, 9555);
                        RewardClassQuest(player, 9500); // Call of Water
                        RewardClassQuest(player, 9501);
                        RewardClassQuest(player, 9503);
                        RewardClassQuest(player, 9504);
                        RewardClassQuest(player, 9508);
                        RewardClassQuest(player, 9509);
                        RewardClassQuest(player, 9551); // Call of Air
                        RewardClassQuest(player, 9552);
                        RewardClassQuest(player, 9553);
                        RewardClassQuest(player, 9554);
                    } else {
                        if (player->getORace() == RACE_ORC || player->getORace() == RACE_TROLL)
                        {
                            RewardClassQuest(player, 1516); // Call of Earth
                            RewardClassQuest(player, 1517);
                            RewardClassQuest(player, 1518);
                            RewardClassQuest(player, 2983); // Call of Fire
                            RewardClassQuest(player, 1524);
                            RewardClassQuest(player, 1525);
                            RewardClassQuest(player, 1526);
                            RewardClassQuest(player, 1527);
                        } else if (player->getORace() == RACE_TAUREN) {
                            RewardClassQuest(player, 1519); // Call of Earth
                            RewardClassQuest(player, 1520);
                            RewardClassQuest(player, 1521);
                            RewardClassQuest(player, 2984); // Call of Fire
                            RewardClassQuest(player, 1524);
                            RewardClassQuest(player, 1525);
                            RewardClassQuest(player, 1526);
                            RewardClassQuest(player, 1527);
                        }

                        // Orc, Tauren and Troll
                        RewardClassQuest(player, 1528); // Call of Water
                        RewardClassQuest(player, 1530);
                        RewardClassQuest(player, 1535);
                        RewardClassQuest(player, 1536);
                        RewardClassQuest(player, 1534);
                        RewardClassQuest(player, 220);
                        RewardClassQuest(player, 63);
                        RewardClassQuest(player, 100);
                        RewardClassQuest(player, 96);
                        RewardClassQuest(player, 1531); // Call of Air
                    }
                    break;
                case CLASS_WARLOCK:
                    if (player->GetTeam() == ALLIANCE)
                    {
                        if (player->getORace() == RACE_HUMAN)
                        {
                            RewardClassQuest(player, 1598); // Imp
                            RewardClassQuest(player, 1685); // Voidwalker
                            RewardClassQuest(player, 1688);
                            RewardClassQuest(player, 1689);                      
                        } else if (player->getORace() == RACE_GNOME) {
                            RewardClassQuest(player, 1599); // Imp
                            RewardClassQuest(player, 1715); // Voidwalker
                            RewardClassQuest(player, 1688);
                            RewardClassQuest(player, 1689);
                        }

                        RewardClassQuest(player, 1717); // Succubus
                        RewardClassQuest(player, 1716);
                        RewardClassQuest(player, 1738);
                        RewardClassQuest(player, 1739);
                        RewardClassQuest(player, 1798); // Mana eater
                        RewardClassQuest(player, 1758);
                        RewardClassQuest(player, 1802);
                        RewardClassQuest(player, 1804);
                        RewardClassQuest(player, 1795);
                        RewardClassQuest(player, 7601); // Infernal
                        RewardClassQuest(player, 7602);
                        RewardClassQuest(player, 7603);
                    } else {
                        if (player->getORace() == RACE_ORC)
                        {
                            RewardClassQuest(player, 1485); // Imp
                            RewardClassQuest(player, 1506); // Voidwalker
                            RewardClassQuest(player, 1501);
                            RewardClassQuest(player, 1504);
                            RewardClassQuest(player, 1507); // Succubus
                            RewardClassQuest(player, 1508);
                            RewardClassQuest(player, 1509);
                            RewardClassQuest(player, 1510);
                            RewardClassQuest(player, 1511);
                            RewardClassQuest(player, 1515);
                            RewardClassQuest(player, 1512);
                            RewardClassQuest(player, 1513);
                        } else if (player->getORace() == RACE_UNDEAD_PLAYER) {
                            RewardClassQuest(player, 1470); // Imp
                            RewardClassQuest(player, 1478); // Voidwalker
                            RewardClassQuest(player, 1473);
                            RewardClassQuest(player, 1471);
                            RewardClassQuest(player, 3001); // Mana eater
                            RewardClassQuest(player, 1801);
                            RewardClassQuest(player, 1803);
                            RewardClassQuest(player, 1805);
                            RewardClassQuest(player, 1795);
                        } else if (player->getORace() == RACE_BLOODELF) {
                            RewardClassQuest(player, 8344); // Imp
                            RewardClassQuest(player, 10788); // Voidwalker
                            RewardClassQuest(player, 9529);
                            RewardClassQuest(player, 9619);
                        } else if (player->getORace() == RACE_UNDEAD_PLAYER || player->getORace() == RACE_BLOODELF) {
                            RewardClassQuest(player, 10605); // Succubus
                            RewardClassQuest(player, 1472);
                            RewardClassQuest(player, 1476);
                            RewardClassQuest(player, 1474);
                        } else if (player->getORace() == RACE_ORC || player->getORace() == RACE_BLOODELF) {
                            RewardClassQuest(player, 2996); // Mana eater
                            RewardClassQuest(player, 1801);
                            RewardClassQuest(player, 1803);
                            RewardClassQuest(player, 1805);
                            RewardClassQuest(player, 1795);
                        }

                        RewardClassQuest(player, 7601); // Infernal
                        RewardClassQuest(player, 7602);
                        RewardClassQuest(player, 7603);
                    }
                    break;
                case CLASS_WARRIOR:
                    if (player->GetTeam() == ALLIANCE)
                    {
                        if (player->getORace() == RACE_HUMAN)
                        {
                            RewardClassQuest(player, 1638); // Defensive Stance
                            RewardClassQuest(player, 1639);
                            RewardClassQuest(player, 1640);
                            RewardClassQuest(player, 1665);        
                        } else if (player->getORace() == RACE_DWARF || player->getORace() == RACE_GNOME) {
                            RewardClassQuest(player, 1679); // Defensive Stance
                            RewardClassQuest(player, 1678);
                        } else if (player->getORace() == RACE_DRAENEI) {
                            RewardClassQuest(player, 9582); // Defensive Stance
                            RewardClassQuest(player, 10350);
                        } else if (player->getORace() == RACE_NIGHTELF) {
                            RewardClassQuest(player, 1684); // Defensive Stance
                            RewardClassQuest(player, 1683);
                        }

                        RewardClassQuest(player, 1718); // Berserker Stance
                        RewardClassQuest(player, 1719);
                    } else {
                        if (player->getORace() == RACE_UNDEAD_PLAYER)
                        {
                            RewardClassQuest(player, 1818); // Defensive Stance
                            RewardClassQuest(player, 1819);
                        } else {
                            RewardClassQuest(player, 1505); // Defensive Stance
                            RewardClassQuest(player, 1498);
                        }

                        RewardClassQuest(player, 1718); // Berserker Stance
                        RewardClassQuest(player, 1719);
                    }
                    break;
            }

            uint32 city1, city2, city3, city4, city5;

            if (player->GetTeam() == HORDE)
            {
                city1 = 76; // Orgrimmar
                city2 = 81; // Thunder Bluff
                city3 = 68; // Undercity
                city4 = 530; // Darkspear Trolls
                city5 = 911; // Silvermoon city
            } else {
                city1 = 72; // Stormwind
                city2 = 47; // Ironforge
                city3 = 69; // Darnassus
                city4 = 54; // Gnomeregan Exiles
                city5 = 911; // Exodar
            }

            RewardReputation(player, city1);
            RewardReputation(player, city2);
            RewardReputation(player, city3);
            RewardReputation(player, city4);
            RewardReputation(player, city5);

            uint32 moneyReward = uint32(sConfigMgr->GetIntDefault("Migrations.RewardMoney", 0));

            if (moneyReward > 0)
            {
                ChatHandler(player->GetSession()).PSendSysMessage(LANG_YOURS_MONEY_GIVEN, "Migrador", moneyReward);
                uint32 playerMoney = player->GetMoney();
                
                if (playerMoney >= (uint32(MAX_MONEY_AMOUNT) - moneyReward))
                    moneyReward -= playerMoney;
    
                player->ModifyMoney(moneyReward);
            }

            uint8 bagAmount = uint8(sConfigMgr->GetIntDefault("Migrations.RewardBagAmount", 0));

            if (bagAmount > 0)
            {
                const ItemTemplate* itemTemplate = sObjectMgr->GetItemTemplate(ITEM_GIGANTIQUE_BAG);

                if (itemTemplate && itemTemplate->AllowableRace & player->getRaceMask())
                {
                    ItemPosCountVec dest;
                    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_GIGANTIQUE_BAG, bagAmount);
                    if (msg == EQUIP_ERR_OK)
                    {
                        if (!dest.empty())
                        {
                            Item* pItem = player->StoreNewItem(dest, ITEM_GIGANTIQUE_BAG, true);
                            player->SendNewItem(pItem, bagAmount, true, true);
                        }
                    }
                    else
                        player->SendEquipError(msg, NULL, NULL, ITEM_GIGANTIQUE_BAG);
                }
                else
                    player->SendEquipError(EQUIP_ERR_YOU_CAN_NEVER_USE_THAT_ITEM, NULL, NULL, ITEM_GIGANTIQUE_BAG);
            }

            int factionProfConfig = sConfigMgr->GetBoolDefault("Migrations.GiveProfessions", 0);
            bool isEligibleForProfessions = false;
            if (factionProfConfig == PROF_EVERYONE
                || (factionProfConfig == PROF_ALLIANCE_ONLY && player->GetTeam() == ALLIANCE)
                || (factionProfConfig == PROF_HORDE_ONLY && player->GetTeam() == HORDE))
                isEligibleForProfessions = true;

            if (isEligibleForProfessions)
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_PROFESSIONS);

                if (quest)
                {
                    player->AddQuest(quest, NULL);
                }
            }

            Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_FINISH_MIGRATION);
            
            if (quest)
            {
                player->AddQuest(quest, NULL);
            }

            std::string faction = (player->GetTeam() == HORDE) ? "Horde" : "Alliance";
            std::string confMapId = "Migrations." + faction + ".MapId";
            std::string confCoordX = "Migrations." + faction + ".ZoneCoordX";
            std::string confCoordY = "Migrations." + faction + ".ZoneCoordY";
            std::string confCoordZ = "Migrations." + faction + ".ZoneCoordZ";

            uint32 posMapId = uint32(sConfigMgr->GetIntDefault(confMapId.c_str(), 571));
            float posX = sConfigMgr->GetFloatDefault(confCoordX.c_str(), 5807.6787f);
            float posY = sConfigMgr->GetFloatDefault(confCoordY.c_str(), 588.2504f);
            float posZ = sConfigMgr->GetFloatDefault(confCoordZ.c_str(), 660.9396f);
            
            player->CastSpell(player, 12438, true); // Slow fall
            player->TeleportTo(posMapId, posX, posY, posZ + 1.0f, player->GetOrientation());    
        }
        else if (quest->GetQuestId() == QUEST_FINISH_MIGRATION)
        {
            // Dalaran
            uint32 posMapId = uint32(571);
            float posX = 5807.6787f;
            float posY = 588.2504f;
            float posZ = 660.9396f;

            player->CastSpell(player, 12438, true); // Slow fall
            player->TeleportTo(posMapId, posX, posY, posZ + 1.0f, 1.705188f);
        }

        return true;
    }

    inline void MaxSkill(Player* player, uint16 skill)
    {
        player->SetSkill(skill, player->GetSkillStep(skill), 450, 450);
    }

    inline void RewardReputation(Player* player, uint32 factionId)
    {
        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

        if (factionEntry)
        {
            player->GetReputationMgr().SetOneFactionReputation(factionEntry, 42999, false);
            player->GetReputationMgr().SendState(player->GetReputationMgr().GetState(factionEntry));
        }
    }

    inline bool HasTwoProfessions(Player* player)
    {
        uint32 proffesions[11] = { 202, 333, 164, 171, 197, 393, 186, 165, 755, 773, 182 };
        uint32 professionCount = 0;

        for (uint8 i = 0; i < 11;++i)
            if (player->HasSkill(proffesions[i]))
                ++professionCount;

        return professionCount > 1;
    }

    inline void RewardClassQuest(Player* player, uint32 questId)
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        
        if (quest)
        {
            player->AddQuest(quest, NULL);
            player->CompleteQuest(questId);
            player->RewardQuest(quest, 0, player);
        }
    }
};

void AddSC_npc_migrador()
{
    new npc_migrador();
}