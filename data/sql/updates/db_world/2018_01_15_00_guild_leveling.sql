DELETE FROM `trinity_string` WHERE `entry` BETWEEN 11328 AND 11332;
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
('11328', '|cff00ffff============== GUILD LEVELING ==============', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
('11329', '|Cff00ff00 Nivel:|r %u.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
('11330', '|Cff00ff00 Experiencia:|r %u de %u.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
('11331', '|cff00ffff============================================', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
('11332', 'Guild Leveling System: La Hermandad %s ha alcanzado el nivel %d.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

DELETE FROM `command` WHERE `permission` BETWEEN 1051 AND 1056;
INSERT INTO `command` (`name`,`permission`,`help`) VALUES
('guild leveling info', 1052, 'Syntax: .guild leveling info\r\nObtienes informacion sobre el nivel de hermandad y la experiencia necesaria para avanzar al siguiente nivel.\r\n\nBattleCore'),
('guild leveling bonus', 1053, 'Syntax: .guild leveling bonus\r\nObtienes informacion detallada sobre los bonus activos.\r\n\nBattleCore'),
('guild leveling set level', 1055, 'Syntax: .guild leveling set level $Level "$GuildName"\r\nAsignas nivel a una hermandad.\r\n\nBattleCore'),
('guild leveling set xp', 1056, 'Syntax: .guild leveling set exp $Xp "$GuildName"\r\nAsignas experiencia a una hermandad.\r\n\nBattleCore');

-- add item guild vault mobile in npc redeem
SET @REDEEM_ENRTY = 5000002;
SET @ITEM_ENTRY = 1000012;
SET @VENDOR_ENTRY = @REDEEM_ENRTY;

DELETE FROM `item_template` WHERE `entry`= @ITEM_ENTRY;
INSERT INTO `item_template` (`entry`, `class`, `subclass`, `SoundOverrideSubclass`, `name`, `displayid`, `Quality`, `Flags`, `FlagsExtra`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `flagsCustom`, `VerifiedBuild`) VALUES
(@ITEM_ENTRY, 4, 0, -1, 'Guild Vault', 60274, 5, 0, 0, 1, 0, 0, 0, -1, -1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18282, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 1, 'Guild Vault Mobile', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 'custom_item_guild_vault_mobile', 0, 0, 0, 0, 0, 0);


DELETE FROM `npc_vendor` WHERE `item`= @ITEM_ENTRY;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(@VENDOR_ENTRY, 0, @ITEM_ENTRY, 0, 0, 0);