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

class command_json : public CommandScript
{
public:
    command_json() : CommandScript("command_json") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand jsonCommandTable[] =
        {
            { "server",                 RBAC_PERM_COMMAND_JSON_SERVER,              true, &HandleJsonServer,                "", NULL},
            { NULL,                     0,                                         false, NULL,                             "", NULL}
        };

        static ChatCommand commandTable[] =
        {
            { "json",                   RBAC_PERM_COMMAND_JSON,                   true,  NULL,                  "", jsonCommandTable},
            { NULL,                     0,                                       false,  NULL,                              "", NULL}
        };

        return commandTable;
    }

    // .json server
    static bool HandleJsonServer(ChatHandler* handler,const char*args)
    {
        uint32 timeInSecs = sWorld->GetUptime();

        uint32 secs    = timeInSecs % MINUTE;
        uint32 minutes = timeInSecs % HOUR / MINUTE;
        uint32 hours   = timeInSecs % DAY / HOUR;
        uint32 days    = timeInSecs / DAY;

        uint32 players           = sWorld->GetPlayerCount();
        uint32 maxPlayers        = sWorld->GetMaxPlayerCount();
        uint32 activeClients     = sWorld->GetActiveSessionCount();
        uint32 queuedClients     = sWorld->GetQueuedSessionCount();
        uint32 maxActiveClients  = sWorld->GetMaxActiveSessionCount();
        uint32 maxQueuedClients  = sWorld->GetMaxQueuedSessionCount();

        handler->PSendSysMessage("{\"secs\":\"%u\",\"minutes\":\"%u\",\"hours\":\"%u\",\"days\":\"%u\",\"players\":\"%u\",\"maxPlayers\":\"%u\",\"activeClients\":\"%u\",\"queuedClients\":\"%u\",\"maxActiveClients\":\"%u\",\"maxQueuedClients\":\"%u\"}", secs, minutes, hours, days, players, maxPlayers, activeClients, queuedClients, maxActiveClients, maxQueuedClients);

        return true;
    }
};

void AddSC_command_json()
{
    new command_json();
}
