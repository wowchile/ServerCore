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

#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "Guild.h"
#include "Player.h"

enum Vaults {
    GOBJECT_GUILD_VAULT_MOBILE = 980001
};

class custom_item_guild_vault_mobile : public ItemScript
{
public:
    custom_item_guild_vault_mobile() : ItemScript("custom_item_guild_vault_mobile"){}

    bool OnUse(Player * player, Item * /*item*/, SpellCastTargets const& /*targets*/)
    {
        if (!player->GetGuild()) {
            player->GetSession()->SendNotification("No perteneces a una guild");
            return false;
        }

        GameObject* guildVault = player->FindNearestGameObject(GOBJECT_GUILD_VAULT_MOBILE, 30.f);
        if (guildVault && guildVault->IsEventVisibleFor(player))
            player->GetSession()->SendNotification("Ya existe un Guild Vault Mobile invocado");
        else if (player->IsInCombat())
            player->GetSession()->SendNotification("No puedes utilizar este item en combate");
        else if (player->isMoving() || player->IsFlying() || player->IsFalling() || player->IsInFlight())
            player->GetSession()->SendNotification("No puedes utilizar este item en movimiento");
        else if (player->GetMap()->Instanceable() || player->GetTransport())
            player->GetSession()->SendNotification("No puedes invocar el Guild Vault aquí");
        else
        {
            guildVault = player->SummonGameObject(GOBJECT_GUILD_VAULT_MOBILE, player->GetPositionX() + 1, player->GetPositionY() + 1, player->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, MINUTE * IN_MILLISECONDS);
            guildVault->SetEventVisibleFor(player, true);
            guildVault->EnableCollision(false);
            return true;
        }

        return false;
    }
};

void AddSC_item_custom_guild_leveling_system()
{
    new custom_item_guild_vault_mobile();
}
