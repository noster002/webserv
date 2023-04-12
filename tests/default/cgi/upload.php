<?php
	$dir = "../uploads/";
	$file = $dir . $_FILES['file']['name'];
	$anotherfile = $dir . $_FILES['anotherfile']['name'];
	move_uploaded_file($_FILES['file']["tmp_name"], $file);
	move_uploaded_file($_FILES['anotherfile']["tmp_name"], $anotherfile);
	$text = $_POST['text'];
?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Index</title>
</head>
<body>
	<h1>PHP result</h1>
	<p>file: <a href="<?= $file?>">click here to see file</a></p>
	<p>anotherfile: <a href="<?= $anotherfile?>">click here to see anotherfile</a></p>
	<p>text: <?= $text?></p>
</body>
</html>