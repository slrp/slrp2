<?php

/**
 * @file
 * SystematicReview module hidden component.
 */

/**
 * Implements _systematicreview_defaults_component().
 */
function _systematicreview_defaults_hidden() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
    ),
  );
}

/**
 * Implements _systematicreview_theme_component().
 */
function _systematicreview_theme_hidden() {
  return array(
    'systematicreview_display_hidden' => array(
      'render element' => 'element',
      'file' => 'components/hidden.inc',
    ),
  );
}

/**
 * Implements _systematicreview_edit_component().
 */
function _systematicreview_edit_hidden($component) {
  $form = array();
  $form['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Default value'),
    '#default_value' => $component['value'],
    '#description' => t('The default value of the field.') . theme('systematicreview_token_help'),
    '#cols' => 60,
    '#rows' => 5,
    '#weight' => 0,
  );
  $form['extra']['description'] = array(); // Hide the description box.
  $form['display'] = array('#type' => 'markup'); // Hide the display options.

  return $form;
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_render_hidden($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'hidden',
    '#title' => $filter ? _systematicreview_filter_xss($component['name']) : $component['name'],
    '#default_value' => $filter ? _systematicreview_filter_values($component['value']) : $component['value'],
    '#weight' => $component['weight'],
  );

  if (isset($value[0])) {
    $element['#default_value'] = $value[0];
  }

  return $element;
}

/**
 * Implements _systematicreview_display_component().
 */
function _systematicreview_display_hidden($component, $value, $format = 'html') {
  $element = array(
    '#title' => t('!name (hidden)', array('!name' => $component['name'])),
    '#markup' => isset($value[0]) ? $value[0] : NULL,
    '#weight' => $component['weight'],
    '#theme' => 'systematicreview_display_hidden',
    '#format' => $format,
    '#theme' => 'systematicreview_display_hidden',
    '#theme_wrappers' => $format == 'text' ? array('systematicreview_element_text') : array('systematicreview_element'),
    '#systematicreview_component' => $component,
  );

  if ($format == 'html') {
    $element['#access'] = user_access('edit all systematicreview submissions') || user_access('access all systematicreview results');
  }

  return $element;
}

function theme_systematicreview_display_hidden($variables) {
  $element = $variables['element'];

  return $element['#format'] == 'html' ? check_plain($element['#markup']) : $element['#markup'];
}

/**
 * Implements _systematicreview_analysis_component().
 */
function _systematicreview_analysis_hidden($component, $sids = array()) {
  $query = db_select('systematicreview_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('no', 'data'))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid']);

  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }

  $nonblanks = 0;
  $submissions = 0;
  $wordcount = 0;

  $result = $query->execute();
  foreach ($result as $data) {
    if (strlen(trim($data['data'])) > 0) {
      $nonblanks++;
      $wordcount += str_word_count(trim($data['data']));
    }
    $submissions++;
  }

  $rows[0] = array( t('Empty'), ($submissions - $nonblanks));
  $rows[1] = array( t('Non-empty'), $nonblanks);
  $rows[2] = array( t('Average submission length in words (ex blanks)'),
                    ($nonblanks !=0 ? number_format($wordcount/$nonblanks, 2) : '0'));
  return $rows;
}

/**
 * Implements _systematicreview_csv_data_component().
 */
function _systematicreview_table_hidden($component, $value) {
  return check_plain(empty($value[0]) ? '' : $value[0]);
}

/**
 * Implements _systematicreview_csv_data_component().
 */
function _systematicreview_csv_headers_hidden($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $component['name'];
  return $header;
}

/**
 * Implements _systematicreview_csv_data_component().
 */
function _systematicreview_csv_data_hidden($component, $export_options, $value) {
  return empty($value[0]) ? '' : $value[0];
}
