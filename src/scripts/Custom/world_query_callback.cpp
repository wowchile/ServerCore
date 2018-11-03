#include "ScriptPCH.h"
#include "World.h"
#include "world_query_callback.h"

class world_query_callback : public WorldScript
{
public:
    world_query_callback() : WorldScript("world_query_callback") {}

    void OnStartup()
    {
        sWorldCallbackScript->_pinfoCallback.SetParam(NULL);
        sWorldCallbackScript->_bcAccInfoCallback.SetParam(NULL);
    }

    void OnUpdate(uint32 /*diff*/)
    {
        PreparedQueryResult result;

        if (sWorldCallbackScript->_pinfoCallback.IsReady())
        {
            sWorldCallbackScript->_pinfoCallback.GetResult(result);
            PlayerInfoCallback(result, sWorldCallbackScript->_pinfoCallback.GetParam());
            // Don't call FreeResult() here, the callback handler will do that depending on the events in the callback chain
        }

        if (sWorldCallbackScript->_bcAccInfoCallback.IsReady())
        {
            sWorldCallbackScript->_bcAccInfoCallback.GetResult(result);
            BcAccountInfoCallback(result, sWorldCallbackScript->_bcAccInfoCallback.GetParam());
            // Don't call FreeResult() here, the callback handler will do that depending on the events in the callback chain
        }
    }

    void PlayerInfoCallback(PreparedQueryResult result, PlayerInfoCommand* playerInfoCommand)
    {
        switch (sWorldCallbackScript->_pinfoCallback.GetStage())
        {
            case 0:
            {
                if (!playerInfoCommand->_session)
                {
                    delete playerInfoCommand;
                    sWorldCallbackScript->_pinfoCallback.Reset();
                }

                if (result)
                {
                    Field* fields = result->Fetch();
                    playerInfoCommand->_userName      = fields[0].GetString();
                    playerInfoCommand->_security      = fields[1].GetUInt8();

                    // Only fetch these fields if commander has sufficient rights)
                    if (playerInfoCommand->_session->HasPermission(RBAC_PERM_COMMANDS_PINFO_CHECK_PERSONAL_DATA) && // RBAC Perm. 48, Role 39
                        playerInfoCommand->_session->GetSecurity() >= AccountTypes(playerInfoCommand->_security))
                    {
                        playerInfoCommand->_eMail     = fields[2].GetString();
                        playerInfoCommand->_lastIp    = fields[3].GetString();
                        playerInfoCommand->_lastLogin = fields[4].GetString();

                        uint32 ip = inet_addr(playerInfoCommand->_lastIp.c_str());
                        EndianConvertReverse(ip);

                        // If ipcountry table is populated, it displays the country
                        IpCountry* ipCountry = sIpCountryMgr->GetIpCountry(ip);
                        if (ipCountry)
                        {
                            playerInfoCommand->_lastIp.append(" (");
                            playerInfoCommand->_lastIp.append(ipCountry->GetCountryName());
                            playerInfoCommand->_lastIp.append(")");
                        }
                    }
                    else
                    {
                        playerInfoCommand->_eMail     = "Unauthorized";
                        playerInfoCommand->_lastIp    = "Unauthorized";
                        playerInfoCommand->_lastLogin = "Unauthorized";
                    }
                    playerInfoCommand->_muteTime      = fields[5].GetUInt64();
                    playerInfoCommand->_muteReason    = fields[6].GetString();
                    playerInfoCommand->_muteBy        = fields[7].GetString();
                }

                // Returns banType, banTime, bannedBy, banreason
                PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PINFO_BANS);
                stmt->setUInt32(0, playerInfoCommand->_accId);

                sWorldCallbackScript->_pinfoCallback.FreeResult();
                sWorldCallbackScript->_pinfoCallback.SetFutureResult(LoginDatabase.AsyncQuery(stmt));
                sWorldCallbackScript->_pinfoCallback.NextStage();
            }
            break;
            case 1:
            {
                if (!playerInfoCommand->_session)
                {
                    delete playerInfoCommand;
                    sWorldCallbackScript->_pinfoCallback.Reset();
                }

                PreparedStatement* stmt;

                // Ban data print variables
                int64 banTime                 = -1;
                std::string banType           = "None";
                std::string banReason         = "Unknown";
                std::string bannedBy          = "Unknown";

                // Character data print variables
                std::string raceStr, classStr   = "None";
                std::string genderStr           = playerInfoCommand->_session->GetTrinityString(LANG_ERROR);
                int8 locale                     = playerInfoCommand->_session->GetSessionDbcLocale();
                uint32 xp                       = 0;
                uint32 xptotal                  = 0;

                // Position data print
                std::string areaName    = "<unknown>";
                std::string zoneName    = "<unknown>";

                // Guild data print variables defined so that they exist, but are not necessarily used
                uint32 guildId           = 0;
                uint8 guildRankId        = 0;
                std::string guildName;
                std::string guildRank;
                std::string note;
                std::string officeNote;

                if (result)
                {
                    Field* fields = result->Fetch();
                    banTime       = int64(fields[1].GetUInt64() ? 0 : fields[0].GetUInt32());
                    bannedBy      = fields[2].GetString();
                    banReason     = fields[3].GetString();
                }
                else
                {
                    banType = "Character";
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PINFO_BANS);
                    stmt->setUInt32(0, playerInfoCommand->_lowguid);
                    result = CharacterDatabase.Query(stmt);
                }

                // Can be used to query data from World database
                stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_REQ_XP);
                stmt->setUInt8(0, playerInfoCommand->_level);
                result = WorldDatabase.Query(stmt);

                if (result)
                {
                    Field* fields = result->Fetch();
                    xptotal       = fields[0].GetUInt32();
                }

                // Can be used to query data from Characters database
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PINFO_XP);
                stmt->setUInt32(0, playerInfoCommand->_lowguid);
                result = CharacterDatabase.Query(stmt);

                if (result)
                {
                    Field* fields = result->Fetch();
                    xp            = fields[0].GetUInt32(); // Used for "current xp" output and "%u XP Left" calculation
                    uint32 gguid  = fields[1].GetUInt32(); // We check if have a guild for the person, so we might not require to query it at all

                    if (gguid != 0)
                    {
                        // Guild Data - an own query, because it may not happen.
                        stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MEMBER_EXTENDED);
                        stmt->setUInt32(0, playerInfoCommand->_lowguid);
                        result = CharacterDatabase.Query(stmt);
                        if (result)
                        {
                            Field* fields  = result->Fetch();
                            guildId        = fields[0].GetUInt32();
                            guildName      = fields[1].GetString();
                            guildRank      = fields[2].GetString();
                            guildRankId    = fields[3].GetUInt8();
                            note           = fields[4].GetString();
                            officeNote     = fields[5].GetString();
                        }
                    }
                }

                // Creates a chat link to the character. Returns nameLink
                std::string nameLink = ChatHandler(playerInfoCommand->_session).playerLink(playerInfoCommand->_targetName);

                // Initiate output
                // Output I. LANG_PINFO_PLAYER
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_PLAYER, playerInfoCommand->_target ? "" : playerInfoCommand->_session->GetTrinityString(LANG_OFFLINE), nameLink.c_str(), playerInfoCommand->_lowguid);

                // Output II. LANG_PINFO_GM_ACTIVE if character is gamemaster
                if (playerInfoCommand->_target && playerInfoCommand->_target->IsGameMaster())
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_GM_ACTIVE);

                // Output III. LANG_PINFO_BANNED if ban exists and is applied
                if (banTime >= 0)
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_BANNED, banTime > 0 ? secsToTimeString(banTime - time(nullptr), true).c_str() : "permanently", bannedBy.c_str(), banReason.c_str());

                // Output IV. LANG_PINFO_MUTED if mute is applied
                if (playerInfoCommand->_muteTime > 0)
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_MUTED, secsToTimeString(playerInfoCommand->_muteTime - time(NULL), true).c_str(), playerInfoCommand->_muteReason.c_str(), playerInfoCommand->_muteBy.c_str());

                // Output V. LANG_PINFO_ACC_ACCOUNT
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_ACC_ACCOUNT, playerInfoCommand->_userName.c_str(), playerInfoCommand->_accId, playerInfoCommand->_security);

                // Output VI. LANG_PINFO_ACC_LASTLOGIN
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_ACC_LASTLOGIN, playerInfoCommand->_lastLogin.c_str());

                // Output VII. LANG_PINFO_ACC_OS
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_ACC_OS, playerInfoCommand->_latency);

                // Output VIII. LANG_PINFO_ACC_REGMAILS
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_ACC_REGMAILS, playerInfoCommand->_eMail.c_str(), playerInfoCommand->_eMail.c_str()); // TODO: Implement registration email

                // Output IX. LANG_PINFO_ACC_IP
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_ACC_IP, playerInfoCommand->_lastIp.c_str(), "No");

                // Output X. LANG_PINFO_CHR_LEVEL
                if (playerInfoCommand->_level != sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_LEVEL_LOW, playerInfoCommand->_level, xp, xptotal, (xptotal - xp));
                else
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_LEVEL_HIGH, playerInfoCommand->_level);

                // Output XI. LANG_PINFO_CHR_RACE
                raceStr  = GetRaceName(playerInfoCommand->_raceid, locale);
                classStr = GetClassName(playerInfoCommand->_classid, locale);
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_RACE, (playerInfoCommand->_gender == 0 ? playerInfoCommand->_session->GetTrinityString(LANG_CHARACTER_GENDER_MALE) : playerInfoCommand->_session->GetTrinityString(LANG_CHARACTER_GENDER_FEMALE)), raceStr.c_str(), classStr.c_str());

                // Output XII. LANG_PINFO_CHR_ALIVE
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_ALIVE, playerInfoCommand->_alive.c_str());

                // Output XIII. LANG_PINFO_CHR_PHASE if player is not in GM mode (GM is in every phase)
                if (playerInfoCommand->_target && !playerInfoCommand->_target->IsGameMaster())                            // IsInWorld() returns false on loadingscreen, so it's more
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_PHASE, playerInfoCommand->_phase);        // precise than just target (safer ?).
                                                                                  // However, as we usually just require a target here, we use target instead.
                // Output XIV. LANG_PINFO_CHR_MONEY
                uint32 gold                   = playerInfoCommand->_money / GOLD;
                uint32 silv                   = (playerInfoCommand->_money % GOLD) / SILVER;
                uint32 copp                   = (playerInfoCommand->_money % GOLD) % SILVER;
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_MONEY, gold, silv, copp);

                // Position data
                MapEntry const* map = sMapStore.LookupEntry(playerInfoCommand->_mapId);
                AreaTableEntry const* area = GetAreaEntryByAreaID(playerInfoCommand->_areaId);
                if (area)
                {
                    areaName = area->area_name[locale];

                    AreaTableEntry const* zone = GetAreaEntryByAreaID(area->zone);
                    if (zone)
                        zoneName = zone->area_name[locale];
                }

                if (playerInfoCommand->_target)
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_MAP, map->name[locale], (!zoneName.empty() ? zoneName.c_str() : "<Unknown>"), (!areaName.empty() ? areaName.c_str() : "<Unknown>"));

                // Output XVII. - XVIX. if they are not empty
                if (!guildName.empty())
                {
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_GUILD, guildName.c_str(), guildId);
                    ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_GUILD_RANK, guildRank.c_str(), uint32(guildRankId));
                    if (!note.empty())
                        ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_GUILD_NOTE, note.c_str());
                    if (!officeNote.empty())
                        ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_GUILD_ONOTE, officeNote.c_str());
                }

                // Output XX. LANG_PINFO_CHR_PLAYEDTIME
                ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_PLAYEDTIME, (secsToTimeString(playerInfoCommand->_totalPlayerTime, true)).c_str());

                // Mail Data - an own query, because it may or may not be useful.
                // SQL: "SELECT SUM(CASE WHEN (checked & 1) THEN 1 ELSE 0 END) AS 'readmail', COUNT(*) AS 'totalmail' FROM mail WHERE `receiver` = ?"
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PINFO_MAILS);
                stmt->setUInt32(0, playerInfoCommand->_lowguid);
                result = CharacterDatabase.Query(stmt);
                if (result)
                {
                    // Define the variables, so the compiler knows they exist
                    uint32 rmailint = 0;

                    // Fetch the fields - readmail is a SUM(x) and given out as char! Thus...
                    // ... while totalmail is a COUNT(x), which is given out as INt64, which we just convert on fetch...
                    Field* fields         = result->Fetch();
                    std::string readmail  = fields[0].GetString();
                    uint32 totalmail      = uint32(fields[1].GetUInt64());

                    // ... we have to convert it from Char to int. We can use totalmail as it is
                    rmailint = atol(readmail.c_str());

                    // Output XXI. LANG_INFO_CHR_MAILS if at least one mail is given
                    if (totalmail >= 1)
                       ChatHandler(playerInfoCommand->_session).PSendSysMessage(LANG_PINFO_CHR_MAILS, rmailint, totalmail);
                }

                delete playerInfoCommand;
                sWorldCallbackScript->_pinfoCallback.Reset();
            }
            break;
        }
    }

    void BcAccountInfoCallback(PreparedQueryResult result, BcAccountInfo* bcAccountInfo)
    {
        switch (sWorldCallbackScript->_bcAccInfoCallback.GetStage())
        {
            case 0:
            {
                if (!bcAccountInfo->_session)
                {
                    delete bcAccountInfo;
                    sWorldCallbackScript->_bcAccInfoCallback.Reset();
                    return;
                }

                // Copia de LookupPlayerSearchCommand(result, -1);
                int32 limit = -1;
                if (!result)
                {
                    ChatHandler(bcAccountInfo->_session).PSendSysMessage(LANG_NO_PLAYERS_FOUND);
                    delete bcAccountInfo;
                    sWorldCallbackScript->_bcAccInfoCallback.Reset();
                    return;
                }

                int i = 0;
                uint32 count = 0;
                uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);
                do
                {
                    if (maxResults && count++ == maxResults)
                    {
                        ChatHandler(bcAccountInfo->_session).PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                        return;
                    }

                    Field* fields = result->Fetch();
                    uint32 acc_id = fields[0].GetUInt32();
                    std::string acc_name = fields[1].GetString();

                    if (bcAccountInfo->_session->GetSecurity() < AccountTypes(sAccountMgr->GetSecurity(acc_id, realmID)))
                        continue;

                    QueryResult chars = CharacterDatabase.PQuery("SELECT guid, name, online FROM characters WHERE account = '%u'", acc_id);
                    if (chars)
                    {
                        ChatHandler(bcAccountInfo->_session).PSendSysMessage(LANG_LOOKUP_PLAYER_ACCOUNT, acc_name.c_str(), acc_id);

                        uint64 guid = 0;
                        std::string name;
                        bool online = false;

                        do
                        {
                            Field* charfields = chars->Fetch();
                            guid = charfields[0].GetUInt64();
                            name = charfields[1].GetString();
                            online = charfields[2].GetBool();

                            ChatHandler(bcAccountInfo->_session).PSendSysMessage("  %s (GUID %u) %s", name.c_str(), GUID_LOPART(guid), online ? "|c00077766Online" : "|cffff0000Offline");
                            ++i;

                        } while (chars->NextRow() && (limit == -1 || i < limit));
                    }
                } while (result->NextRow());

                if (i == 0)                                                // empty accounts only
                {
                    ChatHandler(bcAccountInfo->_session).PSendSysMessage(LANG_NO_PLAYERS_FOUND);
                    delete bcAccountInfo;
                    sWorldCallbackScript->_bcAccInfoCallback.Reset();
                    return;
                }

                delete bcAccountInfo;
                sWorldCallbackScript->_bcAccInfoCallback.Reset();
            }
            break;
        }
    }
};

void AddSC_world_query_callback()
{
    new world_query_callback();
}
