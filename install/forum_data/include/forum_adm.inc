<?php
/**
* copyright            : (C) 2001-2011 Advanced Internet Designs Inc.
* email                : forum@prohost.org
* $Id$
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; version 2 of the License.
**/

class fud_forum
{
	var	$id, $cat_id, $name, $descr, $parent, $url_redirect, $post_passwd, $forum_icon, $forum_opt, $date_created,
		$view_order, $message_threshold, $max_attach_size, $max_file_attachments;

	function __construct()
	{
		foreach ($this as $k => $v) {
			if (isset($_POST['frm_'. $k])) {
				$this->{$k} = $_POST['frm_'. $k];
			}
		}
	}

	function add($pos)
	{
		$tbl =& $GLOBALS['DBHOST_TBL_PREFIX'];

		if ($pos == 'LAST') {
			$this->view_order = (int)q_singleval('SELECT MAX(view_order) FROM '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum WHERE cat_id='. $this->cat_id) + 1;
		} else {
			$this->view_order = 1;
			q('UPDATE '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum SET view_order=view_order+1 WHERE cat_id='. $this->cat_id);
		}

		$this->id = db_qid('INSERT INTO '. $tbl .'forum (
			cat_id,
			name,
			descr,
			parent,
			url_redirect,
			post_passwd,
			date_created,
			view_order,
			forum_icon,
			max_attach_size,
			max_file_attachments,
			message_threshold,
			forum_opt
		) VALUES (
			'. $this->cat_id .',
			'. _esc(htmlspecialchars($this->name)) .',
			'. ssn($this->descr) .',
			'. (int)$this->parent .',
			'. ssn($this->url_redirect) .',
			'. ssn($this->post_passwd) .',
			'. __request_timestamp__ .',
			'. $this->view_order .',
			'. ssn($this->forum_icon) .',
			'. (int)$this->max_attach_size .',
			'. (int)$this->max_file_attachments .',
			'. (int)$this->message_threshold .',
			'. (int)$this->forum_opt .'
		)');

		fud_use('dbadmin.inc', true);	// For drop_table() and create_table().

		/* Create Forum Lock (fl_) table. */
		$fl_tbl = $tbl .'fl_'. $this->id;
		frm_add_lock_tbl($fl_tbl);

		/* Create Thread View (tv_) table. */
		$tv_tbl = $tbl .'tv_'. $this->id;
		frm_add_view_tbl($tv_tbl);

		if (($gid = group_add($this->id, $this->name, 2))) {
			grp_rebuild_cache(array(0, 2147483647));
		}

		return $this->id;
	}

	function sync($id, $cat_id)
	{
		$tbl =& $GLOBALS['DBHOST_TBL_PREFIX'];

		$gid = q_singleval('SELECT id FROM '. $tbl .'groups WHERE forum_id='. $id);

		q('UPDATE '. $tbl .'forum SET
			cat_id='. $cat_id .',
			name='. _esc(htmlspecialchars($this->name)) .',
			descr='. ssn($this->descr) .',
			parent='. (int)$this->parent .',
			url_redirect='. ssn($this->url_redirect) .',
			post_passwd='. ssn($this->post_passwd) .',
			forum_icon='. ssn($this->forum_icon) .',
			max_attach_size='. (int)$this->max_attach_size .',
			max_file_attachments='. (int)$this->max_file_attachments .',
			message_threshold='. (int)$this->message_threshold .',
			forum_opt='. (int)$this->forum_opt .'
		WHERE id='. $id);

		q('UPDATE '. $tbl .'groups SET name='. _esc($this->name) .' WHERE id='. $gid);
	}
}

function create_cat_select($name, $def, $blocked)
{
	$ret = '';
	$c = uq('SELECT id, name FROM '. $GLOBALS['DBHOST_TBL_PREFIX'] .'cat WHERE id!='. $blocked .' ORDER BY view_order');
	while ($r = db_rowarr($c)) {
		$ret .= '<option value="'. $r[0] .'"'. ($r[0] == $def ? ' selected' : '') .'>'. $r[1] .'</option>';
	}
	unset($c);

	return ($ret ? '<select name="'. $name .'">'. $ret .'</select>' : '');
}

function frm_change_pos($oldp, $newp, $cat_id)
{
	if ($oldp == $newp || !$newp) {
		return;
	}

	db_lock($GLOBALS['DBHOST_TBL_PREFIX'] .'forum WRITE');
	q('UPDATE '.$GLOBALS['DBHOST_TBL_PREFIX'] .'forum SET view_order=2147483647 WHERE cat_id='. $cat_id .' AND view_order='. $oldp);
	if ($oldp < $newp) {
		q('UPDATE '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum SET view_order=view_order-1 WHERE cat_id='. $cat_id .' AND view_order<='. $newp .' AND view_order>'. $oldp);
		$maxp = q_singleval('SELECT MAX(view_order) FROM '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum WHERE cat_id='. $cat_id .' AND view_order!=2147483647');
		if ($newp > $maxp) {
			$newp = $maxp + 1;
		}
	} else {
		q('UPDATE '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum SET view_order=view_order+1 WHERE  cat_id='. $cat_id .' AND view_order<'. $oldp .' AND view_order>='. $newp);
	}
	q('UPDATE '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum SET view_order='. $newp .' WHERE cat_id='. $cat_id .' AND view_order=2147483647');
	db_unlock();
}

function frm_rebuild_order($cat_id)
{
	$i = 1;
	$c = q('SELECT id, view_order FROM '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum WHERE cat_id='. $cat_id .' ORDER BY view_order');
	while ($r = db_rowarr($c)) {
		if ($i != $r[1]) {
			q('UPDATE '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum SET view_order='. $i .' WHERE id='. $r[0]);
		}
		++$i;
	}
	unset($c);
}

function frm_move_forum($id, $dest_cat, $src_cat)
{
	q('UPDATE '. $GLOBALS['DBHOST_TBL_PREFIX'] .'forum SET cat_id='. $dest_cat .' WHERE id='. $id);
	if (db_affected()) {
		if ($src_cat) {
			frm_rebuild_order($src_cat);
		}
		if ($dest_cat) {
			frm_rebuild_order($dest_cat);
		}
		return 1;
	}
	return;
}

/* Create Forum Lock table for a forum (drop if exists, may be old format). */
function frm_add_lock_tbl($fl_tbl)
{
	fud_use('dbadmin.inc', true);	// For drop_table() and create_table().

	drop_table($fl_tbl, true);	// If exists.

	create_table('CREATE TABLE '. $fl_tbl .' (id INT)');
}

/* Create Thread View table for a forum (drop if exists, may be old format). */
function frm_add_view_tbl($tv_tbl)
{
	fud_use('dbadmin.inc', true);	// For drop_table() and create_table().

	drop_table($tv_tbl, true);	// If exists.

	foreach (explode(';', file_get_contents($GLOBALS['DATA_DIR'] .'/sql/fud_tv_1.tbl')) as $v) {
		if (!strncmp(($v = trim($v)), 'CREATE', 6)) {
			create_table(str_replace('{SQL_TABLE_PREFIX}tv_1', $tv_tbl, $v));
		}
	}
}

function frm_delete($id)
{
	fud_use('dbadmin.inc', true);	// For drop_table().
	$tbl =& $GLOBALS['DBHOST_TBL_PREFIX'];

	q('DELETE FROM '. $tbl .'ann_forums WHERE forum_id='. $id);
	q('DELETE FROM '. $tbl .'thread WHERE forum_id='. $id);
	q('DELETE FROM '. $tbl .'mod WHERE forum_id='. $id);
	q('DELETE FROM '. $tbl .'forum_notify WHERE forum_id='. $id);
	q('DELETE FROM '. $tbl .'group_cache WHERE resource_id='. $id);
	q('DELETE FROM '. $tbl .'thread WHERE forum_id='. $id);
	q('DELETE FROM '. $tbl .'mlist WHERE forum_id='. $id);
	q('DELETE FROM '. $tbl .'nntp WHERE forum_id='. $id);
	$gid = q_singleval('SELECT id FROM '. $tbl .'groups WHERE forum_id='. $id);
	drop_table($tbl .'fl_'. $id, true);
	drop_table($tbl .'tv_'. $id, true);
	q('DELETE FROM '. $tbl .'forum WHERE id='. $id);

	if ($gid) {
		fud_use('groups_adm.inc', true);
		group_delete($gid);
	}
}

?>
