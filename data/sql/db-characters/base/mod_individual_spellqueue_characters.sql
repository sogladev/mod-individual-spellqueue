-- This table stores individual spell queue settings for characters
CREATE TABLE IF NOT EXISTS `mod_individual_spellqueue` (
  `guid` INT NOT NULL,
  `window` INT NOT NULL DEFAULT 400,
  `latency` TINYINT(1) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=UTF8MB3;
