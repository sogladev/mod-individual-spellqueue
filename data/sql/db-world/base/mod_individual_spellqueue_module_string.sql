-- module string
SET @MODULE_STRING := 'individual-spellqueue';
DELETE FROM `module_string` WHERE `module` = @MODULE_STRING;
INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
(@MODULE_STRING, 1, '|cffffffff Your current spellqueue window is {}.|r'),
(@MODULE_STRING, 2, '|cffffffff The maximum rate limit is {}.|r'),
(@MODULE_STRING, 3, '|cffffffff The minimum rate limit is {}.|r'),
(@MODULE_STRING, 4, 'You have updated your spellqueue window to {}.'),
(@MODULE_STRING, 5, 'You have disabled your spellqueue.'),
(@MODULE_STRING, 6, 'You have enabled your spellqueue with a window value of {}.'),
(@MODULE_STRING, 7, 'You have restored your spellqueue window to the default value of {}.'),
(@MODULE_STRING, 8, 'The individual-spellqueue-module is disabled.');

-- 'deDE', 'ruRU', 'ruRU', 'koKR', 'frFR', 'zhCN', 'zhTW', 'esES', 'esMX',
DELETE FROM `module_string_locale` WHERE `module` = @MODULE_STRING;
-- INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `string`) VALUES
-- (@MODULE_STRING, 0 , 'deDE', ''),

