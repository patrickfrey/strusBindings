<?php
ob_start();
phpinfo(INFO_GENERAL);
foreach (explode("\n", ob_get_clean()) as $infoline)
{
	$val = [];
	if (preg_match( "/Loaded Configuration File => (.*)$/", $infoline, $val))
	{
		echo "$val[1]\n";
	}
}
?>
