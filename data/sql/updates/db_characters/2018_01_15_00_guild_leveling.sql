ALTER TABLE `guild` ADD `xp` INT(10) UNSIGNED DEFAULT 0 NOT NULL AFTER `BankMoney`;
ALTER TABLE `guild` ADD `level` TINYINT(3) UNSIGNED DEFAULT 1 NOT NULL AFTER `xp`;

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `battlecl_guild_leveling_bonus_config`
-- ----------------------------
DROP TABLE IF EXISTS `battlecl_guild_leveling_bonus_config`;
CREATE TABLE `battlecl_guild_leveling_bonus_config` (
  `BonusId` smallint(5) UNSIGNED NOT NULL DEFAULT '0',
  `RequiredGuildLevel` tinyint(3) UNSIGNED NOT NULL DEFAULT '0',
  `comment` longtext CHARACTER SET utf8
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `battlecl_guild_leveling_bonus_config` VALUES
('1', '4', 'Gold Bonus GuildBank (Rank 1)  | 5% Bonus gold for the guild bank'),                                       -- GUILD_LEVELING_BONUS_GOLD_GUILD_BANK_RANK_1          = 1,
('2', '2', 'Amount Experience (Rank 1)     | +5% XP at the end of quest or kill monsters'),                            -- GUILD_LEVELING_BONUS_XP_RANK_1                       = 2,
('3', '2', 'Faster Spirit                  | Double movement speed as spirit'),                                        -- GUILD_LEVELING_BONUS_FASTER_SPIRIT                   = 3,
('4', '6', 'Low Cost Repair (Rank 1)       | -25% Repair costs'),                                                      -- GUILD_LEVELING_BONUS_LOW_COST_REPAIR_RANK_1          = 4,
('5', '8', 'Gold Bonus GuildBank (Rank 2)  | 10% Bonus gold for the guild bank'),                                      -- GUILD_LEVELING_BONUS_GOLD_GUILD_BANK_RANK_2          = 5,
('6', '5', 'Mount Speed (Rank 1)           | +5% Increased mount speed (Except Battlegrounds & Arenas)'),              -- GUILD_LEVELING_BONUS_MOUNT_SPEED_RANK_1              = 6,
('7', '3', 'Amount Reputation  (Rank 1)     | +5% Reputation gain'),                                                    -- GUILD_LEVELING_BONUS_AMOUNT_REPUTATION_RANK_1        = 7,
('8', '4', 'Amount Experience (Rank 2)     | +10% XP at the end of quest or kill monsters'),                           -- GUILD_LEVELING_BONUS_XP_RANK_2                       = 8,
('9', '6', 'Low Cost Repair (Rank 2)       | -50% Repair costs'),                                                      -- GUILD_LEVELING_BONUS_LOW_COST_REPAIR_RANK_2          = 9,
('10', '6', 'Mount Speed (Rank 2)          | +10% Increased mount speed (Except Battlegrounds & Arenas)'),             -- GUILD_LEVELING_BONUS_MOUNT_SPEED_RANK_2              = 10,
('11', '7', 'Amount Reputation  (Rank 2)   | +10% Reputation gain'),                                                   -- GUILD_LEVELING_BONUS_AMOUNT_REPUTATION_RANK_2        = 11,
('12', '5', 'Amount Honor (Rank 1)         | +5% Honor'),                                                              -- GUILD_LEVELING_BONUS_AMOUNT_HONOR_RANK_1             = 12,
('13', '8', 'Amount Honor (Rank 2)         | +10% Honor'),                                                             -- GUILD_LEVELING_BONUS_AMOUNT_HONOR_RANK_2             = 13,
('14', '8', 'Guild Vault Mobile                | item guild vault mobile');                                                -- GUILD_LEVELING_BONUS_VAULT_MOBILE                    = 14,


SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `battlecl_guild_leveling_xp_for_next_level`
-- ----------------------------
DROP TABLE IF EXISTS `battlecl_guild_leveling_xp_for_next_level`;
CREATE TABLE `battlecl_guild_leveling_xp_for_next_level` (
  `level` tinyint(3) NOT NULL DEFAULT '0',
  `xp_for_next_level` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `battlecl_guild_leveling_xp_for_next_level` VALUES
('2', '8000000'),
('3', '12000000'),
('4', '18000000'),
('5', '27000000'),
('6', '40500000'),
('7', '60750000'),
('8', '91125000'),
('9', '136687500');