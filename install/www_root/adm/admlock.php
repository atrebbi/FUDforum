<?php
/**
* copyright            : (C) 2001-2009 Advanced Internet Designs Inc.
* email                : forum@prohost.org
* $Id: admlock.php,v 1.51 2009/09/30 16:47:33 frank Exp $
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; version 2 of the License.
**/

	require ('./GLOBALS.php');
	fud_use('adm.inc', true);
	fud_use('glob.inc', true);

	require($WWW_ROOT_DISK . 'adm/header.php');

	if (isset($_POST['usr_passwd'], $_POST['usr_login']) && q_singleval("SELECT id FROM ".$DBHOST_TBL_PREFIX."users WHERE login="._esc($_POST['usr_login'])." AND passwd='".md5($_POST['usr_passwd'])."' AND (users_opt & 1048576) > 0")) {
		$FUD_OPT_2 |= 8388608;
		if (isset($_POST['btn_unlock'])) {
			$dirperms = 0777;
			$fileperms = 0666;
			@unlink($ERROR_PATH.'FILE_LOCK');
			$FUD_OPT_2 ^= 8388608;
		} else {
			if (!strncmp(PHP_SAPI, 'apache', 6)) {
				$dirperms = 0700;
				$fileperms = 0600;
			} else {
				$dirperms = 0711;
				$fileperms = 0644;
			}
		}

		$dirs = array(realpath($DATA_DIR));
		if ($WWW_ROOT_DISK != $DATA_DIR) {
			$dirs[] = realpath($WWW_ROOT_DISK);
		}

		$u = umask(0);

		while (list(,$v) = each($dirs)) {
			@chmod($v, $dirperms);
			if (!is_readable($v)) {
				echo 'ERROR: Unable to open "'.$v.'" directory<br />';
				continue;
			}
			if (!($files = glob($v . '/{.b*,.h*,.p*,.n*,.m*,*}', GLOB_BRACE|GLOB_NOSORT))) {
				continue;
			}
			foreach ($files as $path) {
				if (@is_file($path) && !@chmod($path, $fileperms)) {
					echo 'ERROR: couldn\'t chmod "'.$path.'"<br />';
				} else if (@is_dir($path) && !is_link($path)) {
					$dirs[] = $path;
				}
			}
		}
		umask($u);

		change_global_settings(array('FUD_OPT_2' => $FUD_OPT_2));
	}

	$status = ($FUD_OPT_2 & 8388608 ? 'LOCKED' : 'UNLOCKED');
?>
<h2>Lock/Unlock Forum Files</h2>
<?php if ($status == 'UNLOCKED' ) echo '<div class="alert">For security reasons, remember to lock your forum\'s files after you are done editing them.</div>'; ?>
<form method="post" action="">
<p>The forum's files appear to be: 
<?php echo '<font color="'. ($status=='LOCKED' ? 'green' : 'red') .'">'. $status .'</font>'; ?>
</p>
<table border="0" cellspacing="0" cellpadding="3">
<tr><td>Login:</td><td><input type="text" name="usr_login" value="<?php echo $usr->alias; ?>" /></td></tr>
<tr><td>Password:</td><td><input type="password" name="usr_passwd" /></td></tr>
<tr><td colspan="2" align="center">
	<input type="submit" name="btn_lock" value="Lock Files" />
	<input type="submit" name="btn_unlock" value="Unlock Files" />
</td></tr>
</table>
<br />
<div class="tutor">If this test claims that the forum is unlocked, but you still cannot modify your files click on the "Unlock Files" button.</div>
<?php echo _hs; ?>
</form>
<?php require($WWW_ROOT_DISK . 'adm/footer.php'); ?>
