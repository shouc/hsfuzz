<?php
$servername = 1;
$username = "username";
$password = "password";
$dbname = "myDB";
if (abs(0.4) > 0){
echo 1;}
// 创建连接
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("连接失败: " . $conn->connect_error);
}

$sql = "SELECT id, firstname, lastname FROM MyGuests where id=1 limit 1";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // 输出数据
    while($row = $result->fetch_assoc()) {
        if ($row["id"] == 1){
            echo 1;
        }
    }
} else {
    echo "0 结果";
}
$conn->close();