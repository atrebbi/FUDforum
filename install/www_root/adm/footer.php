<?php if (defined('shell_script') ) return; ?>
<br />
<div style="text-align:right;">
<script type="text/javascript">
/* <![CDATA[ */
var wikilink = document.getElementsByTagName('H2').item(0).firstChild.data;
if (wikilink.length > 0) {
	document.write('[ <a href="http://cvs.prohost.org/index.php/'+
	wikilink+
	'" title="Context sensitive help (FUDforum wiki)">Help</a> ]');
}
/* ]]> */
</script>
</div>
</td>
</tr>
</table>
</body>
</html>