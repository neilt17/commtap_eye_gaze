<?php

require_once 'display/includes.inc';
require_once 'display/show.inc';
require_once 'display/theme.inc';

conf_init();

$root_path = $_SERVER['DOCUMENT_ROOT'];
require_once $root_path . '/' . 'global-settings.php';

$global_settings = global_settings();
$global_settings['root_path'] = $root_path;

print show_page();
