<?php
/**
* copyright            : (C) 2001-2004 Advanced Internet Designs Inc.
* email                : forum@prohost.org
* $Id: admmysql.php,v 1.1 2005/11/28 17:55:08 hackie Exp $
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
**/
	require('./GLOBALS.php');
	fud_use('adm.inc', true);

	if (__dbtype__ != 'mysql') {
		exit("This control panel is intended for MySQL users only!");
	}

	// get a list of supported charsets
	$chars = db_all('SHOW CHARACTER SET');
	sort($chars);

	if (!empty($_POST['charset']) && in_array($_POST['charset'], $chars)) {
		foreach (get_fud_table_list() as $v) {
			q('ALTER TABLE '.$v.' CONVERT TO CHARACTER SET '.$_POST['charset']);
		}
	}

	require($WWW_ROOT_DISK . 'adm/admpanel.php');
?>
<h2>MySQL Table Character Set Adjuster</h2>
<form method="post" name="a_frm">
<?php echo _hs; ?>
<table class="datatable solidtable">
<tr class="field">
        <td>Available Character Sets</td>
	<td><select name="charset"><?php foreach ($chars as $v) { echo '<option value="'.$v.'">'.$v.'</option>'; } ?></select></td>
</tr>
<tr class="field">
	<td colspan=2 align=right><input tabindex="3" type="submit" value="Change Charset" name="btn_submit"></td>
</tr>
</table>
</form>
<script>
<!--
document.a_frm.subject.focus();
//-->
</script>
<?php require($WWW_ROOT_DISK . 'adm/admclose.html'); ?>