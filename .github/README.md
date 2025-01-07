# AzerothCore Module Individual Spellqueue

- Latest build status with azerothcore:

[![Build Status](
https://github.com/sogladev/mod-individual-spellqueue/actions/workflows/core-build.yml/badge.svg?branch=master&event=push)](https://github.com/sogladev/mod-individual-spellqueue)

This is a module for [AzerothCore](http://www.azerothcore.org) that allows players to set a custom spellqueue window per character

- Modifies SpellQueue.Window to use custom values per character

WIP: requires hook patch commit https://github.com/azerothcore/azerothcore-wotlk/commit/f77b4a97d1c1

## How to install
https://www.azerothcore.org/wiki/installing-a-module

1. Requires source recompilation
2. Modify config
  found in `/etc/modules`, copy `.conf.dist` to `.conf` and edit
3. Apply database changes: this should be done automaticly

## How to remove

1. Undo database changes: 

```
acore_characters:
DROP TABLE `individual_spellqueue`;

acore_world:
SET @MODULE_STRING := 'individual-spellqueue';
DELETE FROM `command` WHERE `name` IN ('spellqueue', 'spellqueue set', 'spellqueue view', 'spellqueue default', 'spellqueue enable', 'spellqueue disable');
DELETE FROM `module_string` WHERE `module` = @MODULE_STRING;
DELETE FROM `module_string_locale` WHERE `module` = @MODULE_STRING;
```
2. Remove `mod-individual-spellqueue` folder

## How to create your own module

1. Use the script `create_module.sh` located in [`modules/`](https://github.com/azerothcore/azerothcore-wotlk/tree/master/modules) to start quickly with all the files you need and your git repo configured correctly (heavily recommended).
1. You can then use these scripts to start your project: https://github.com/azerothcore/azerothcore-boilerplates
1. Do not hesitate to compare with some of our newer/bigger/famous modules.
1. Edit the `README.md` and other files (`include.sh` etc...) to fit your module. Note: the README is automatically created from `README_example.md` when you use the script `create_module.sh`.
1. Publish your module to our [catalogue](https://github.com/azerothcore/modules-catalogue).

