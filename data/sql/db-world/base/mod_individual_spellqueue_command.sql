-- COMMAND
DELETE FROM `command` WHERE `name` IN ('spellqueue', 'spellqueue set', 'spellqueue view', 'spellqueue default', 'spellqueue enable', 'spellqueue disable', 'spellqueue latency');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('spellqueue', 0, 'Syntax: .spellqueue $subcommand\nType .help spellqueue to see a list of subcommands\nor .help spellqueue $subcommand to see info on the subcommand.'),
('spellqueue set', 0, 'Syntax: .spellqueue set $window\nSet your custom spellqueue window.'),
('spellqueue view', 0, 'Syntax: .spellqueue view\nView your current spellqueue window.'),
('spellqueue default', 0, 'Syntax: .spellqueue default\nSet your custom spellqueue window to the default value'),
('spellqueue enable', 0, 'Syntax: .spellqueue enable\nEnable the custom spellqueue window.'),
('spellqueue disable', 0, 'Syntax: .spellqueue disable\nDisable the custom spellqueue window.'),
('spellqueue latency', 0, 'Syntax: .spellqueue latency\nToggle latency compensation.');
