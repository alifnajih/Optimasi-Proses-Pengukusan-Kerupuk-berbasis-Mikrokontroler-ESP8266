<?php
require 'config.php';

$pos_max = $_GET['pos_max'];
$sql = "UPDATE tbl_data SET level_max='$pos_max'";
$result = $db->query($sql);
echo $pos_max;
