<?php

/**
 * @file
 * SystematicReview module file component.
 */

/**
 * Implements _systematicreview_defaults_component().
 */
function _systematicreview_defaults_file() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'mandatory' => 0,
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
      'filtering' => array(
        'types' => array('gif', 'jpg', 'png'),
        'addextensions' => '',
        'size' => 800,
      ),
      'savelocation' => '',
      'width' => '',
      'title_display' => 0,
      'description' => '',
      'attributes' => array(),
    ),
  );
}

/**
 * Implements _systematicreview_theme_component().
 */
function _systematicreview_theme_file() {
  return array(
    'systematicreview_edit_file' => array(
      'render element' => 'form',
      'file' => 'components/file.inc',
    ),
    'systematicreview_render_file' => array(
      'render element' => 'element',
      'file' => 'components/file.inc',
    ),
    'systematicreview_display_file' => array(
      'render element' => 'element',
      'file' => 'components/file.inc',
    ),
  );
}

/**
 * Implements _systematicreview_edit_component().
 */
function _systematicreview_edit_file($component) {
    $form = array();
    $form['#theme'] = 'systematicreview_edit_file';

    $form['validation']['filtering'] = array(
      '#element_validate' => array('_systematicreview_edit_file_filtering_validate'),
      '#parents' => array('extra', 'filtering'),
    );

    // Find the list of all currently valid extensions.
    $current_types = isset($component['extra']['filtering']['types']) ? $component['extra']['filtering']['types'] : array();

    $types = array('gif', 'jpg', 'png');
    $form['validation']['filtering']['types']['webimages'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Web Images'),
      '#options' => drupal_map_assoc($types),
      '#default_value' => array_intersect($current_types, $types),
    );

    $types = array('bmp', 'eps', 'tif', 'pict', 'psd');
    $form['validation']['filtering']['types']['desktopimages'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Desktop Images'),
      '#options' => drupal_map_assoc($types),
      '#default_value' => array_intersect($current_types, $types),
    );

    $types = array('txt', 'rtf', 'html', 'odf', 'pdf', 'doc', 'docx', 'ppt', 'pptx', 'xls', 'xlsx', 'xml');
    $form['validation']['filtering']['types']['documents'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Documents'),
      '#options' => drupal_map_assoc($types),
      '#default_value' => array_intersect($current_types, $types),
    );

    $types = array('avi', 'mov', 'mp3', 'ogg', 'wav');
    $form['validation']['filtering']['types']['media'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Media'),
      '#options' => drupal_map_assoc($types),
      '#default_value' => array_intersect($current_types, $types),
    );

    $types = array('bz2', 'dmg', 'gz', 'jar', 'rar', 'sit', 'tar', 'zip');
    $form['validation']['filtering']['types']['archives'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Archives'),
      '#options' => drupal_map_assoc($types),
      '#default_value' => array_intersect($current_types, $types),
    );

    $form['validation']['filtering']['addextensions'] = array(
      '#type' => 'textfield',
      '#title' => t('Additional Extensions'),
      '#default_value' => $component['extra']['filtering']['addextensions'],
      '#description' => t('Enter a list of additional file extensions for this upload field, seperated by commas.<br /> Entered extensions will be appended to checked items above.'),
      '#size' => 60,
      '#weight' => 3,
      '#default_value' => $component['extra']['filtering']['addextensions'],
    );

    $form['validation']['filtering']['size'] = array(
      '#type' => 'textfield',
      '#title' => t('Max Upload Size'),
      '#default_value' => $component['extra']['filtering']['size'],
      '#description' => t('Enter the max file size a user may upload (in KB).'),
      '#size' => 10,
      '#weight' => 3,
      '#field_suffix' => t('KB'),
      '#default_value' => $component['extra']['filtering']['size'],
      '#parents' => array('extra', 'filtering', 'size'),
      '#element_validate' => array('_systematicreview_edit_file_size_validate'),
    );
    $form['extra']['savelocation'] = array(
      '#type' => 'textfield',
      '#title' => t('Upload Directory'),
      '#default_value' => $component['extra']['savelocation'],
      '#description' => t('SystematicReview uploads are always saved in the site files directory. You may optionally specify a subfolder to store your files.'),
      '#weight' => 3,
      '#field_prefix' => file_stream_wrapper_get_instance_by_scheme(file_default_scheme())->getDirectoryPath() . '/systematicreview/',
      '#element_validate' => array('_systematicreview_edit_file_check_directory'),
      '#after_build' => array('_systematicreview_edit_file_check_directory'),
    );
    $form['display']['width'] = array(
      '#type' => 'textfield',
      '#title' => t('Width'),
      '#default_value' => $component['extra']['width'],
      '#description' => t('Width of the file field.') . ' ' . t('Leaving blank will use the default size.'),
      '#size' => 5,
      '#maxlength' => 10,
      '#weight' => 4,
      '#parents' => array('extra', 'width')
    );
    return $form;
}

/**
 * A Form API element validate function to check filesize is numeric.
 */
function _systematicreview_edit_file_size_validate($element) {
  if (!empty($element['#value'])) {
    if (!is_numeric($element['#value']) || intval($element['#value']) != $element['#value']) {
      form_error($element, t('Max upload size must be a number in KB.'));
    }
  }
}

/**
 * A Form API after build and validate function.
 *
 * Ensure that the destination directory exists and is writable.
 */
function _systematicreview_edit_file_check_directory($element) {
  $base_dir = file_build_uri('systematicreview');
  $destination_dir = file_build_uri('systematicreview/' . $element['#value']);

  // Sanity check input to prevent use parent (../) directories.
  $real_base = drupal_realpath($base_dir);
  $real_destination = drupal_realpath($destination_dir);
  if (strpos($real_destination, $real_base) !== 0) {
    form_error($element, t('The save directory %directory is not valid.', array('%directory' => $destination_dir)));
  }
  else {
    $destination_success = file_prepare_directory($destination_dir, FILE_CREATE_DIRECTORY);
    if (!$destination_success) {
      form_error($element, t('The save directory %directory could not be created. Check that the systematicreview files directory is writtable.', array('%directory' => $destination_dir)));
    }
  }

  return $element;
}

/**
 * A Form API element validate function.
 *
 * Change the submitted values of the component so that all filtering extensions
 * are saved as a single array.
 */
function _systematicreview_edit_file_filtering_validate($element, &$form_state) {
  // Predefined types.
  $extensions = array();
  foreach (element_children($element['types']) as $category) {
    foreach (array_keys($element['types'][$category]['#value']) as $extension) {
      if ($element['types'][$category][$extension]['#value']) {
        $extensions[] = $extension;
      }
    }
  }

  // Additional types.
  $additional_extensions = explode(',', $element['addextensions']['#value']);
  foreach ($additional_extensions as $extension) {
    $clean_extension = drupal_strtolower(trim($extension));
    if (!empty($clean_extension) && !in_array($clean_extension, $extensions)) {
      $extensions[] = $clean_extension;
    }
  }

  form_set_value($element['types'], $extensions, $form_state);
}

function theme_systematicreview_edit_file($variables) {
  $form = $variables['form'];

  // Add a little JavaScript to check all the items in one type.
  $javascript = '
    function check_category_boxes() {
      var checkValue = !document.getElementById("edit-extra-filtering-types-"+arguments[0]+"-"+arguments[1]).checked;
      for(var i=1; i < arguments.length; i++) {
        document.getElementById("edit-extra-filtering-types-"+arguments[0]+"-"+arguments[i]).checked = checkValue;
      }
    }
 ';
  drupal_add_js($javascript, 'inline');

  // Format the components into a table.
  $per_row = 6;
  $rows = array();
  foreach (element_children($form['validation']['filtering']['types']) as $key => $filtergroup) {
    $row = array();
    $first_row = count($rows);
    if ($form['validation']['filtering']['types'][$filtergroup]['#type'] == 'checkboxes') {
      // Add the title.
      $row[] = $form['validation']['filtering']['types'][$filtergroup]['#title'];
      $row[] = '&nbsp;';
      // Convert the checkboxes into individual form-items.
      $checkboxes = form_process_checkboxes($form['validation']['filtering']['types'][$filtergroup]);
      // Render the checkboxes in two rows.
      $checkcount = 0;
      $jsboxes = '';
      foreach (element_children($checkboxes) as $key) {
        $checkbox = $checkboxes[$key];
        if ($checkbox['#type'] == 'checkbox') {
          $checkcount++;
          $jsboxes .= "'" . $checkbox['#return_value'] . "',";
          if ($checkcount <= $per_row) {
            $row[] = array('data' => drupal_render($checkbox));
          }
          elseif ($checkcount == $per_row + 1) {
            $rows[] = array('data' => $row, 'style' => 'border-bottom: none;');
            $row = array(array('data' => '&nbsp;'), array('data' => '&nbsp;'));
            $row[] = array('data' => drupal_render($checkbox));
          }
          else {
            $row[] = array('data' => drupal_render($checkbox));
          }
        }
      }
      // Pretty up the table a little bit.
      $current_cell = $checkcount % $per_row;
      if ($current_cell > 0) {
        $colspan = $per_row - $current_cell   + 1;
        $row[$current_cell + 1]['colspan'] = $colspan;
      }
      // Add the javascript links.
      $jsboxes = drupal_substr($jsboxes, 0, drupal_strlen($jsboxes) - 1);
      $rows[] = array('data' => $row);
      $select_link = ' <a href="javascript:check_category_boxes(\'' . $filtergroup . '\',' . $jsboxes . ')">(select)</a>';
      $rows[$first_row]['data'][1] = array('data' => $select_link, 'width' => 40);
      unset($form['validation']['filtering']['types'][$filtergroup]);
    }
    elseif ($filtergroup != 'size') {
      // Add other fields to the table (ie. additional extensions).
      $row[] = $form['validation']['filtering']['types'][$filtergroup]['#title'];
      unset($form['validation']['filtering']['types'][$filtergroup]['#title']);
      $row[] = array(
        'data' => drupal_render($form['validation']['filtering']['types'][$filtergroup]),
        'colspan' => $per_row + 1,
      );
      unset($form['validation']['filtering']['types'][$filtergroup]);
      $rows[] = array('data' => $row);
    }
  }
  $header = array(array('data' => t('Category'), 'colspan' => '2'), array('data' => t('Types'), 'colspan' => $per_row));

  // Create the table inside the form.
  $form['validation']['filtering']['types']['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows
  );

  $output = drupal_render_children($form);

  return $output;
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_render_file($component, $value = NULL, $filter = TRUE) {
  // Normally every file component is given a unique ID based on its key.
  if (isset($component['nid'])) {
    $node = node_load($component['nid']);
    $form_key = implode('_', systematicreview_component_parent_keys($node, $component));
  }
  // If being used as a default though, we don't yet have a form key.
  else {
    $form_key = 'default';
  }

  $element[$form_key] = array(
    '#type' => 'file',
    //'#required' => $component['mandatory'], // Drupal core bug with required file uploads.
    '#attributes' => $component['extra']['attributes'],
    '#tree' => FALSE, // file_check_upload assumes a flat $_FILES structure.
    '#element_validate' => array(
      '_systematicreview_validate_file',
      '_systematicreview_required_file',  // Custom required routine.
    ),
    '#systematicreview_component' => $component,
    '#theme_wrappers' => array(),
  );
  $element['#systematicreview_component'] = $component;
  $element['#weight'] = $component['weight'];
  $element['#title'] = $filter ? _systematicreview_filter_xss($component['name']) : $component['name'];
  $element['#title_display'] = $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before';
  $element['#description'] = $filter ? _systematicreview_filter_descriptions($component['extra']['description']) : $component['extra']['description'];

  $element['#theme'] = 'systematicreview_render_file';
  $element['#theme_wrappers'] = array('systematicreview_element');

  // Change the 'width' option to the correct 'size' option.
  if ($component['extra']['width'] > 0) {
    $element[$form_key]['#size'] = $component['extra']['width'];
  }

  // Handles the asterisk for mandatory fields. Note that this is only on the
  // wrapper, not on the file field itself. We handle validation separately.
  if ($component['mandatory']) {
    $element['#required'] = TRUE;
  }

  // Add a hidden element to store the FID for new files.
  $element['_fid'] = array(
    '#type' => 'hidden',
    '#default_value' => '',
  );

  // Add a hidden element to store the FID for existing files.
  $element['_old'] = array(
    '#type' => 'hidden',
    '#value' => isset($value[0]) ? $value[0] : NULL,
  );

  return $element;
}

/**
 * Render a File component.
 */
function theme_systematicreview_render_file($variables) {
  $element = $variables['element'];

  // Add information about the existing file, if any.
  if (isset($element['#default_value'])) {
    $element['_fid']['#value'] = $element['#default_value'];
  }
  $value = $element['_fid']['#value'] ? $element['_fid']['#value'] : $element['_old']['#value'];

  if ($value && ($file = systematicreview_get_file($value))) {
    $firstchild = array_shift(array_keys($element));
    $element[$firstchild]['#suffix'] = ' ' . l(t('Download @filename', array('@filename' => systematicreview_file_name($file->uri))), systematicreview_file_url($file->uri)) . (isset($element['#suffix']) ? $element['#suffix'] : '');
    $element[$firstchild]['#description'] = '<div class="systematicreview-newfile-message">' . t('Uploading a new file will replace the current file.') . '</div>' . (isset($element[$firstchild]['#description']) ? $element[$firstchild]['#description'] : '');
  }

  return drupal_render_children($element);
}

/**
 * A Form API element validate function.
 *
 * Fix Drupal core's handling of required file fields.
 */
function _systematicreview_required_file($element, $form_state) {
  $component = $element['#systematicreview_component'];
  $parents = $element['#array_parents'];
  array_pop($parents);
  $form_key = implode('_', $parents);

  // Do not validate requiredness on back or draft button.
  if (isset($form_state['clicked_button']['#validate']) && empty($form_state['clicked_button']['#validate'])) {
    return;
  }

  // Check if a value is already set in the hidden field.
  $values = $form_state['values'];
  $key = array_shift($parents);
  $found = FALSE;
  while (isset($values[$key])) {
    if (isset($values[$key])) {
      $values = $values[$key];
      $found = TRUE;
    }
    else {
      $found = FALSE;
    }
    $key = array_shift($parents);
  }

  if (!$found || (empty($values['_fid']) && empty($values['_old']))) {
    if (empty($_FILES['files']['name'][$form_key]) && $component['mandatory']) {
      form_error($element, t('%field field is required.', array('%field' => $component['name'])));
    }
  }
}

/**
 * A Form API element validate function.
 *
 * Ensure that the uploaded file matches the specified file types.
 */
function _systematicreview_validate_file($element, &$form_state) {
  $component = $element['#systematicreview_component'];
  $form_key = implode('_', $element['#parents']);

  if (empty($_FILES['files']['name'][$form_key]) || form_get_error($element)) {
    return;
  }

  // Build a human readable list of extensions:
  $extensions = $component['extra']['filtering']['types'];
  $extension_list = '';
  if (count($extensions) > 1) {
    for ($n = 0; $n < count($extensions) - 1; $n++) {
      $extension_list .= $extensions[$n] . ', ';
    }
    $extension_list .= 'or ' . $extensions[count($extensions) - 1];
  }
  elseif (!empty($extensions)) {
    $extension_list = $extensions[0];
  }

  if (in_array('jpg', $extensions)) {
    $extensions[] = 'jpeg';
  }

  $strrpos = function_exists('mb_strrpos') ? 'mb_strrpos' : 'strrpos';
  $dot = $strrpos($_FILES['files']['name'][$form_key], '.');
  $extension = drupal_strtolower(drupal_substr($_FILES['files']['name'][$form_key], $dot+1));
  $file_error = FALSE;
  if (!empty($extensions) && !in_array($extension, $extensions)) {
    form_error($element, t("Files with the '%ext' extension are not allowed, please upload a file with a %exts extension.", array('%ext' => $extension, '%exts' => $extension_list)));
    $file_error = TRUE;
  }

  // Now let's check the file size (limit is set in KB).
  if ($_FILES['files']['size'][$form_key] > $component['extra']['filtering']['size'] * 1024) {
    form_error($element, t("The file '%filename' is too large (%filesize KB). Please upload a file %maxsize KB or smaller.", array('%filename' => $_FILES['files']['name'][$form_key], '%filesize' => (int) ($_FILES['files']['size'][$form_key]/1024), '%maxsize' => $component['extra']['filtering']['size'])));
    $file_error = TRUE;
  }

  // Save the file to a temporary location.
  if (!$file_error) {
    $upload_dir = file_build_uri('systematicreview/' . $component['extra']['savelocation']);
    if (file_prepare_directory($upload_dir, FILE_CREATE_DIRECTORY)) {
      // Note that the $extensions list was already validated above but needs to
      // be passed into file_save_upload() for it to pass internal validation
      // and not use the default extension checklist.
      $file = file_save_upload($form_key, array('file_validate_extensions' => array(implode(' ', $extensions))), $upload_dir);
      if ($file) {
        // Set the hidden field value.
        $parents = $element['#array_parents'];
        array_pop($parents);
        $parents[] = '_fid';
        form_set_value(array('#parents' => $parents), $file->fid, $form_state);
      }
      else {
        drupal_set_message(t('The uploaded file %filename was unable to be saved. The destination directory may not be writable.', array('%filename' => $file->filename)), 'error');
      }
    }
    else {
      drupal_set_message(t('The uploaded file was unable to be saved. The destination directory does not exist.'), 'error');
    }
  }
}

/**
 * Implements _systematicreview_submit_component().
 */
function _systematicreview_submit_file($component, $value) {

  if ($value['_fid'] && ($file = systematicreview_get_file($value['_fid']))) {
    // Save any new files permanently.
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);

    // Delete any previous files.
    if ($value['_old'] && ($existing = systematicreview_get_file($value['_old']))) {
      file_delete($existing);
    }

    $value = $file->fid;
  }
  else {
    $value = $value['_old'] ? $value['_old'] : NULL;
  }

  return $value;
}

/**
 * Implements _systematicreview_display_component().
 */
function _systematicreview_display_file($component, $value, $format = 'html') {
  $fid = isset($value[0]) ? $value[0] : NULL;
  return array(
     '#title' => $component['name'],
     '#value' => $fid ? systematicreview_get_file($fid) : NULL,
     '#weight' => $component['weight'],
     '#theme' => 'systematicreview_display_file',
     '#theme_wrappers' => $format == 'text' ? array('systematicreview_element_text') : array('systematicreview_element'),
     '#systematicreview_component' => $component,
     '#format' => $format,
  );
}

/**
 * Format the output of text data for this component
 */
function theme_systematicreview_display_file($variables) {
  $element = $variables['element'];

  $file = $element['#value'];
  $url = !empty($file) ? systematicreview_file_url($file->uri) : t('no upload');
  return !empty($file) ? ($element['#format'] == 'text' ? $url : l($file->filename, $url)) : ' ';
}

/**
 * Implements _systematicreview_delete_component().
 */
function _systematicreview_delete_file($component, $value) {
  // Delete an individual submission file.
  if (isset($value[0]) && ($file = systematicreview_get_file($value[0]))) {
    file_delete($file);
  }
}

/**
 * Implements _systematicreview_analysis_component().
 */
function _systematicreview_analysis_file($component, $sids = array()) {
  $query = db_select('systematicreview_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('no', 'data'))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid']);

  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }

  $nonblanks = 0;
  $sizetotal = 0;
  $submissions = 0;

  $result = $query->execute();
  foreach ($result as $data) {
    $file = systematicreview_get_file($data['data']);
    if (isset($file->filesize)) {
      $nonblanks++;
      $sizetotal += $file->filesize;
    }
    $submissions++;
  }

  $rows[0] = array(t('Left Blank'), ($submissions - $nonblanks));
  $rows[1] = array(t('User uploaded file'), $nonblanks);
  $rows[2] = array(t('Average uploaded file size'), ($sizetotal != 0 ? (int) (($sizetotal/$nonblanks)/1024) . ' KB' : '0'));
  return $rows;
}

/**
 * Implements _systematicreview_table_component().
 */
function _systematicreview_table_file($component, $value) {
  $output = '';
  $file = systematicreview_get_file($value[0]);
  if (!empty($file->fid)) {
    $output = '<a href="' . systematicreview_file_url($file->uri) . '">' . check_plain(systematicreview_file_name($file->uri)) . '</a>';
    $output .= ' (' . (int) ($file->filesize/1024) . ' KB)';
  }
  return $output;
}

/**
 * Implements _systematicreview_csv_headers_component().
 */
function _systematicreview_csv_headers_file($component, $export_options) {
  $header = array();
  // Two columns in header.
  $header[0] = array('', '');
  $header[1] = array($component['name'], '');
  $header[2] = array(t('Name'), t('Filesize (KB)'));
  return $header;
}

/**
 * Implements _systematicreview_csv_data_component().
 */
function _systematicreview_csv_data_file($component, $export_options, $value) {
  $file = systematicreview_get_file($value[0]);
  return empty($file->filename) ? array('', '') : array(systematicreview_file_url($file->uri), (int) ($file->filesize/1024));
}

/**
 * Helper function to create proper file names for uploaded file.
 */
function systematicreview_file_name($filepath) {
  if (!empty($filepath)) {
    $info = pathinfo($filepath);
    $file_name = $info['basename'];
  }
  return isset($file_name) ? $file_name : '';
}

/**
 * Helper function to create proper URLs for uploaded file.
 */
function systematicreview_file_url($uri) {
  if (!empty($uri)) {
    $file_url = file_create_url($uri);
  }
  return isset($file_url) ? $file_url : '';
}

/**
 * Helper function to load a file from the database.
 */
function systematicreview_get_file($fid) {
  return file_load($fid);
}
