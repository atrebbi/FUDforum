<?php
/***************************************************************************
* copyright            : (C) 2001-2004 Advanced Internet Designs Inc.
* email                : forum@prohost.org
* $Id: return.inc.t,v 1.20 2004/01/04 16:38:27 hackie Exp $
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
***************************************************************************/

function check_return($returnto)
{
	if ($GLOBALS['FUD_OPT_2'] & 32768 && !empty($_SERVER['PATH_INFO'])) {
		if (!$returnto || !strncmp($returnto, '/er/', 4)) {
			header('Location: {ROOT}/i/'._rsidl);
		} else {
			/* unusual situation, path_info & normal themes are active */
			if ($returnto[0] == '/') {
				header('Location: {ROOT}'.$returnto);
			} else {
				header('Location: {ROOT}?'.$returnto);
			}
		}
	} else {
		if (!$returnto || !strncmp($returnto, 't=error', 7)) {
			header('Location: {ROOT}?t=index&'._rsidl);
		} else {
			if (strpos($returnto, 'S=') === false && $GLOBALS['FUD_OPT_1'] & 128) {
				header('Location: {ROOT}?'.$returnto.'&S='.s);
			} else {
				header('Location: {ROOT}?'.$returnto);
			}
		}
	}
	exit;
}
?>