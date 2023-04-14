<?php

$user = $_POST["first"]. " ".$_POST["last"]. " ". $_POST["img_profile"]. "\n";
$db = fopen("db_user.txt", "a");
fwrite($db, $user);
fclose($db);
header('Location: http://localhost:8082/users_app/', 302);
