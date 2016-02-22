<?php

function theme_render_template($template_file, $variables) {
  extract($variables, EXTR_SKIP);  // Extract the variables to a local namespace
  ob_start();                      // Start output buffering
  include "./display/templates/$template_file";      // Include the template file
  $contents = ob_get_contents();   // Get the contents of the buffer
  ob_end_clean();                  // End buffering and discard
  return $contents;                // Return the contents
}

function theme_list($list) {
  $content = '';
  if (!empty($list)) {
    foreach ($list as $item) {
      $content .= '<li>' . $item . '</li>';
    }
    $content = '<ul>' . $content . '</ul>';
  }
  return $content;
}

function theme_cell($board_cell) {
  return theme_render_template('cell.tpl.php', $board_cell);
}