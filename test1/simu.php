<?php

$template = '../aes.h';

$memoryFrom = array('aes.o');

function compile()
{
	system('"'.dirname(__FILE__).'\compile.bat"');
	clearstatcache();
}

function simu()
{
	if (!file_exists('input.txt')) {
		echo "Simulation error!!! No input.txt file.\n";
		return;
	}
	$f = file('input.txt');
	$spLo = -1;
	$spHi = -1;
	foreach ($f as $line) {
		$line = trim($line);
		$setsp = -1;
		if (substr($line, 0, 7) == '[SP] = ') {
			$setsp = (int)substr($line, 7);
		}
		if (substr($line, 0, 8) == '[SPL] = ') {
			$spLo = (int)substr($line, 8);
			if ($spHi >= 0) $setsp = $spLo | ($spHi << 8);
		}
		if (substr($line, 0, 8) == '[SPH] = ') {
			$spHi = (int)substr($line, 8);
			if ($spLo >= 0) $setsp = $spLo | ($spHi << 8);
		}
		if ($setsp >= 0) {
			//echo("SP: $setsp\n");
			stack($setsp);
		} else {
			list($node, $time, $name, $eq, $val) = preg_split("/\s/", $line, -1, PREG_SPLIT_NO_EMPTY);
			if ($time > 0 && substr($name, 0, 4) == 'PORT' && $eq == '<=') {
				$name = substr($name, 4);
				$time = (int)$time;
				$val = hexdec($val);
				//echo("AC: $name$val ($time)\n");
				action($name, $val, $time);
			}
		}
	}
}

function memory()
{
	global $memoryFrom;
	
	$f = file("input.map");

	if (!$f) {
		echo "Compilation error! No input.map file!\n";
		exit();
	}

	$flash = 0;
	$data = 0;

	$infoFor = array_combine($memoryFrom, $memoryFrom);

	foreach ($f as $line)
	{
		list($seg, $addr, $size, $file) = preg_split("/\s/", trim($line), -1, PREG_SPLIT_NO_EMPTY);
		$size = hexdec(ltrim($size, '0xX'));
		if ($infoFor[$file] && $size > 0) {
			$print = true;
			switch ($seg)
			{
				case '.text':
				case '.text.startup':
				case '.progmem.data':
					$flash += $size;
					break;
				case '.bss':
				case 'COMMON':
					$data += $size;
					break;
				case '.data':
					$flash += $size;
					$data += $size;
					break;
				default:
					if (substr($seg, 0, 6) != '.debug') {
						echo("Unknown segment: $seg\n");
					}
					$print = false;
					break;
			}
			if ($print) {
				//echo(str_pad($file, 20).str_pad($seg, 20).$size."\n");
			}
		}
	}

	global $flashSize, $dataSize;
	$flashSize = $flash;
	$dataSize = $data;

}


function parseList($keys, $values, $input)
{
	$n = count($input);
	if ($n >= count($keys)) {
		return array($input);
	} else {
		$ret = array();
		foreach ($values[$n] as $val) {
			$input[$n] = $val;
			$ret = array_merge($ret, parseList($keys, $values, $input));
		}
	}
	return $ret;
}


function readVectors()
{
	global $vectNames, $vectors;
	$f = file(dirname(__FILE__).'/vectors.txt');
	$vectNames = array();
	$vectors = array();
	foreach ($f as $line) {
		if (!strlen(trim($line)) || substr(trim($line), 0, 1) == '#') continue;
		$tab = preg_split("/\\s/", trim($line), -1, PREG_SPLIT_NO_EMPTY);
		if (in_array('|', $tab)) {
			foreach ($tab as $k=>$n) {
				if ($n != '|') $vectNames[$k] = $n;
			}
		} else {
			$vectors[] = $tab;
		}
	}
}


function parse()
{
	global $template, $vectNames, $templateText, $vectPaterns;
	$f = file($template);
	$out = '';
	$patern = "/#define\\s+(".implode('|', $vectNames).")\\s+.*/";
	foreach ($f as $line) {
		$line = preg_replace($patern, '#define \\1 AkGrSmoPk1\\1qpIhpP65V2', $line);
		$out .= rtrim($line)."\r\n";
	}
	foreach ($vectNames as $k=>$name) $vectPaterns[$k] = "AkGrSmoPk1{$name}qpIhpP65V2";
	$templateText = $out;
}

function save($values)
{
	global $template, $vectNames, $templateText, $vectPaterns;
	file_put_contents($template, str_replace($vectPaterns, $values, $templateText));
}

function stack($sp)
{
	global $minstack, $stack;
	$minstack = min($sp, $minstack);
	$stack = $sp;
}

function action($name, $val, $time)
{
	global $funcName, $propName, $state, $stats, $minstack, $stack, $timer, $info;
	switch ($name)
	{
	case 'A':
		switch ($val)
		{
		case 0:
			$info = array();
			$stats = array();
			$state = null;
			break;
		case 1:
			$timer = $time;
			$minstack = $stack;
			break;
		case 2:
			$stats[$funcName]['ticks'] += $time - $timer;
			$stats[$funcName]['stack'] = max($stats[$funcName]['stack'], $stack - $minstack);
			$stats[$funcName]['ok'] = '?';
			break;
		case 3:
			if ($stats[$funcName]['ok'] != 'ERR') $stats[$funcName]['ok'] = 'OK';
			break;
		case 4:
			$stats[$funcName]['ok'] = 'ERR';
			echo("ERR\n");
			break;
		case 5:
			$funcName = '';
			$state = 'funcName';
			break;
		case 6:
			$propName = '';
			$state = 'propName';
			break;
		case 7:
			$infoValue = 0;
			$state = 'funcValue';
			break;
		case 8:
			$funcValue = 0;
			$state = 'infoValue';
			break;
		}
		break;

	case 'B':
		switch ($state) 
		{
		case 'funcName':
			$funcName .= chr($val);
			break;
		case 'propName':
			$propName .= chr($val);
			break;
		case 'infoValue':
			$info[$propName] = ($info[$funcName] << 8) + $val;
			break;
		case 'funcValue':
			$stats[$funcName][$propName] = ($stats[$funcName][$propName] << 8) + $val;
			break;
		}
		break;
	}
}

readVectors();
parse();

$hash = array();

foreach ($vectors as $k=>$vect) {
	save($vect);
	compile();

	$md5 = @md5_file('input.hex');

	if (!$md5) $md5 = '00000000000000000000000000000000';

	if ($md5 == '00000000000000000000000000000000' && !$hash[$md5]) {

		$info['flash'] = 'ERR';
		$info['flashMax'] = 'ERR';
		$info['data'] = 'ERR';
		$info['ram'] = 'ERR';
		$info['user'] = 'ERR';
		$info['func'] = array();
		$info['vect'] = $vect;
		$hash[$md5] = $info;

	} elseif (!$hash[$md5]) {

		memory();
		simu();

		$info['flash'] = $flashSize;
		$info['flashMax'] = $flashSize + 255*$info['padding'];
		$info['data'] = $dataSize;
		$info['ram'] = 0;
		foreach ($stats as $s) $info['ram'] = max($info['ram'], $s['stack']);
		$info['ram'] += $info['data'] + $info['user'];
		$info['func'] = $stats;
		$info['vect'] = $vect;

		$hash[$md5] = $info;

	} else {

		$oval = $hash[$md5]['vect'];
		foreach ($oval as $k2=>$v) {
			$tab = explode('`', $v);
			array_push($tab, $vect[$k2]);
			$tab = array_unique($tab);
			$oval[$k2] = implode('`', $tab);
		}
		$hash[$md5]['vect'] = $oval;

	}
	echo("$md5    ".($k+1)."/".count($vectors)."\n");
}

$names = array();
foreach ($hash as $info) {
	$names = array_merge($names, array_keys($info['func']));
	$names = array_unique($names);
}

echo "\n\n";

$nl = '';
foreach ($vectNames as $name) {
	echo "\t";
	$nl .= "$name\t";
}
echo "\t\t\t\t\t";
$nl .= "flash\tflashMax\tuser\tdata\tram\t";
foreach ($names as $name) {
	echo $name."\t\t";
	$nl .= "ticks\tstack\t";
}
echo "\n$nl\n";

foreach ($hash as $info) {
	foreach ($vectNames as $k=>$name) {
		echo $info['vect'][$k]."\t";
	}
	echo $info['flash']."\t";
	echo $info['flashMax']."\t";
	echo $info['user']."\t";
	echo $info['data']."\t";
	echo $info['ram']."\t";
	foreach ($names as $name) {
		if ($info['func'][$name]['ok'] != 'ERR') {
			echo $info['func'][$name]['ticks']."\t";
			echo $info['func'][$name]['stack']."\t";
		} else {
			echo $info['func'][$name]['ok']."\t";
			echo $info['func'][$name]['ok']."\t";
		}
	}
	echo "\n";
}

//print_r($hash);

echo "\n\n";
