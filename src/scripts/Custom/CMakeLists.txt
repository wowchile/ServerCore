# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  ${AC_SCRIPTS_DIR}/Custom/world_query_callback.cpp
  ${AC_SCRIPTS_DIR}/Custom/world_query_callback.h
  ${AC_SCRIPTS_DIR}/Custom/Commands/command_bc.cpp
  ${AC_SCRIPTS_DIR}/Custom/Commands/command_graveyard.cpp
  ${AC_SCRIPTS_DIR}/Custom/Commands/command_phase_system.cpp
  ${AC_SCRIPTS_DIR}/Custom/Commands/command_spy.cpp
  ${AC_SCRIPTS_DIR}/Custom/Item/item_arena_battlemaster_summoner.cpp
  ${AC_SCRIPTS_DIR}/Custom/Item/item_custom_rates.cpp
  ${AC_SCRIPTS_DIR}/Custom/Item/item_shirt_illusion.cpp
  ${AC_SCRIPTS_DIR}/Custom/Item/item_guild_vault_mobile.cpp
  ${AC_SCRIPTS_DIR}/Custom/Npc/npc_house_trigger.cpp
  ${AC_SCRIPTS_DIR}/Custom/Npc/npc_store.cpp
)

AC_ADD_SCRIPT_LOADER("Custom" "ScriptLoader.h")

message("  -> Prepared: Custom scripts")