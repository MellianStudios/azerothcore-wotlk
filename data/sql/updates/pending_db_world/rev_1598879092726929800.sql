INSERT INTO `version_db_world` (`sql_rev`) VALUES ('1598879092726929800');
/*
 * General: Build Update
 * Update by Knindza | <www.azerothcore.org>
 * Copyright (C) <www.shadowburn.net> & <www.lichbane.com>
*/

/* Content 3.1.x */ 
SET @Build := 9767;

UPDATE `quest_template` SET `VerifiedBuild` = @Build WHERE `Id` IN (13864, 13835, 13829, 13828, 13812, 13809, 13788, 13785, 13784, 13783, 13780, 13779, 13778, 13775, 13774, 13837, 13838, 13863, 13862, 13861, 13860, 13859, 13858, 13857, 13856, 13855, 13854, 13852, 13851, 13847, 13839, 13773, 13770, 13600, 13682, 13675, 13674, 13673, 13670, 13669, 13666, 13664, 13663, 13654, 13643, 13641, 13616, 13603, 13741, 13742, 13769, 13768, 13764, 13763, 13762, 13759, 13758, 13757, 13754, 13743, 13746, 13747, 13748, 13752, 13753);
