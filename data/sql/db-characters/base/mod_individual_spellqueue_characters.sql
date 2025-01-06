-- This table stores individual spell queue settings for characters
CREATE TABLE `individual_spellqueue` (
  `guid` INT NOT NULL,
  `window` INT NOT NULL DEFAULT 400,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
