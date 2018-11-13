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

enum Actions
{
    ACTION_NONE                 = 0,
    ACTION_SEND_ORDERS          = 1001,
    ACTION_SEND_GUILDHOUSES     = 1002,
    ACTION_SEND_SHIPHOUSES      = 1003,
    ACTION_REDEMPTION_CONFIRM   = 1004,
    ACTION_REDEMPTION_SUCCESS   = 1005,
    ACTION_HOUSE_GUILD_FAIL     = 1009,
    ACTION_HOUSE_TELE           = 1010,
    ACTION_HOUSE_TELE_GUEST     = 1011,
    ACTION_FAIL                 = 1012,
    ACTION_TRADE                = 1013,
    ACTION_CLOSE                = 1014
};

enum SpecialSpells
{
    SPELL_TELEPORT_TO_DALARAN   = 53360,
    SPELL_GHOST_NIGHT_ELF       = 20584,
    SPELL_GHOST                 = 8326
};

enum SpecialItems
{
    ITEM_GUILD_HOUSE    = 100000000,
    ITEM_SHIP_HOUSE     = 200000000,
    ITEM_CONFIRM_SPACER = 500000000,
    ITEM_CUSTOM_TEMPLATE_SPACER = 10000000
};

struct Redemption
{
    uint32 item;
    uint32 house;
};

class npc_store : public CreatureScript
{
public:
    npc_store() : CreatureScript("npc_store") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Muestrame mis ordenes pendietes, por favor.", GOSSIP_SENDER_MAIN, ACTION_SEND_ORDERS);

        if (House* house = sHouseMgr->GetHouseBy(player))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Llevame a mi House", GOSSIP_SENDER_MAIN, ACTION_HOUSE_TELE);

            HouseRoom* room = house->GetRoom(player->GetGuildId());

            std::stringstream str;
            uint32 elapsed  = room->GetEndTime() - uint32(time(NULL));
            uint32 segs     = elapsed % 60;
            uint32 mins     = elapsed / MINUTE % 60;
            uint32 hours    = elapsed / HOUR % 24;
            uint32 days     = elapsed / DAY;

            str << "Su House caduca en ";
            if (days > 0)
                str << days << " dias ";
            if (hours > 0)
                str << hours << " horas ";
            if (days < 1 && mins > 0)
                str << mins << " minutos ";
            if (hours < 1 && segs > 0)
                str << segs << " segundos";

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, str.str().c_str(), GOSSIP_SENDER_MAIN, ACTION_CLOSE);
        }

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, ACTION_TRADE);

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        if (action != ACTION_NONE)
            player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case ACTION_NONE:
                break;
            case ACTION_SEND_ORDERS:
                return OnGossipSelect(player, creature, sender, SendOrders(player));
            case ACTION_SEND_GUILDHOUSES:
                return OnGossipSelect(player, creature, sender, SendHouses(player, HOUSE_TYPE_GUILD));
            case ACTION_SEND_SHIPHOUSES:
                return OnGossipSelect(player, creature, sender, SendHouses(player, HOUSE_TYPE_SHIP));
            case ACTION_HOUSE_TELE:
                return OnGossipSelect(player, creature, sender, TeleHouse(player, sHouseMgr->GetHouseIdBy(player), false));
            case ACTION_HOUSE_TELE_GUEST:
                return OnGossipSelect(player, creature, sender, TeleHouse(player, _houseMemory[player->GetGUID()].house, true));
            case ACTION_HOUSE_GUILD_FAIL:
                player->GetSession()->SendNotification("No tienes Guild");
                return OnGossipSelect(player, creature, sender, ACTION_CLOSE);
            case ACTION_REDEMPTION_CONFIRM:
            {
                std::stringstream actionText;
                actionText << "Solicitar:  ";
                actionText << (_houseMemory[player->GetGUID()].item < ITEM_SHIP_HOUSE ? "Guild House " : "Ship House ") << _houseMemory[player->GetGUID()].house;

                std::stringstream actionConfirmText;
                actionConfirmText << "Estas seguro que deseas solicitar la House ";
                actionConfirmText << (_houseMemory[player->GetGUID()].item < ITEM_SHIP_HOUSE ? "Guild House " : "Ship House ") << _houseMemory[player->GetGUID()].house;
                actionConfirmText << "?";

                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, actionText.str().c_str(), GOSSIP_SENDER_MAIN, _houseMemory[player->GetGUID()].house + _houseMemory[player->GetGUID()].item, actionConfirmText.str().c_str(), 0, false);

                if (sHouseMgr->GetHouseIdBy(player) == 0)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Quiero dar un paseo de 2 minutos en ella", GOSSIP_SENDER_MAIN, ACTION_HOUSE_TELE_GUEST);
                break;
            }
            case ACTION_REDEMPTION_SUCCESS:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ahi tienes, todo tuyo", GOSSIP_SENDER_MAIN, ACTION_CLOSE);
                break;
            case ACTION_FAIL:
                player->GetSession()->SendNotification("Ha ocurrido un problema tecnico, vuelva mas tarde");
                return OnGossipSelect(player, creature, sender, ACTION_CLOSE);
            case ACTION_CLOSE:
                player->PlayerTalkClass->SendCloseGossip();
                return true;
            case ACTION_TRADE:
                player->PlayerTalkClass->ClearMenus();
                player->GetSession()->SendListInventory(creature->GetGUID());
                return true;
            default:
                return OnGossipSelect(player, creature, sender, ProcessItem(player, action));
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

        return true;
    }

    Actions SendOrders(Player* player)
    {
        PreparedStatement* stmt;

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ORDER_EXIST);
        stmt->setUInt32(0, player->GetSession()->GetAccountId());
        PreparedQueryResult result1 = CharacterDatabase.Query(stmt);
        if (!result1)
        {
            player->GetSession()->SendNotification("No tienes ordenes pendientes");
            return ACTION_CLOSE;
        }
        else
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ORDER);
            stmt->setUInt32(0, player->GetSession()->GetAccountId());
            PreparedQueryResult result2 = CharacterDatabase.Query(stmt);
            if (!result2)
            {
                player->GetSession()->SendNotification("No puedo ver las ordenes en este momento");
                return ACTION_CLOSE;
            }
            else
            {
                do
                {
                    Field* fields2 = result2->Fetch();

                    std::stringstream actionText;
                    actionText << "Retirar: ";
                    actionText << fields2[0].GetString();

                    std::stringstream actionConfirmText;
                    actionConfirmText << "Estas seguro que deseas retirar ";
                    actionConfirmText << fields2[0].GetString();
                    actionConfirmText << "?";

                    uint32 item = fields2[1].GetUInt32();

                    if (item != ITEM_GUILD_HOUSE && item != ITEM_SHIP_HOUSE)
                        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, actionText.str().c_str(), GOSSIP_SENDER_MAIN, item, actionConfirmText.str().c_str(), 0, false);
                    else
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, actionText.str().c_str(), GOSSIP_SENDER_MAIN, item);
                }
                while (result2->NextRow());
            }
        }

        return ACTION_NONE;
    }

    Actions SendHouses(Player* player, HouseTypeId typeId)
    {
        House* playerHouse = sHouseMgr->GetHouseBy(player);
        bool sended = false;

        for (HouseMgr::HouseSet::const_iterator itr = sHouseMgr->GetHouseSetBegin(); itr != sHouseMgr->GetHouseSetEnd(); itr++)
        {
            House* house = (*itr);

            if (house->GetTypeId() != typeId || !house->IsAvailable() ||                                                // NO monstrar houses que no correspondan al typeId solicitado, o que se encuentren bloqueadas
                (house->GetTeamId() != TEAM_NEUTRAL && house->GetTeamId() != player->GetTeamId()) ||                    // NO mostrar houses que no sean de su faccion, a excepecion de las nuetrales
                (playerHouse && playerHouse != house) ||                                                                // CON House -> NO mostrar House que no sea la suya
                (!playerHouse && house->IsInUse()) ||                                                                   // SIN House -> NO mostrar Houses en uso
                (playerHouse == house && !house->IsPhaseable() && house->IsInUse() && house->GetRoomsInUseSize() > 1))  // NO mostrar houses que se les ha desactivado el sistema de fase y aun no se desocupan
                continue;

            sended = true;

            std::stringstream str;
            str << (typeId == HOUSE_TYPE_GUILD ? "Guild House " : "Ship House ") << (uint32)house->GetHouseId() << " " << (house->IsPhaseable() ? "(Comun)" : "(Exclusiva)");
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, str.str().c_str(), GOSSIP_SENDER_MAIN, ITEM_CONFIRM_SPACER + house->GetHouseId() + (typeId == HOUSE_TYPE_GUILD ? ITEM_GUILD_HOUSE : ITEM_SHIP_HOUSE));
        }

        if (!sended)
        {
            std::stringstream str;
            str << "No hay " << (typeId == HOUSE_TYPE_GUILD ? "Guild" : "Ship") << " Houses disponibles";
            auto notif = str.str();
            player->GetSession()->SendNotification("%s", notif.c_str());
            return ACTION_CLOSE;
        }

        return ACTION_NONE;
    }

    void CreateHouseMemory(Player* player, uint32 item)
    {
        _houseMemory[player->GetGUID()].house = 0;

        item -= ITEM_CONFIRM_SPACER;

        if (item > ITEM_SHIP_HOUSE)
        {
            _houseMemory[player->GetGUID()].house = item - ITEM_SHIP_HOUSE;
            _houseMemory[player->GetGUID()].item = ITEM_SHIP_HOUSE;
        }
        else
        {
            _houseMemory[player->GetGUID()].house = item - ITEM_GUILD_HOUSE;
            _houseMemory[player->GetGUID()].item = ITEM_GUILD_HOUSE;
        }
    }

    uint8 GetOrderStack(Player* player, uint32 item)
    {
        PreparedStatement* stmt;
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ORDER_STACK);
        stmt->setUInt32(0, player->GetSession()->GetAccountId());
        stmt->setUInt32(1, item);
        PreparedQueryResult result = CharacterDatabase.Query(stmt);
        if (!result)
            return 0;
        else
        {
            Field* fields = result->Fetch();
            return fields[0].GetUInt8();
        }
    }

    Actions ProcessItem(Player* player, uint32 item)
    {
        uint8 stack = GetOrderStack(player, item);

        if (item > ITEM_CONFIRM_SPACER)
        {
            CreateHouseMemory(player, item);
            return ACTION_REDEMPTION_CONFIRM;
        }

        // ITEM_GUILD_HOUSE (100.000.000) es canjear una GuildHouse
        if (item == ITEM_GUILD_HOUSE)
        {
            if (stack != 1)
                return ACTION_FAIL;
            if (player->GetGuildId() == 0)
                return ACTION_HOUSE_GUILD_FAIL;
            return ACTION_SEND_GUILDHOUSES;
        }

        // ITEM_SHIP_HOUSE (200.000.000) es canjear una ShipHouse
        if (item == ITEM_SHIP_HOUSE)
        {
            if (stack != 1)
                return ACTION_FAIL;
            if (player->GetGuildId() == 0)
                return ACTION_HOUSE_GUILD_FAIL;
            return ACTION_SEND_SHIPHOUSES;
        }

        // Ship o Guild House seleccionada
        // ITEM_GUILD_HOUSE + XXX (100.000.XXX) ids de GuildHouses
        if (item > ITEM_GUILD_HOUSE)
        {
            uint32 house = item;
            // ITEM_SHIP_HOUSE + XXX (200.000.XXX) ids de ShipHouses
            if (item < ITEM_SHIP_HOUSE)
                house -= ITEM_GUILD_HOUSE;
            else
                house -= ITEM_SHIP_HOUSE;
            return UpdateHouse(player, item - house, house);
        }

        // Customize players
        if (item <= 128)
        {
            if (stack != 1)
                return ACTION_FAIL;
            player->SetAtLoginFlag(AtLoginFlags(item));
            return UpdateOrder(player, item);
        }

        if (stack < 1 || stack > 20)
            return ACTION_FAIL;

        // Normal items
        const ItemTemplate* itemTemplate = sObjectMgr->GetItemTemplate(item);
        if (itemTemplate && itemTemplate->AllowableRace & player->getRaceMask())
        {
            ItemPosCountVec dest;
            InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item, stack);
            if (msg == EQUIP_ERR_OK)
            {
                if (!dest.empty())
                {
                    Item* pItem = player->StoreNewItem(dest, item, true);
                    /* @TODO Los tabards blizzlike de adorno quedan inutilizables
                    if (item < ITEM_CUSTOM_TEMPLATE_SPACER && pItem->GetTemplate()->InventoryType != INVTYPE_NON_EQUIP)
                        pItem->SetOnlyTransmo();
                    */
                    player->SendNewItem(pItem, stack, true, true);
                    return UpdateOrder(player, item, stack);
                }
            }
            else
                player->SendEquipError(msg, NULL, NULL, item);
        }
        else
            player->SendEquipError(EQUIP_ERR_YOU_CAN_NEVER_USE_THAT_ITEM, NULL, NULL, item);

        return ACTION_FAIL;
    }

    Actions UpdateOrder(Player* player, uint32 item, uint32 stack = 1)
    {
        PreparedStatement* stmt;
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ORDER_STATUS);
        stmt->setUInt32(0, player->GetGUIDLow());
        stmt->setUInt32(1, player->GetSession()->GetAccountId());
        stmt->setUInt32(2, item);
        stmt->setUInt8(3, stack);
        CharacterDatabase.Query(stmt);
        return ACTION_REDEMPTION_SUCCESS;
    }

    Actions UpdateHouse(Player* player, uint32 item, uint8 houseId)
    {
        House* house = sHouseMgr->GetHouseBy(houseId);
        House* playerHouse = sHouseMgr->GetHouseBy(player);

        if (!house || !house->IsAvailable() || (!playerHouse && house->IsInUse()))
            return ACTION_FAIL;

        if (playerHouse && !house->GetRoom(player->GetGuildId()))
            return ACTION_HOUSE_GUILD_FAIL;

        house->Renew(player->GetGuildId());

        _houseMemory.erase(player->GetGUID());

        return UpdateOrder(player, item);
    }

    Actions TeleHouse(Player* player, uint32 id, bool guest)
    {
        House* house = sHouseMgr->GetHouseBy(id);
        if (!house)
            return ACTION_FAIL;

        if (guest)
        {
            player->CastSpell(player, player->getRace() == RACE_NIGHTELF ? SPELL_GHOST_NIGHT_ELF : SPELL_GHOST, true);
            if (Aura* aura = player->GetAura(player->getRace() == RACE_NIGHTELF ? SPELL_GHOST_NIGHT_ELF : SPELL_GHOST))
                aura->SetDuration(2 * MINUTE * IN_MILLISECONDS);
            else
                return ACTION_FAIL;

            _houseMemory.erase(player->GetGUID());
        }

        player->CastSpell(player, 12438, true); // Slow fall
        player->TeleportTo(house->GetMapId(), house->GetPositionX(), house->GetPositionY(), house->GetPositionZ() + 10.0f, player->GetOrientation());

        return ACTION_CLOSE;
    }

private:
    typedef std::map<uint64, Redemption> Memory;

    Memory _houseMemory;
};

void AddSC_npc_store()
{
    new npc_store();
}
