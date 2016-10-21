<?php
header('Content-Type: application/json');

$data = $_POST['data'] . "\n#\n#";
$descriptorspec = array(
   0 => array("pipe", "r"),  // stdin is a pipe that the child will read from
   1 => array("pipe", "w"),  // stdout is a pipe that the child will write to
);

if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
	$process = proc_open('LR', $descriptorspec, $pipes);
else
	$process = proc_open('./LR', $descriptorspec, $pipes);

if(is_resource($process)) {

	fwrite($pipes[0], $data);
	fclose($pipes[0]);

	echo stream_get_contents($pipes[1]);
	fclose($pipes[1]);
	proc_close($process);
}


