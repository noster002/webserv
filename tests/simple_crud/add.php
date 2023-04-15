<?php
$uploaddir = 'uploads/';
$uploadfile = $uploaddir . $_POST['first'] . "_" . $_POST['last'] . "_" . basename($_FILES['img_profile']['name']);
move_uploaded_file($_FILES['img_profile']['tmp_name'], $uploadfile);
$user = $_POST["first"]. " ".$_POST["last"]. " ". $uploadfile."\n";
$db = fopen("db_user.txt", "a");
fwrite($db, $user);
fclose($db);
header('Location: http://localhost:8082/', 302);
