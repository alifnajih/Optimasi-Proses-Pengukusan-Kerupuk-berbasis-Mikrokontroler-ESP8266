<?php
require 'config.php';

$pos_min = $_GET['pos_min'];
$sql = "UPDATE tbl_data SET level_min='$pos_min'";
$result = $db->query($sql);
echo $pos_min;
