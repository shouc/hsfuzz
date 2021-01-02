<?php
error_reporting(E_ALL);

$dsn = "mysql:host=localhost;dbname=wordpress_dev";
$user = "root";
$passwd = "shou@123";
$dbh = new PDO($dsn, $user, $passwd);
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$stmt = $dbh->prepare("INSERT INTO wp_terms (name) VALUES (:name)");
$stmt->execute([':name' => 'David']);




