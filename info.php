<html>
 <head>
  <title>Тестируем PHP</title>
 </head>
 <body>
 <?php
    for ($i = 0; $i < 10; $i++)
        echo '<p>Привет, мир!</p>';
    echo '<p>' + phpinfo() + '</p>';
 ?>
 </body>
</html>