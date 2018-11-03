DROP TABLE IF EXISTS `battlecl_character_log_rename`;
CREATE TABLE `battlecl_character_log_rename` (
  `guid` int(10) unsigned NOT NULL,
  `oldName` varchar(12) NOT NULL,
  `newName` varchar(12) NOT NULL,
  `time` int(10) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;