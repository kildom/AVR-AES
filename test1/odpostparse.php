<?php

//$argv[1] = 'input.od';

$f = file($argv[1]);
$out = '';
foreach ($f as $k=>$line) {
	$pos = stripos($line, 'lac');
	if ($pos !== false) {
		list($addr, $lo, $hi) = preg_split("/\\s/", $line, -1, PREG_SPLIT_NO_EMPTY);
		$line = substr_replace($line, ".word 0x$hi$lo\r\n", $pos);
	}
	$out .= $line;
}

file_put_contents($argv[1], $out);
