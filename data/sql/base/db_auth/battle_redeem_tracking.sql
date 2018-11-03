CREATE TABLE `battle_redeem_tracking` (
  `guid` int(10) NOT NULL AUTO_INCREMENT,
  `account` int(10) NOT NULL DEFAULT '0',
  `giftFrom` int(10) NOT NULL DEFAULT '0',
  `characters` int(10) NOT NULL DEFAULT '0',
  `orderDate` int(10) NOT NULL DEFAULT '0',
  `redemptionDate` int(10) NOT NULL DEFAULT '0',
  `item` int(10) NOT NULL DEFAULT '0',
  `stack` int(10) NOT NULL DEFAULT '1',
  `realm` int(1) NOT NULL DEFAULT '0',
  `method` int(1) NOT NULL DEFAULT '0',
  `status` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM AUTO_INCREMENT=23391 DEFAULT CHARSET=utf8;
