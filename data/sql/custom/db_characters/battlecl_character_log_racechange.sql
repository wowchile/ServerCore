DROP TABLE IF EXISTS `battlecl_character_log_racechange`;
CREATE TABLE `battlecl_character_log_racechange` (
  `guid` int(10) unsigned NOT NULL,
  `oldRace` tinyint(3) unsigned NOT NULL,
  `newRace` tinyint(3) unsigned NOT NULL,
  `time` int(10) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;