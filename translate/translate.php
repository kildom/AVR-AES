<?php

function regRepl($m)
{
	global $regList;
	$regList[] = '_'.$m[2];
	return $m[1]."\"_".$m[2]."\"".$m[3];
}

function rxxRepl($m)
{
	$n = (int)$m[1];
	return "ST" . floor($n/4 + 1) . ($n%4 + 1);
}

function instrReplace($text)
{
	$text .= '  ';
	$text = preg_replace_callback("/r([0-9][0-9]?)/", 'rxxRepl', $text);
	$text = preg_replace("/[XYZ][LH]:([XYZ][LH])/", '\\1', $text);
	$text = preg_replace_callback("/([^0-9a-zA-Z_])([XYZ][LH]?)([^0-9a-zA-Z_])/", 'regRepl', $text);
	$text = preg_replace_callback("/([^0-9a-zA-Z_])(H[0-9])([^0-9a-zA-Z_])/", 'regRepl', $text);
	$text = preg_replace_callback("/([^0-9a-zA-Z_])(ST[1-4][1-4])([^0-9a-zA-Z_])/", 'regRepl', $text);
	$text = preg_replace_callback("/([^0-9a-zA-Z_])(I)([^0-9a-zA-Z_])/", 'regRepl', $text);
	$text = preg_replace_callback("/([^0-9a-zA-Z_])(Rcon)([^0-9a-zA-Z_])/", 'regRepl', $text);
	$text = preg_replace("/([^0-9a-zA-Z_])(PC)([^0-9a-zA-Z_])/", '\\1\\.\\3', $text);
	$text = preg_replace("/high\\(/i", 'hi8(', $text);
	$text = preg_replace("/low\\(/i", 'lo8(', $text);
	$text = preg_replace("/\\$([0-9a-f][0-9a-f][0-9a-f]?[0-9a-f]?)/i", '0x\\1', $text);
	$text = preg_replace("/(i?sbox[0-9a-f]?[0-9a-f]?)\\<\\<1/i", 'aes_tab_\\1', $text);
	$text = preg_replace("/(xtime)\\<\\<1/i", 'aes_tab_\\1', $text);
	return trim($text);
}

function translate($f)
{
	global $regList;
	$file = file($f);
	$out = '';
	$copy = false;
	foreach($file as $line) {
		if (trim(substr($line, 0, 8)) == ';BEGIN_C') {
			$name = trim(substr($line, 8));
			$out .= "\r\n//------------------------   ".$name."   ------------------------\\\\\r\n\tasm volatile (\r\n8zcn9Gez03vYXdjgNJuM";
			$copy = true;
			$regList = array();
			continue;
		}
		if (trim($line) == ';END_C') {
			$copy = false;
			$regList = array_unique($regList);
			sort($regList);
			$out .= "\t\t:\r\n\t\t:\r\n\t\t: \"memory\", ".implode(', ',$regList)."\r\n";
			$out .= "\t\t#undef ".implode("\r\n\t\t#undef ", $regList)."\r\n\t);\r\n\r\n";
			$out = str_replace('8zcn9Gez03vYXdjgNJuM', "\t\t#define ".implode(" \"\"\r\n\t\t#define ", $regList)." \"\"\r\n", $out);
			continue;
		}
		$pos = strpos($line, ';');
		if ($pos !== false) {
			$instr = trim(substr($line, 0, $pos));
			$com = "\t\t// ".trim(substr($line, $pos+1));
		} else {
			$instr = trim($line);
			$com = '';
		}
		if ($copy) {
//			$out .= $lab;
			if (strlen($instr)) {
				$out .= "\t\t\"".instrReplace($instr)."\t\t\\n\"$com\r\n";
			} else {
				if (strlen($com)) {
					$out .= "$com\r\n";
				}
			}
		}
	}
	file_put_contents("$f.c", $out);
}

$dir = opendir('.');
while ($f = readdir($dir)) {
	if (is_file($f) && strtolower(strrchr($f, '.')) == '.asm') translate($f);
}
closedir($dir);
