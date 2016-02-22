<!DOCTYPE html>
<html>
<head lang="en">
  <meta charset="UTF-8">
  <title>Communication Board</title>
  <link href="static/resources/board.css" rel="stylesheet">
  <!-- If using wih Drupal, say, ensure the page loads this version of jQuery
  rather than the usual one -->

</head>
<body>
<div id="board">
<?php print $content ?>
</div>
<canvas id="dwell-timer-canvas" width="2000" height="1000"></canvas>
<script src="static/resources/jquery-1.12.0.js"></script>
<script src="static/resources/board.js"></script>
<script type="text/javascript">
  <!--//--><![CDATA[//><!--
  jQuery.extend(App.settings, <?php print $aac_settings ?>);
  //--><!]]>
</script>
</body>
</html>