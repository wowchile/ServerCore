#include "ScriptPCH.h"
#include "Chat.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "WorldSession.h"
#include "Player.h"
#include "Pet.h"
#include "ArenaTeam.h"
#include "ArenaSpectator.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "CreatureTextMgr.h"

std::vector<Battleground*> ratedArenas;

enum ClassTalents
{
    SPELL_DK_BLOOD                  = 62905,    // Not Dancing Rune Weapon (i used Improved Death Strike)
    SPELL_DK_FROST                  = 49143,    // Not Howling Blash (i used Frost Strike)
    SPELL_DK_UNHOLY                 = 49206,

    SPELL_DRUID_BALANCE             = 48505,
    SPELL_DRUID_FERAL_COMBAT        = 50334,
    SPELL_DRUID_RESTORATION         = 63410,    // Not Wild Growth (i used Improved Barkskin)

    SPELL_HUNTER_BEAST_MASTERY      = 53270,
    SPELL_HUNTER_MARKSMANSHIP       = 53209,
    SPELL_HUNTER_SURVIVAL           = 53301,

    SPELL_MAGE_ARCANE               = 44425,
    SPELL_MAGE_FIRE                 = 31661,    // Not Living Bomb (i used Dragon's Breath)
    SPELL_MAGE_FROST                = 44572,

    SPELL_PALADIN_HOLY              = 53563,
    SPELL_PALADIN_PROTECTION        = 53595,
    SPELL_PALADIN_RETRIBUTION       = 53385,

    SPELL_PRIEST_DISCIPLINE         = 47540,
    SPELL_PRIEST_HOLY               = 47788,
    SPELL_PRIEST_SHADOW             = 47585,

    SPELL_ROGUE_ASSASSINATION       = 1329,     // Not Hunger For Blood (i used Mutilate)
    SPELL_ROGUE_COMBAT              = 51690,
    SPELL_ROGUE_SUBTLETY            = 51713,

    SPELL_SHAMAN_ELEMENTAL          = 51490,
    SPELL_SHAMAN_ENHACEMENT         = 30823,    // Not Feral Spirit (i used Shamanistic Rage)
    SPELL_SHAMAN_RESTORATION        = 974,      // Not Riptide (i used Earth Shield)

    SPELL_WARLOCK_AFFLICTION        = 48181,
    SPELL_WARLOCK_DEMONOLOGY        = 59672,
    SPELL_WARLOCK_DESTRUCTION       = 50796,

    SPELL_WARRIOR_ARMS              = 46924,
    SPELL_WARRIOR_FURY              = 23881,    // Not Titan's Grip (i used Bloodthirst)
    SPELL_WARRIOR_PROTECTION        = 46968,
};

enum NpcSpectatorActions
{
    // will be used for scrolling
    NPC_SPECTATOR_ACTION_MAIN_MENU          = 1,
    NPC_SPECTATOR_ACTION_SPECIFIC           = 10,
    NPC_SPECTATOR_ACTION_2V2_GAMES          = 100,
    NPC_SPECTATOR_ACTION_3V3_GAMES          = 300,
    NPC_SPECTATOR_ACTION_5V5_GAMES          = 500,

    // NPC_SPECTATOR_ACTION_SELECTED_PLAYER + player.Guid()
    NPC_SPECTATOR_ACTION_SELECTED_PLAYER    = 700
};

enum PlayerArenaSlots
{
    SLOT_ARENA_2v2      = 0,
    SLOT_ARENA_3v3      = 1,
    SLOT_ARENA_5v5      = 2
};

const uint8 GAMES_ON_PAGE = 15;

void LoadAllArenas()
{
    ratedArenas.clear();
    for (uint8 i = 0; i <= MAX_BATTLEGROUND_TYPE_ID; ++i)
    {
        if (!sBattlegroundMgr->IsArenaType(BattlegroundTypeId(i)))
            continue;

        BattlegroundContainer arenas = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BattlegroundTypeId(i));

        if (arenas.empty())
            continue;

        for (BattlegroundContainer::const_iterator itr = arenas.begin(); itr != arenas.end(); ++itr)
        {
            Battleground* arena = itr->second;

            if (!arena->GetPlayersSize())
                continue;

            if (!arena->isRated())
                continue;

            ratedArenas.push_back(arena);
        }
    }

    if (ratedArenas.size() < 2)
        return;

    std::vector<Battleground*>::iterator itr = ratedArenas.begin();
    int size = ratedArenas.size();
    int count = 0;
    for (; itr != ratedArenas.end(); ++itr)
    {
        if (!(*itr))
        {
            count++;
            continue;
        }

        // I have no idea if this event could ever happen, but if it did, it would most likely
        // cause crash
        if (count >= size)
            return;

        // Bubble sort, oh yeah, that's the stuff..
        for (int i = count; i < size; i++)
        {
            if (Battleground* tmpBg = ratedArenas[i])
            {
                Battleground* tmp = (*itr);
                (*itr) = ratedArenas[i];
                ratedArenas[i] = tmp;
            }
        }
        count++;
    }

    return;
}

uint16 GetSpecificArenasCount(ArenaType type, uint16 count1)
{
    if (ratedArenas.empty())
        return 0;

    uint16 count[2] = { 0, 0 };

    for (std::vector<Battleground*>::const_iterator citr = ratedArenas.begin(); citr != ratedArenas.end(); ++citr)
    {
        if (Battleground* arena = (*citr))
        {
            if (arena->GetArenaType() == type)
            {
                if (arena->GetStatus() == STATUS_IN_PROGRESS)
                    count[1]++;

                count[0]++;
            }
        }
    }

    count1 = count[1];
    return count[0];
}

class npc_arena_spectator : public CreatureScript
{
public:
    npc_arena_spectator() : CreatureScript("npc_arena_spectator") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        LoadAllArenas();

        uint32 arenasQueueTotal[3] = { 0, 0, 0 };
        uint32 arenasQueuePlaying[3] = { 0, 0, 0 };
        arenasQueueTotal[0] = GetSpecificArenasCount(ARENA_TYPE_2v2, arenasQueuePlaying[0]);
        arenasQueueTotal[1] = GetSpecificArenasCount(ARENA_TYPE_3v3, arenasQueuePlaying[1]);
        arenasQueueTotal[2] = GetSpecificArenasCount(ARENA_TYPE_5v5, arenasQueuePlaying[2]);

        std::stringstream Gossip2s;
        Gossip2s << "Arenas 2vs2 (Jugando: " << arenasQueueTotal[0] << ")";
        std::stringstream Gossip3s;
        Gossip3s << "Arenas 3v3 (Jugando: " << arenasQueueTotal[1] << ")";
        std::stringstream Gossip5s;
        Gossip5s << "Arenas 5vs5 (Jugando:" << arenasQueueTotal[2] << ")";
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, Gossip2s.str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, Gossip3s.str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, Gossip5s.str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES);
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Mirar a un jugador especifico.", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SPECIFIC, "", 0, true);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();

        if (action == NPC_SPECTATOR_ACTION_MAIN_MENU)
        {
            OnGossipHello(player, creature);
            return true;
        }

        if (action >= NPC_SPECTATOR_ACTION_2V2_GAMES && action < NPC_SPECTATOR_ACTION_3V3_GAMES)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "Actualizar", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
            ShowPage(player, action - NPC_SPECTATOR_ACTION_2V2_GAMES, false);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        else if (action >= NPC_SPECTATOR_ACTION_3V3_GAMES && action < NPC_SPECTATOR_ACTION_SELECTED_PLAYER)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "Actualizar", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
            ShowPage(player, action - NPC_SPECTATOR_ACTION_3V3_GAMES, true);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        else if (action >= NPC_SPECTATOR_ACTION_5V5_GAMES && action < NPC_SPECTATOR_ACTION_SELECTED_PLAYER)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "Actualizar", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES);
            ShowPage(player, action - NPC_SPECTATOR_ACTION_5V5_GAMES, true);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        else
        {
            uint64 guid = action - NPC_SPECTATOR_ACTION_SELECTED_PLAYER;
            if (Player* target = ObjectAccessor::FindPlayer(guid))
            {
                ChatHandler handler(player->GetSession());
                char const* pTarget = target->GetName().c_str();
                ArenaSpectator::HandleSpectatorSpectateCommand(&handler, pTarget);
            }
        }

        return true;
    }

    std::string GetClassNameById(Player* player)
    {
        std::string sClass = "";
        switch (player->getClass())
        {
        case CLASS_WARRIOR:
            if (player->HasTalent(SPELL_WARRIOR_ARMS, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_WARRIOR_FURY, player->GetActiveSpec()))
                sClass = "F";
            else if (player->HasTalent(SPELL_WARRIOR_PROTECTION, player->GetActiveSpec()))
                sClass = "P";
            sClass += "Warrior ";
            break;
        case CLASS_PALADIN:
            if (player->HasTalent(SPELL_PALADIN_HOLY, player->GetActiveSpec()))
                sClass = "H";
            else if (player->HasTalent(SPELL_PALADIN_PROTECTION, player->GetActiveSpec()))
                sClass = "P";
            else if (player->HasTalent(SPELL_PALADIN_RETRIBUTION, player->GetActiveSpec()))
                sClass = "R";
            sClass += "Paladin ";
            break;
        case CLASS_HUNTER:
            if (player->HasTalent(SPELL_HUNTER_BEAST_MASTERY, player->GetActiveSpec()))
                sClass = "B";
            else if (player->HasTalent(SPELL_HUNTER_MARKSMANSHIP, player->GetActiveSpec()))
                sClass = "M";
            else if (player->HasTalent(SPELL_HUNTER_SURVIVAL, player->GetActiveSpec()))
                sClass = "S";
            sClass += "Hunter ";
            break;
        case CLASS_ROGUE:
            if (player->HasTalent(SPELL_ROGUE_ASSASSINATION, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_ROGUE_COMBAT, player->GetActiveSpec()))
                sClass = "C";
            else if (player->HasTalent(SPELL_ROGUE_SUBTLETY, player->GetActiveSpec()))
                sClass = "S";
            sClass += "Rogue ";
            break;
        case CLASS_PRIEST:
            if (player->HasTalent(SPELL_PRIEST_DISCIPLINE, player->GetActiveSpec()))
                sClass = "D";
            else if (player->HasTalent(SPELL_PRIEST_HOLY, player->GetActiveSpec()))
                sClass = "H";
            else if (player->HasTalent(SPELL_PRIEST_SHADOW, player->GetActiveSpec()))
                sClass = "S";
            sClass += "Priest ";
            break;
        case CLASS_DEATH_KNIGHT:
            if (player->HasTalent(SPELL_DK_BLOOD, player->GetActiveSpec()))
                sClass = "B";
            else if (player->HasTalent(SPELL_DK_FROST, player->GetActiveSpec()))
                sClass = "F";
            else if (player->HasTalent(SPELL_DK_UNHOLY, player->GetActiveSpec()))
                sClass = "U";
            sClass += "DK ";
            break;
        case CLASS_SHAMAN:
            if (player->HasTalent(SPELL_SHAMAN_ELEMENTAL, player->GetActiveSpec()))
                sClass = "EL";
            else if (player->HasTalent(SPELL_SHAMAN_ENHACEMENT, player->GetActiveSpec()))
                sClass = "EN";
            else if (player->HasTalent(SPELL_SHAMAN_RESTORATION, player->GetActiveSpec()))
                sClass = "R";
            sClass += "Shaman ";
            break;
        case CLASS_MAGE:
            if (player->HasTalent(SPELL_MAGE_ARCANE, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_MAGE_FIRE, player->GetActiveSpec()))
                sClass = "Fi";
            else if (player->HasTalent(SPELL_MAGE_FROST, player->GetActiveSpec()))
                sClass = "Fr";
            sClass += "Mage ";
            break;
        case CLASS_WARLOCK:
            if (player->HasTalent(SPELL_WARLOCK_AFFLICTION, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_WARLOCK_DEMONOLOGY, player->GetActiveSpec()))
                sClass = "Dem";
            else if (player->HasTalent(SPELL_WARLOCK_DESTRUCTION, player->GetActiveSpec()))
                sClass = "Des";
            sClass += "Warlock ";
            break;
        case CLASS_DRUID:
            if (player->HasTalent(SPELL_DRUID_BALANCE, player->GetActiveSpec()))
                sClass = "B";
            else if (player->HasTalent(SPELL_DRUID_FERAL_COMBAT, player->GetActiveSpec()))
                sClass = "F";
            else if (player->HasTalent(SPELL_DRUID_RESTORATION, player->GetActiveSpec()))
                sClass = "R";
            sClass += "Druid ";
            break;
        default:
            sClass = "<Unknown>";
            break;
        }

        return sClass;
    }

    std::string GetGamesStringData(Battleground* team, uint16 mmr, uint16 mmrTwo)
    {
        std::string teamsMember[BG_TEAMS_COUNT];
        uint32 firstTeamId = 0;
        for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin(); itr != team->GetPlayers().end(); ++itr)
        {
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            {
                if (player->IsSpectator())
                    continue;

                if (player->IsGameMaster())
                    continue;

                uint32 team = itr->second->Team;
                if (!firstTeamId)
                    firstTeamId = team;

                teamsMember[firstTeamId == team] += GetClassNameById(player);
            }
        }

        std::string data = teamsMember[0] + "(";
        std::stringstream ss;
        ss << mmr;
        std::stringstream sstwo;
        sstwo << mmrTwo;
        data += ss.str();
        data += ") vs ";
        data += teamsMember[1] + "(" + sstwo.str();
        data += ")";
        return data;
    }

    uint64 GetFirstPlayerGuid(Battleground* team)
    {
        for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin(); itr != team->GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                return itr->first;
        return 0;
    }

    void GetTeamsMMRs(Battleground* arena, Player* target, uint16 &mmrOne, uint16 &mmrTwo)
    {
        uint32 firstTeamId = target->GetArenaTeamId(arenaSlot);
        uint8 arenaSlot = 0;
        switch (arena->GetArenaType())
        {
            case ARENA_TYPE_2v2:
                arenaSlot = SLOT_ARENA_2v2;
                break;
            case ARENA_TYPE_3v3:
                arenaSlot = SLOT_ARENA_3v3;
                break;
            case ARENA_TYPE_5v5:
                arenaSlot = SLOT_ARENA_5v5;
                break;
            default: // do nothing
                break;
        }

        // Get first team MMR using target player
        if (ArenaTeam *targetArenaMmr = sArenaTeamMgr->GetArenaTeamById(firstTeamId))
            mmrOne = targetArenaMmr->GetMember(target->GetGUID())->MatchMakerRating;

        // Find second team MMR
        Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
        for (; citr != arena->GetPlayers().end(); ++citr)
        {
            if (Player* plrs = sObjectAccessor->FindPlayer(citr->first))
            {
                if (plrs->GetArenaTeamId(arenaSlot) != firstTeamId)
                {
                    if (ArenaTeam *arenaMmr = sArenaTeamMgr->GetArenaTeamById(plrs->GetArenaTeamId(arenaSlot)))
                        mmrTwo = arenaMmr->GetMember(plrs->GetGUID())->MatchMakerRating;
                    break;
                }
            }
        }
    }

    void ShowPage(Player* player, uint16 page, bool IsTop)
    {
        uint16 TypeTwo = 0;
        uint16 TypeThree = 0;
        uint16 TypeFive = 0;
        uint16 mmr = 0;
        uint16 mmrTwo = 0;
        bool haveNextPage[3];
        haveNextPage[2] = false;
        haveNextPage[1] = false;
        haveNextPage[0] = false;

        for (uint8 i = 0; i <= MAX_BATTLEGROUND_TYPE_ID; ++i)
        {
            if (!sBattlegroundMgr->IsArenaType(BattlegroundTypeId(i)))
                continue;

            BattlegroundContainer arenas = sBattlegroundMgr->GetAllBattlegroundsWithTypeId(BattlegroundTypeId(i));

            if (arenas.empty())
                continue;

            for (BattlegroundContainer::const_iterator itr = arenas.begin(); itr != arenas.end(); ++itr)
            {
                Battleground* arena = itr->second;
                Player* target = ObjectAccessor::FindPlayer(GetFirstPlayerGuid(arena));
                if (!target)
                    continue;

                if (target->GetTypeId() == TYPEID_PLAYER && target->HasAura(SPELL_ARENA_PREPARATION))
                    continue;

                if (!arena->GetPlayersSize() || !arena->isRated())
                    continue;

                // Fill both MMR ratings
                GetTeamsMMRs(arena, target, mmr, mmrTwo);

                if (IsTop && arena->GetArenaType() == ARENA_TYPE_3v3)
                {
                    TypeThree++;
                    if (TypeThree > (page + 1) * GAMES_ON_PAGE)
                    {
                        haveNextPage[1] = true;
                        break;
                    }

                    if (TypeThree >= page * GAMES_ON_PAGE)
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena));
                }
                else if (!IsTop && arena->GetArenaType() == ARENA_TYPE_2v2)
                {
                    TypeTwo++;
                    if (TypeTwo > (page + 1) * GAMES_ON_PAGE)
                    {
                        haveNextPage[0] = true;
                        break;
                    }

                    if (TypeTwo >= page * GAMES_ON_PAGE)
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena));
                }
                else if (IsTop && arena->GetArenaType() == ARENA_TYPE_5v5)
                {
                    TypeFive++;
                    if (TypeFive > (page + 1) * GAMES_ON_PAGE)
                    {
                        haveNextPage[2] = true;
                        break;
                    }

                    if (TypeFive >= page * GAMES_ON_PAGE)
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena));
                }
            }
        }

        if (page > 0)
        {
            if (haveNextPage[0] == true)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "<<", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page - 1);
            if (haveNextPage[1] == true)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "<<", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page - 1);
            if (haveNextPage[2] == true)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "<<", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES + page - 1);
        }
        else
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "Volver al menu", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_MAIN_MENU);

        if (haveNextPage[0] == true)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, ">>", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page + 1);
        if (haveNextPage[1] == true)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, ">>", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page + 1);
        if (haveNextPage[2] == true)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, ">>", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_5V5_GAMES + page + 1);
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        if (!player)
            return true;

        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();
        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
                case NPC_SPECTATOR_ACTION_SPECIFIC: // choosing a player
                    if (Player* target = sObjectAccessor->FindPlayerByName(code))
                    {
                        ChatHandler handler(player->GetSession());
                        char const* pTarget = target->GetName().c_str();
                        ArenaSpectator::HandleSpectatorSpectateCommand(&handler, pTarget);
                    }
                    else
                        ChatHandler(player->GetSession()).SendSysMessage("El jugador no existe.");
                    return true;
            }
        }
        return false;
    }
};

void AddSC_npc_arena_spectator()
{
    new npc_arena_spectator();
}