UPDATE `world`.`gameobject_loot_template` SET `groupid`='0' WHERE  `entry`=27078 AND `item`=45788; -- Move Freya's Sigil (n) to group 0
UPDATE `world`.`gameobject_loot_template` SET `groupid`='0' WHERE  `entry`=27079 AND `item`=45814; -- Move Freya's Sigil (h) to group 0
DELETE FROM `world`.`gameobject_loot_template` WHERE  `entry` IN (27078, 27079) AND `item`=47241; -- Delete duplicated entry for Emblem of Triump
DELETE FROM `world`.`gameobject_loot_template` WHERE  `entry`=27079 AND `item`=5; -- Delete duplicated entry for tokens