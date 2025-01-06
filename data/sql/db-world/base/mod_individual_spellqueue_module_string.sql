-- module string
SET @MODULE_STRING := 'individual-spellqueue';
DELETE FROM `module_string` WHERE `module` = @MODULE_STRING;
INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
(@MODULE_STRING, 1, 'Your current spellqueue window is {}.'),
(@MODULE_STRING, 2, 'The maximum rate limit is {}.'),
(@MODULE_STRING, 3, 'The minimum rate limit is {}.'),
(@MODULE_STRING, 4, 'You have updated your spellqueue window to {}.'),
(@MODULE_STRING, 5, 'You have disabled your spellqueue.'),
(@MODULE_STRING, 6, 'You have enabled your spellqueue with a window of {}.'),
(@MODULE_STRING, 7, 'You have restored your spellqueue window to the default value of {}.'),
(@MODULE_STRING, 8, 'The individual-spellqueue-module is disabled.'),
(@MODULE_STRING, 9, 'You have enabled spellqueue latency compensation with a window of {}.'),
(@MODULE_STRING, 10, 'You have disabled spellqueue latency compensation with a window of {}.');

-- 'deDE', 'ruRU', 'ruRU', 'koKR', 'frFR', 'zhCN', 'zhTW', 'esES', 'esMX',
DELETE FROM `module_string_locale` WHERE `module` = @MODULE_STRING;
-- INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `string`) VALUES
-- (@MODULE_STRING, 0 , 'deDE', ''),

