<?php
/***************************************************************************
* copyright            : (C) 2001-2004 Advanced Internet Designs Inc.
* email                : forum@prohost.org
* $Id: tmp_view.php.t,v 1.7 2004/01/04 16:38:27 hackie Exp $
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
***************************************************************************/

	if (!empty($_GET['img'])) {
		$file = $TMP . basename($_GET['img']);
	} else {
		$file = $WWW_ROOT_DISK . 'blank.gif';
	}

	if (!@file_exists($file) || !($im = @getimagesize($file))) {
		$file = $WWW_ROOT_DISK . 'blank.gif';
		$im = getimagesize($file);
	}

	header('Content-type: '.$im['mime']);
	fpassthru(fopen($file, 'rb'));
?>