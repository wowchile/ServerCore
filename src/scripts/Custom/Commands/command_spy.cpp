#include "ScriptPCH.h"
#include "Channel.h"
#include "Group.h"

std::map<uint32,uint32> gmListening;//mapa para almacenar a los q se buscan
std::map<uint32,uint32> gmListeningGroup;

class chat_spy_script : public PlayerScript
{
public:
    chat_spy_script() : PlayerScript("chat_spy_script") { }

    void OnChat(Player *sender, uint32 /*type*/, uint32 lang, std::string& msg, Player *receiver)
    {
        if (gmListening.size() > 0 && lang != LANG_ADDON)
        {
            if(Player* gm = GetGmFromList(sender))
                ChatHandler(gm->GetSession()).PSendSysMessage("[WHISPER] %s tells %s: %s",
                    sender->GetName().c_str(), receiver ? receiver->GetName().c_str() : "<unknown>", msg.c_str());
            if(Player* gm = GetGmFromList(receiver))
                ChatHandler(gm->GetSession()).PSendSysMessage("[WHISPER] %s tells %s: %s",
                    sender->GetName().c_str(), receiver ? receiver->GetName().c_str() : "<unknown>", msg.c_str());
        }
    }

    void OnChat(Player* sender, uint32 /*type*/, uint32 lang, std::string& msg, Group* group)
    {
        if(gmListeningGroup.size() > 0 && lang != LANG_ADDON)
            if(Player* gm = GetGmFromGroupList(group))
                ChatHandler(gm->GetSession()).PSendSysMessage("[Group] %s tell: %s",
                    sender->GetName().c_str(), msg.c_str());
    }

    Player* GetGmFromList(Player* from)
    {
        std::map<uint32,uint32>::const_iterator itr = gmListening.find(from->GetGUIDLow());
        return itr != gmListening.end() ? sObjectMgr->GetPlayerByLowGUID(itr->second) : NULL;
    }

    Player* GetGmFromGroupList(Group* group)
    {
        std::map<uint32,uint32>::const_iterator itr = gmListeningGroup.find(group->GetLowGUID());
        return itr != gmListeningGroup.end() ? sObjectMgr->GetPlayerByLowGUID(itr->second) : NULL;
    }
};

class chat_spy_commandscript : public CommandScript
{
public:
    chat_spy_commandscript() : CommandScript("chat_spy_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> spyCommandTable =
        {
            { "follow",         RBAC_PERM_COMMAND_SPY_FOLLOW,       false,  &HandleSpyFollowCommand,        ""},
            { "groupfollow",    RBAC_PERM_COMMAND_SPY_GROUPFOLLOW,  false,  &HandleSpyFollowGroupCommand,   ""},
            { "unfollow",       RBAC_PERM_COMMAND_SPY_UNFOLLOW,     false,  &HandleSpyUnFollowCommand,      ""},
            { "groupunfollow",  RBAC_PERM_COMMAND_SPY_GROUPUNFOLLOW,false,  &HandleSpyUnFollowGroupCommand, ""},
            { "clear",          RBAC_PERM_COMMAND_SPY_CLEAR,        false,  &HandleSpyClearCommand,         ""},
            { "status",         RBAC_PERM_COMMAND_SPY_STATUS,       false,  &HandleSpyStatusCommand,        ""}
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "spy",            RBAC_PERM_COMMAND_SPY,  false,  NULL, "", spyCommandTable}
        };

        return commandTable;
    }

    static bool HandleSpyFollowCommand(ChatHandler* handler, const char* args)
    {
        //.spy follow nombre
        if(!*args)
            return false;
        //Obtenemos el nombre
        char *cName = strtok ((char*)args," ");
        if(!cName)
            return false;
        std::string pTarget = args;
        if(!normalizePlayerName(pTarget))
        {
            handler->SendSysMessage("Player no encontrado");
            return true;
        }
        if(Player* target = ObjectAccessor::FindPlayer(sObjectMgr->GetPlayerGUIDByName(pTarget.c_str())))
        {
            std::map<uint32,uint32>::const_iterator itr = gmListening.find(target->GetGUIDLow());
            if(itr != gmListening.end())
                if(Player* gm = sObjectMgr->GetPlayerByLowGUID(itr->second))
                {
                    handler->PSendSysMessage("%s ya esta siendo seguido por: %s",target->GetName().c_str(),gm->GetName().c_str());
                    return true;
                }

                gmListening[target->GetGUIDLow()] = handler->GetSession()->GetPlayer()->GetGUIDLow();
                handler->PSendSysMessage("Siguiendo a: %s",target->GetName().c_str());
        }
        else
            handler->SendSysMessage("Player no encontrado");
        return true;
    }

    static bool HandleSpyFollowGroupCommand(ChatHandler* handler, const char* args)
    {
        //.spy followraid nombre
        if(!*args)
            return false;

        //Obtenemos el nombre
        char *cName = strtok ((char*)args," ");
        if(!cName)
            return false;
        std::string pTarget = args;
        if(!normalizePlayerName(pTarget))
        {
            handler->SendSysMessage("Player no encontrado");
            return true;
        }
        if(Player* target = ObjectAccessor::FindPlayer(sObjectMgr->GetPlayerGUIDByName(pTarget.c_str())))
        {
            if(Group* group = target->GetGroup())
            {
                std::map<uint32,uint32>::const_iterator itr = gmListeningGroup.find(group->GetLowGUID());
                if(itr != gmListeningGroup.end())
                    if(Player* gm = sObjectMgr->GetPlayerByLowGUID(itr->second))
                    {
                        handler->PSendSysMessage("%s ya esta siendo seguido por: %s",target->GetName().c_str(),gm->GetName().c_str());
                        return true;
                    }

                    gmListeningGroup[group->GetLowGUID()] = handler->GetSession()->GetPlayer()->GetGUIDLow();
                    handler->PSendSysMessage("Siguiendo grupo de: %s",target->GetName().c_str());
            }
            else
                handler->SendSysMessage("El jugador no esta en grupo");
        }
        else
            handler->SendSysMessage("Player no encontrado");
        return true;
    }

    static bool HandleSpyUnFollowCommand(ChatHandler* handler, const char* args)
    {
        //.spy follow nombre
        if(!*args)
            return false;
        //Obtenemos el nombre
        char *cName = strtok ((char*)args," ");
        if(!cName)
            return false;

        std::string pTarget = args;
        if(!normalizePlayerName(pTarget))
        {
            handler->SendSysMessage("Player no encontrado");
            return true;
        }
        if(Player* target = ObjectAccessor::FindPlayer(sObjectMgr->GetPlayerGUIDByName(pTarget.c_str())))
        {
            handler->PSendSysMessage("Dejando de seguir a: %s",cName);
            if(gmListening.find(target->GetGUIDLow()) != gmListening.end())
                gmListening.erase(target->GetGUIDLow());
        }
        else
            handler->SendSysMessage("Player no encontrado");
        return true;
    }

    static bool HandleSpyUnFollowGroupCommand(ChatHandler* handler, const char* args)
    {
        //.spy follow nombre
        if(!*args)
            return false;
        //Obtenemos el nombre
        char *cName = strtok ((char*)args," ");
        if(!cName)
            return false;

        std::string pTarget = args;
        if(!normalizePlayerName(pTarget))
        {
            handler->SendSysMessage("Player no encontrado");
            return true;
        }
        if(Player* target = ObjectAccessor::FindPlayer(sObjectMgr->GetPlayerGUIDByName(pTarget.c_str())))
        {
            if(Group* group = target->GetGroup())
            {
                if(gmListeningGroup.find(group->GetLowGUID()) != gmListening.end())
                {
                    gmListeningGroup.erase(group->GetLowGUID());
                    handler->PSendSysMessage("Dejando de seguir el grupo de: %s",cName);
                }
                else
                    handler->PSendSysMessage("No estas siguiendo el grupo de: %s",cName);
            }
            else
                handler->SendSysMessage("El jugador no esta en grupo");
        }
        else
            handler->SendSysMessage("Player no encontrado");
        return true;
    }

    static bool HandleSpyClearCommand(ChatHandler* handler, const char* /*args*/)
    {
        gmListening.clear();
        handler->SendSysMessage("Lista limpiada");
        return true;
    }

    static bool HandleSpyStatusCommand(ChatHandler* handler, const char* args)
    {
        uint32 guidlow = handler->GetSession()->GetPlayer()->GetGUIDLow();
        std::map<uint32,uint32>::iterator next;
        handler->SendSysMessage("Siguiendo a: ---------");
        for(std::map<uint32,uint32>::iterator itr = gmListening.begin(); itr != gmListening.end();itr = next)
        {
            //Existe la posibilidad que se borre itr por lo que salvamos el siguiente
            next = itr;
            ++next;
            if(itr->second == guidlow)
                if(Player* target = ObjectAccessor::FindPlayer(itr->first))
                    handler->PSendSysMessage("%s",target->GetName().c_str());
                else //Aprovechamos de limpiar los offline
                    gmListening.erase(itr);
        }
        handler->SendSysMessage("End ------------------");
        return true;
    }

};

class chat_spy_logout_cleaner : public PlayerScript
{
public:
    chat_spy_logout_cleaner() : PlayerScript("chat_spy_logout_cleaner") { }

    void OnLogout(Player* player)
    {
        if(player && player->IsGameMaster())
        {
            uint32 guidlow = player->GetGUIDLow();
            std::map<uint32,uint32>::iterator next;
            for(std::map<uint32,uint32>::iterator itr = gmListening.begin(); itr != gmListening.end();itr = next)
            {
                next = itr;//itr se invalida por lo que lo guardamos
                ++next;
                if(itr->second == guidlow)
                    gmListening.erase(itr);
            }
        }
    }
};

void AddSC_chat_spy()
{
    new chat_spy_script();
    new chat_spy_commandscript();
    new chat_spy_logout_cleaner();
}
