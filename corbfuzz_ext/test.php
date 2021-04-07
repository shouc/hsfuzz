<?php
$a = 1;
session_start();

// var_dump($_SESSION);
if ($a == 0){
    echo $_COOKIE["a"];
}

echo 2;