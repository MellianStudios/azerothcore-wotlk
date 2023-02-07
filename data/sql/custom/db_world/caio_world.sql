/*
SQLyog Community v13.2.0 (64 bit)
MySQL - 8.0.28
*********************************************************************
*/
/*!40101 SET NAMES utf8 */;

delete from `command` where `name` LIKE "%caio%";
insert into `command` (`name`, `security`, `help`) values ('caio','0','Syntax: .caio $subcommand\r\nType .caio to see the list of possible subcommands or .help caio $subcommand to see info on subcommands');
insert into `command` (`name`, `security`, `help`) values ('caio version','0','Syntax: .caio version\r\nShows the AIO version');
insert into `command` (`name`, `security`, `help`) values ('caio addaddon','3','Syntax: .caio addaddon $addonName \"$addonFile\"\r\nAdds an addon to addon list for players with $permission and force reloads all affected player addons');
insert into `command` (`name`, `security`, `help`) values ('caio removeaddon','3','Syntax: .caio removeaddon $addonName\r\nRemoves an addon from addon list and force reloads all affected player addons');
insert into `command` (`name`, `security`, `help`) values ('caio reloadaddons','3','Syntax: .caio reloadaddons\r\nReloads all client addons loaded on the server and forces reload on all players.');
insert into `command` (`name`, `security`, `help`) values ('caio forcereload','3','Syntax: .caio forcereload $playerName\r\nForce reloads player\\\'s addons. Player addons are synced with server.');
insert into `command` (`name`, `security`, `help`) values ('caio forcereset','3','Syntax: .caio forcereset $playerName\r\nForce resets player\\\'s addons. Player addons are deleted and downloaded again.');
insert into `command` (`name`, `security`, `help`) values ('caio forcereloadall','3','Syntax: .caio forcereloadall [$permission]\r\nForce reloads players of $permission and above. Affected players\\\' addons are synced with the server.');
insert into `command` (`name`, `security`, `help`) values ('caio forceresetall','3','Syntax: .caio forceresetall [$permission]\r\nForce resets players of $permission and above. Affected players\\\' addons are deleted and downloaded again.');
insert into `command` (`name`, `security`, `help`) values ('caio send','3','Syntax: .caio send $playerName \"Message\"\r\nSends an addon message to player');
insert into `command` (`name`, `security`, `help`) values ('caio sendall','3','Syntax: .caio sendall [$permission] \"Message\"\r\nSends an addon message to all players of $permission and above');

delete from `acore_string` where `entry` IN (60002, 60003, 60004, 60005, 60006);
insert into `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) values ('60002','Force reload message sent to %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
insert into `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) values ('60003','Force reset message sent to %s',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
insert into `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) values ('60004','There was a problem reloading client addons. Force reload was not sent.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
insert into `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) values ('60005','Addon with name \\\'%s\\\' already exists or file not found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
insert into `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) values ('60006','Addon with name \\\'%s\\\' not found.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
