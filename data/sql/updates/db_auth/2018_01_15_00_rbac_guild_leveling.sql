-- Add new permissions
DELETE FROM `rbac_permissions` WHERE `id` IN (1051, 1052, 1053, 1054, 1055, 1056);
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(1051, 'Command: guild leveling'),
(1052, 'Command: guild leveling info'),
(1053, 'Command: guild leveling bonus'),
(1054, 'Command: guild leveling set'),
(1055, 'Command: guild leveling set level'),
(1056, 'Command: guild leveling set xp');

-- Add Permissions to "Administrator Commands Role"
DELETE FROM `rbac_role_permissions` WHERE `permissionId` IN (1051, 1052, 1053, 1054, 1055, 1056);
INSERT INTO `rbac_role_permissions` (`roleId`, `permissionId`) VALUES
(1, 1051),
(1, 1052),
(1, 1053),
(43, 1054),
(43, 1055),
(43, 1056);