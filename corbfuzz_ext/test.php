<?php
$servername = "127.0.0.1";
$username = "username";
$password = "password";
$dbname = "myDB";

// 创建连接
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("连接失败: " . $conn->connect_error);
}

$sql = "SELECT id, firstname, lastname FROM MyGuests where id=1 limit 1";
$result = $conn->query($sql);

    // 输出数据
while($row = $result->fetch_assoc()) {
if ($row["x"] > 0){
            echo "id: " . $row["id"]. " - Name: " . $row["firstname"]. " " . $row["lastname"]. "<br>";
}
}
$conn->close();