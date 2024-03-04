<?php
require_once('./Antares.php');

Antares::init([
  "PLATFORM_URL" => 'https://platform.antares.id:8443', // TODO: Change this to your platform URL
  "ACCESS_KEY" => '419e64e6bd5c4df9:8ec386d8490c70cf' // TODO: Change this to your access key
]);

// if (condition) {
  
// }
?>
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Crossbar</title>
  <link rel="stylesheet" href="style.css?v2">
</head>
<body>
  <div class="wadah">
  <div class="table">
      <h1><?php
          try {
            // RETRIEVE DATA
            echo "Retrieve data";
          ?>
      </h1>
      <h4><?php 
          $resp = Antares::getInstance()->get('/antares-cse/antares-id/SmartCrossbar'); // TODO: Change this to your application uri
          if ($resp instanceof AE) {
            echo nl2br($resp->getName() . "\n");
        ?></h4>
    <div class="text">
        <h3>
        <?php 
          // list all application's devices
    $cntUris = $resp->listContainerUris();
    //echo "Containers: " . count($cntUris) . "\n";
    //echo "<br>";
    
    foreach ($cntUris as $cntUri) {
      //echo "  " . $cntUri . "\n";
      //echo "<br>";

      // get device
      $cnt = Antares::getInstance()->get($cntUri);
      $name = $cnt->getName();
      echo "      " . $cnt->getName() . "\n";
      echo "<br>";

      try {
        // get latest data
        $la = $cnt->getLatestContentInstace();
      // var_dump($la);
      $status = $la->con;
      $tulis = preg_replace("/[^a-zA-Z:  0-9 .]/", "", $status);
        echo "<p>$tulis</p>";
        echo "<br>";
      } catch (Exception $e) {
        echo "      last data: " . $e->getMessage() . "\n";
        echo "<br>";
      }
    }
  }
}catch (Exception $e) {
  echo $e->getMessage();
}
        ?>
        </h3>
    </div>
    <!-- <div class="button">
      <h2>Control Palang</h2> 
      <div class="tombol">
        <a href="#"><button >Buka</button></a>
        <a href="#"><button >Tutup</button></a>

      </div>
        </div> -->
  </div>
  </div>
  
</body>
</html>






