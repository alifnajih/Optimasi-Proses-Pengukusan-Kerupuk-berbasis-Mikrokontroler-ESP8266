<?php

require 'config.php';
$sql = "SELECT * FROM tbl_data";
$result = mysqli_query($db, $sql);
$data = mysqli_fetch_array($result);
$max = $data['level_max'];
echo $max;
