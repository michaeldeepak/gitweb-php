<?php
/** @file
 * Configuration file for viewgit.
 *
 */
$conf['projects'] = array(
	'UniteZone' => array('repo' => '/home/unitezone'),
);

// Where git is. Default is to search from PATH, but you can use an absolute
// path as well.
$conf['git'] = '/usr/bin/git';

// If set, contains an array of globs/wildcards where to include projects.
// Use this if you have a lot of projects under a directory.
//$conf['projects_glob'] = array('/path/to/*/.git', '/var/git/*.git');

$conf['datetime'] = '%Y-%m-%d %H:%M';

// More complete format for commit page
$conf['datetime_full'] = '%Y-%m-%d %H:%M:%S';

$conf['debug'] = TRUE;

$conf['servers'] = array( 'qa1' => 'qa.unitezone.om');
