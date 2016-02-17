<?php
include_once('php_functions.inc');
print '<!DOCTYPE html><html><head lang="en"><meta charset="UTF-8">';
print '<title></title>';
print '<link href="style.css" rel="stylesheet">';
print '</head>';
print '<body>';
print '<h1>Eye gaze using HTML and Javascript</h1>';
$explanation = file_get_contents('explanation.txt');
$navigation = file_get_contents('navigation.txt');
// This is not in any way safe for external use!
print '<div class="navigation">' . _filter_autop($navigation) . '</div>';
print '<div class="content">' . _filter_autop($explanation) . '</div>';
print '<div class="clear"></div>';
print '</body></html>';
