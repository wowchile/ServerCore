-- Obtenido de https://github.com/TrinityCore/TrinityCore/issues/4682
-- Autor: Odyssey

-- Fix quest 10722 - Meeting at the Blackwing Coven
SET @QUEST_MEETING_AT_THE_BLACKWING_COVEN := 10722;
SET @NPC_KOLPHIS := 22019;
SET @NPC_KOLPHIS_FIRST_GOSSIP := 8436;
SET @NPC_KOLPHIS_GOSSIP_QUESTCREDIT := 8439;
SET @SPELL_DISGUISE := 38157;

-- Gossip menu inserts from Aokromes sniff
DELETE FROM `gossip_menu` WHERE `entry` IN (8436,8435,8437,8438,8439,8440);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES
(8436,10539),
(8435,10541),
(8437,10542),
(8438,10543),
(8439,10544),
(8440,10545);

-- Add texts from http://www.wowpedia.org/Quest:Meeting_at_the_Blackwing_Coven#Quotes
DELETE FROM `npc_text` WHERE `id` IN (10539,10541,10542,10543,10544,10545);
INSERT INTO `npc_text` (`id`, `text0_0`) VALUES
(10539, 'Nuaar, are you feeling well? You don"t look like yourself today.'),
(10541, 'Right, the meeting. Let"s get down to it then.$B$BThe fact is that the lumber and livestock being gathered from the Ruuan Weald, which you"re responsible for, has slowed to a trickle.$B$BWe need those resources, but you"ve allowed the druids of the Cenarion Expedition to get in the way!'),
(10542, 'I don"t want excuses, I demand results!$B$BYou think what we"ve doing here is a joke?$B$BIf we don"t do this right, then not only will the so-called do-gooders come calling, but we"ll be found unworthy for elevation within the ranks of the Blackwing.'),
(10543, 'You could at least try to sound a little bit convincing.$B$BLook, Nuaar I wasn"t going to tell you this, but I might as well because it"s going to involve you, too.$B$BMaxnar Is planning an all-out attack on the druids at Ruuan Weald. And he intends to wipe them out.$B$BIt"s bad enough that we"ve beend fighting with the Boulder"mok ogres, so we can"t afford another front to deal with. I"ve arrangged for a temporary truce with the Arrakkoa.$B$BWell, what do you think?'),
(10544, 'We"ve putting the final preparations together even as we speak. The rest will depend upon how quickly you can organize you forces at Ruuan Weald.$B$BDo you think that you can handle that and get done quickly?'),
(10545, 'That"s the spirit!$B$BI"m glad that we had this little meeting. I feel much better about the attack now. With leader like you on the front, how can we lose?$B$BAlright, you have your marching orders. Now get back to the Ruuan Weald and make it happen!');

-- Add options to each menu from Aok sniff & wowpedia
DELETE FROM `gossip_menu_option` WHERE `menu_id` IN (8436,8435,8437,8438,8439,8440);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`) VALUES
(8436,0,0, 'I"m fine, thank you. You asked for me?',1,1,8435),
(8435,0,0, 'Oh, it"s not my fault, I can assure you.',1,1,8437),
(8437,0,0, 'Um, no... no, I don"t want that at all.',1,1,8438),
(8438,0,0, 'Impressive. When do we attack?',1,1,8439),
(8439,0,0, 'Absolutely!',1,1,8440);

-- Update NPC with first gossip_menu_id
UPDATE `creature_template` SET `npcflag`=`npcflag`|1, `gossip_menu_id`=@NPC_KOLPHIS_FIRST_GOSSIP WHERE `entry`=@NPC_KOLPHIS;

-- Add SAI to npc
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@NPC_KOLPHIS;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@NPC_KOLPHIS AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@NPC_KOLPHIS,0,0,0,62,0,100,0,@NPC_KOLPHIS_GOSSIP_QUESTCREDIT,0,0,0,15,@QUEST_MEETING_AT_THE_BLACKWING_COVEN,0,0,0,0,0,7,0,0,0,0,0,0,0, 'Kolphis - On 5th Gossip Select - Give Credit');

-- Add conditions
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=15 AND `SourceGroup`=@NPC_KOLPHIS_FIRST_GOSSIP;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionValue1`, `Comment`) VALUES
(15,@NPC_KOLPHIS_FIRST_GOSSIP,0,1,9,@QUEST_MEETING_AT_THE_BLACKWING_COVEN, 'Gossip 8436 available if player has Q10722'),
(15,@NPC_KOLPHIS_FIRST_GOSSIP,0,1,1,@SPELL_DISGUISE, 'Gossip 8436 available if player has Overseer Disguise aura');