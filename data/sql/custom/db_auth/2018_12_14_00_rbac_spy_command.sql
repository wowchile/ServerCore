DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 1076 AND 1082;
INSERT INTO `rbac_permissions` (`id`,`name`) VALUES
(1076,'Command: .spy'),
(1077,'Command: .spy follow'),
(1078,'Command: .spy groupfollow'),
(1079,'Command: .spy unfollow'),
(1080,'Command: .spy groupunfollow'),
(1081,'Command: .spy clear'),
(1082,'Command: .spy status');

DELETE FROM `rbac_role_permissions` WHERE `permissionId` BETWEEN 1076 AND 1082;
INSERT INTO `rbac_role_permissions` (`roleId`, `permissionId`) VALUES
(43, 1076),
(43, 1077),
(43, 1078),
(43, 1079),
(43, 1080),
(43, 1081),
(43, 1082);