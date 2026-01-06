<?php

require 'config.php';
$sql = "SELECT * FROM tbl_data";
$result = mysqli_query($db, $sql);
$data = mysqli_fetch_array($result);
$min = $data['level_min'];
$max = $data['level_max'];

$sql1 = "SELECT * FROM tbl_temperature ORDER BY id DESC LIMIT 5";
$result1 = $db->query($sql1);
if (!$result1) { {
        echo "Error: " . $sql1 . "<br>" . $db->error;
    }
}

?>


<!doctype html>
<html lang="en">

<head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <script src="https://code.jquery.com/jquery-3.1.1.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.1/dist/umd/popper.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.6.1/dist/js/bootstrap.bundle.min.js"></script>
    <title>Dashboard</title>
    <!-- style -->
    <style type="text/css">
        .center {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            text-align: center;
            margin-top: 20px;
        }
    </style>
    <!-- end style -->
    <script type="text/javascript">
        function ubahposisi_min(value) {
            document.getElementById('posisi_min').innerHTML = value;
            var xmlHttp = new XMLHttpRequest();
            xmlHttp.onreadystatechange = function() {
                if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
                    document.getElementById('posisi_min').innerHTML = xmlHttp.responseText;
                }
            }
            xmlHttp.open("GET", "min.php?pos_min=" + value, true);
            xmlHttp.send();
        }

        function ubahposisi_max(value) {
            document.getElementById('posisi_max').innerHTML = value;
            var xmlHttp = new XMLHttpRequest();
            xmlHttp.onreadystatechange = function() {
                if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
                    document.getElementById('posisi_max').innerHTML = xmlHttp.responseText;
                }
            }
            xmlHttp.open("GET", "max.php?pos_max=" + value, true);
            xmlHttp.send();
        }
    </script>
</head>

<body>
    <div class="container">
        <div class="container-fluid center">
            <div style="text-align: center; margin-bottom: 20px;">
                <h4>OPTIMASI PROSES PENGUKUSAN KERUPUK dengan SISTEM <br>BERBASIS MICROKONTROLLER di PABRIK KERUPUK PLOSO</h4>
            </div>
            <div class="row">
                <div class="col">
                    <div class="card text-center" style="margin: auto;">
                        <div class="card-header" style="text-align: center; font-size: 20px;">Temperature</div>
                        <div class="card-body">
                            <!-- card temperature -->
                            <div id="chart_temperature"></div>
                            <!-- end card temperature -->
                        </div>
                    </div>
                </div>
                <div class="col">
                    <div class="card mb-5" style="max-width: 18rem;">
                        <div class="card-header">Set Poin Min</div>
                        <div class="card-body">
                            <!-- slider -->
                            <div style="text-align: center; font-size: 20px;">
                                <label for="customRange1" class="form-label"><span id="posisi_min"><?php echo $min; ?></span> °C</label>
                                <input type="range" class="form-range" id="customRange1" min="0" max="100" step="1" value="<?php echo $min; ?>" onchange="ubahposisi_min(this.value)">
                            </div>
                            <!-- end slider -->
                            <p class="card-text">Mengatur level suhu minimal</p>
                        </div>
                    </div>
                    <div class="card" style="max-width: 18rem;">
                        <div class="card-header">Set Poin Max</div>
                        <div class="card-body">
                            <!-- slider -->
                            <div style="text-align: center; font-size: 20px;">
                                <label for="customRange1" class="form-label"><span id="posisi_max"><?php echo $max; ?></span> °C</label>
                                <input type="range" class="form-range" id="customRange1" min="0" max="100" step="1" value="<?php echo $max; ?>" onchange="ubahposisi_max(this.value)">
                            </div>
                            <!-- end slider -->
                            <p class="card-text">Mengatur level suhu maksimal</p>
                        </div>
                    </div>
                </div>
                <div class="col">
                    <table class="table">
                        <thead>
                            <tr>
                                <th scope="col">No</th>
                                <th scope="col">Temperature</th>
                                <th scope="col">Date Time</th>
                            </tr>
                        </thead>
                        <tbody>
                            <?PHP $i = 1;
                            while ($row = mysqli_fetch_assoc($result1)) { ?>
                                <tr>
                                    <th scope="row"><?php echo $i++; ?></th>
                                    <td><?PHP echo $row['temperature']; ?></td>
                                    <td><?PHP echo date("Y-m-d h:i: A", strtotime($row['created_date'])); ?></td>
                                </tr>
                            <?PHP } ?>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>
    </div>

    <!-- script -->
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script>
        google.charts.load('current', {
            'packages': ['charts']
        });
        google.charts.setOnLoadCallback(drawTemperatureChart);

        function drawTemperatureChart() {
            //guage starting values
            var data = google.visualization.arrayToDataTable([
                ['Label', 'Value'],
                ['°C', 0],
            ]);
            var options = {
                width: 800,
                height: 300,
                redFrom: 70,
                redTo: 99,
                yellowFrom: 40,
                yellowTo: 70,
                greenFrom: 1,
                greenTo: 40,
                minorTicks: 5,
            };
            var chart = new google.visualization.Gauge(document.getElementById('chart_temperature'));
            chart.draw(data, options);

            function refreshData() {
                $.ajax({
                    url: 'getData.php',
                    data: 'q=' + $("#users").val(),
                    dataType: 'json',
                    success: function(responseText) {
                        var var_temperature = parseFloat(responseText.temperature).toFixed(2)
                        var data = google.visualization.arrayToDataTable([
                            ['Label', 'Value'],
                            ['°C', eval(var_temperature)],
                        ]);
                        chart.draw(data, options);
                    },
                    error: function(jqXHR, textStatus, errorThrown) {
                        console.log(errorThrown + ': ' + textStatus);
                    }
                });
            }
            setInterval(refreshData, 1000);
        }
        $(window).resize(function() {
            drawTemperatureChart();
        });
    </script>
    <!-- end script -->
</body>

</html>