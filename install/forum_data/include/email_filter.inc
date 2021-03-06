<?php
/**
* copyright            : (C) 2001-2010 Advanced Internet Designs Inc.
* email                : forum@prohost.org
* $Id$
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; version 2 of the License.
**/

function email_cache_rebuild()
{
	$data = '';
	$c = uq('SELECT email_block_opt, string FROM '. $GLOBALS['DBHOST_TBL_PREFIX'] .'email_block');
	while ($r = db_rowarr($c)) {
		$r[1] = addcslashes($r[1], '\'/\\');
		if ($r[0] != 'SIMPLE') {
			$data .= '\'/'. $r[1] .'/\' => '. $r[0] .',';
		} else {
			$data .= '\''. $r[1] .'\' => '. $r[0] .',';
		}
	}
	unset($c);
	if ($data) {
		$data = substr($data, 0, -1);
	}

	$fp = fopen($GLOBALS['FORUM_SETTINGS_PATH'] .'email_filter_cache', 'w');
	fwrite($fp, '<?php $GLOBALS[\'__FUD_EMAIL_FILTER__\'] = array('. $data .'); ?>');
	fclose($fp);
}

?>
