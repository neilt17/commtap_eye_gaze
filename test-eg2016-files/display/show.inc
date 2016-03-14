<?php

// put specific css and js settings on the page

/**
 *
 * TODO: enable printing - normal and etran frame;
 * TODO: etran frame printing: print for front and back - page numbers on
 * back, goto cells - display goto page number on back
 * TODO: etran - sizing, e.g. offer A2, A3, A4 or custom
 * TODO: etran - size of border where pictures are positioned/size of pictures
 * (as settings)
 * TODO: allow message to repeated if stay dwelling in cell - e.g. delay after
 * it speaks - configure on/off - won't do.
 * TODO: for each board set, can invite others to view and edit it - to a
 * maximum - perhaps 0 if no subscription
 * TODO: in editing - allow cells to be swapped - you choose then swap them
 * TODO: enable uploading of own symbols probably easiest/best...
 * TODO: in js - something is not resizing when browser resizes;
 *
 *
 *
 *
 *
 *
 */

function show_page() {
  if (empty($_GET['set'])) {
    $content = show_set_list();
  }
  else {
    if (empty($_GET['board'])) {
      $content = '<p>' . l('List of page sets', 'display.php') . '</p>';
      $content .= show_page_list($_GET['set']);
    }
    else {
      $content = show_board($_GET['set'], $_GET['board']);
    }
  }
  return $content;
}

function show_set_list() {
  global $global_settings;

  $boards_path = $global_settings['root_path'] . '/' . $global_settings['start_path'] . '/static/boards/';

  $sets = array_diff(scandir($boards_path), array('..', '.'));

  $links = array();
  foreach ($sets as $set) {
    $links[] = l($set, 'display.php', array('query' => array('set' => $set)));
  }

  $variables = array();
  $variables['content'] = theme_list($links);
  $variables['title'] = 'Sets list';

  return theme_render_template('navigation-page.tpl.php', $variables);

}

function show_page_list($set) {
  global $global_settings;

  $set_path = $global_settings['root_path'] . '/' . $global_settings['start_path'] . '/static/boards/' . $set;

  if (!is_dir($set_path)) {
    return 'PAGE NOT FOUND';
  }

  $boards = array_diff(scandir($set_path), array('..', '.', 'settings.txt'));

  $rows = array();
  foreach ($boards as $board) {
    $cell_counts = aac_cell_count($set, $board);
    $row = array();
    $row[] = l($board, 'display.php', array(
      'query' => array(
        'set' => $set,
        'board' => $board
      )
    ));
    $row[] = $cell_counts['rows'] . ' x ' . $cell_counts['cols'];
    $rows[] = $row;
  }

  $header = array('Page name', 'Rows x columns');

  $set_name_safe = check_plain($set);

  $variables = array();
  $variables['content'] = '<h2>Boards list in ' . $set_name_safe . '</h2>';
  $variables['content'] = '<div><a href="#set-settings">Jump to settings</a></div><p>&nbsp;</p>';
  $variables['content'] .= theme_table($header, $rows);

  $variables['content'] .= "<h2 id='set-settings'>Settings for $set_name_safe</h2>";
  $variables['content'] .= '<em>Note, settings are automatically saved when you enter them.</em>';
  $variables['content'] .= aac_settings_form($set);

  $variables['content'] .= '<script src="static/resources/jquery-1.12.0.js"></script>';
  $variables['content'] .= '<script src="static/resources/default_settings.js"></script>';
  $variables['content'] .= '<script type="text/javascript"><!--//--><![CDATA[//><!--';
  $variables['content'] .= "\njQuery.extend(App_settings.default_settings, {set: '$set'});";
  $variables['content'] .= "\n//--><!]]></script>";
  $variables['content'] .= '<script src="display/settings.js"></script>';

  $variables['title'] = check_plain($set);

  return theme_render_template('navigation-page.tpl.php', $variables);
}

function aac_settings_form($set) {

  $html_form = '<form id="localStorageSettings" method="post" action="">
<div class="form-element">
    <label>Dwell time (seconds):</label>
    <div><input type="number" name="dwellTimeSeconds" min="0" id="dwellTimeSeconds" class="stored" value="" /></div>
</div>
<div class="form-element">
    <label>Re-activation delay (seconds):</label>
    <div><input type="number" name="dwellReactivateSeconds" min="0" id="dwellReactivateSeconds" class="stored" value="" /></div>
</div>
<div class="form-element">
    <label>Access method:</label>
    <div><input type="radio" name="accessMethod" class="stored" value="gaze" />Gaze</div>
    <div><input type="radio" name="accessMethod" class="stored" value="click" />Click</div>
    <div><input type="radio" name="accessMethod" class="stored" value="click-and-gaze" />Click and gaze</div>
</div>
    <input type="submit" class="reset-button" value="Reset to defaults" />
</form>';
  return $html_form;


}

function show_board($set, $board) {

  // Get rows and columns
  // Directory must be of the form 1,1-

  global $global_settings;

  $board_local_path = 'static/boards/' . $set . '/' . $board;
  $board_path = $global_settings['root_path'] . $global_settings['start_path'] . '/' . $board_local_path;

  if (!is_dir($board_path)) {
    return 'PAGE NOT FOUND';
  }

  $cells = array_diff(scandir($board_path), array('..', '.'));

  $rows = 0;
  $cols = 0;

  $board_data = array();
  foreach ($cells as $cell) {
    $parts = explode('-', $cell);
    $row_col = explode(',', $parts[0]);
    $row = $row_col[0];
    $col = $row_col[1];
    if (is_numeric($row) && is_numeric($col)) {
      $rows = max($rows, $row);
      $cols = max($cols, $col);

      $data = array();
      if (file_exists($board_path . '/' . $cell . '/BLANK')) {
        $data['blank'] = TRUE;
      }
      else {
        $data['blank'] = FALSE;
      }
      // get the files in the directory:
      $cell_files = scandir($board_path . '/' . $cell);
      foreach ($cell_files as $cell_file) {
        $pathinfo = pathinfo($cell_file);
        switch ($pathinfo['filename']) {
          case 'audio':
          case 'sound':
            $data['audio'][] = url($board_local_path . '/' . $cell . '/' . $cell_file);
            break;
          case 'display':
            if (file_exists($board_path . '/' . $cell . '/' . $cell_file)) {
              $data['display'] = check_plain(file_get_contents($board_path . '/' . $cell . '/' . $cell_file));
            }
            else {
              $data['display'] = '';
            }
            break;
          case 'image':
            $data['image'] = url($board_local_path . '/' . $cell . '/' . $cell_file);
        }
      }
      $board_data['cells'][$row][$col] = $data;
    }
  }

  if (empty($board_data)) {
    return 'Nothing to display.';
  }

  $json = "{
    cellsAcross: $cols,
    cellsDown: $rows,
    set: '$set'
  }";

  $content = '';

  for ($i = 1; $i <= $rows; $i++) {
    for ($j = 1; $j <= $cols; $j++) {
      if (empty($board_data['cells'][$i][$j])) {
        $content .= theme_cell(array('blank' => TRUE));
      }
      else {
        $content .= theme_cell($board_data['cells'][$i][$j]);
      }
    }
    $content .= '<div class="clear"></div>';
  }

  $variables['content'] = $content;
  $variables['aac_settings'] = $json;
  return theme_render_template('board.tpl.php', $variables);


}

function aac_cell_count($set, $board) {

  // Get rows and columns
  // Directory must be of the form 1,1-
  global $global_settings;

  $board_local_path = 'static/boards/' . $set . '/' . $board;
  $board_path = $global_settings['root_path'] . $global_settings['start_path'] . '/' . $board_local_path;

  if (!is_dir($board_path)) {
    return FALSE;
  }

  $cells = array_diff(scandir($board_path), array('..', '.'));

  $rows = 0;
  $cols = 0;
  foreach ($cells as $cell) {
    $parts = explode('-', $cell);
    $row_col = explode(',', $parts[0]);
    $row = $row_col[0];
    $col = $row_col[1];
    if (is_numeric($row) && is_numeric($col)) {
      $rows = max($rows, $row);
      $cols = max($cols, $col);
    }
  }

  return array('rows' => $rows, 'cols' => $cols);

}
