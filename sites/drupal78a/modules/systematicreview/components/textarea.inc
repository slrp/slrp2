<?php

/**
 * @file
 * SystematicReview module textarea component.
 */

/**
 * Implements _systematicreview_defaults_component().
 */
function _systematicreview_defaults_textarea() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 0,
    'extra' => array(
      'cols' => '',
      'rows' => '',
      'title_display' => 0,
      'resizable' => 1,
      'disabled' => 0,
      'description' => '',
      'attributes' => array(),
    ),
  );
}


/**
 * Implements _systematicreview_theme_component().
 */
function _systematicreview_theme_textarea() {
  return array(
    'systematicreview_display_textarea' => array(
      'render element' => 'element',
      'file' => 'components/textarea.inc',
    ),
  );
}

/**
 * Implements _systematicreview_edit_component().
 */
function _systematicreview_edit_textarea($component) {
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
  $form['display']['cols'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => $component['extra']['cols'],
    '#description' => t('Width of the textarea.') . ' ' . t('Leaving blank will use the default size.'),
    '#size' => 5,
    '#maxlength' => 10,
    '#parents' => array('extra', 'cols'),
  );
  $form['display']['rows'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#default_value' => $component['extra']['rows'],
    '#description' => t('Height of the textarea.') . ' ' . t('Leaving blank will use the default size.'),
    '#size' => 5,
    '#maxlength' => 10,
    '#parents' => array('extra', 'rows'),
  );
  $form['display']['resizable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Resizable'),
    '#description' => t('Make this field resizable by the user.'),
    '#weight' => 2,
    '#default_value' => $component['extra']['resizable'],
    '#parents' => array('extra', 'resizable'),
  );
  $form['display']['disabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disabled'),
    '#return_value' => 1,
    '#description' => t('Make this field non-editable. Useful for setting an unchangeable default value.'),
    '#weight' => 11,
    '#default_value' => $component['extra']['disabled'],
    '#parents' => array('extra', 'disabled'),
  );
  return $form;
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_render_textarea($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'textarea',
    '#title' => $filter ? _systematicreview_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#default_value' => $filter ? _systematicreview_filter_values($component['value']) : $component['value'],
    '#required' => $component['mandatory'],
    '#weight' => $component['weight'],
    '#description' => $filter ? _systematicreview_filter_descriptions($component['extra']['description']) : $component['extra']['description'],
    '#rows' => !empty($component['extra']['rows']) ? $component['extra']['rows'] : 5,
    '#cols' => !empty($component['extra']['cols']) ? $component['extra']['cols'] : 60,
    '#attributes' => $component['extra']['attributes'],
    '#resizable' => (bool) $component['extra']['resizable'], // MUST be FALSE to disable.
    '#theme_wrappers' => array('systematicreview_element'),
    '#systematicreview_component' => $component,
  );

  if ($component['extra']['disabled']) {
    $element['#attributes']['readonly'] = 'readonly';
  }

  if (isset($value)) {
    $element['#default_value'] = $value[0];
  }

  return $element;
}

/**
 * Implements _systematicreview_display_component().
 */
function _systematicreview_display_textarea($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'systematicreview_display_textarea',
    '#theme_wrappers' => $format == 'html' ? array('systematicreview_element') : array('systematicreview_element_text'),
    '#format' => $format,
    '#value' => isset($value[0]) ? $value[0] : '',
    '#systematicreview_component' => $component,
  );
}

/**
 * Format the output of data for this component.
 */
function theme_systematicreview_display_textarea($variables) {
  $element = $variables['element'];
  $output = $element['#format'] == 'html' ? str_replace("\n", '<br />', check_plain($element['#value'])) : $element['#value'];
  if (drupal_strlen($output) > 80) {
    $output = ($element['#format'] == 'html') ? '<div class="systematicreview-long-answer">' . $output . '</div>' : $output;
  }
  return $output !== '' ? $output : ' ';
}

/**
 * Implements _systematicreview_analysis_component().
 */
function _systematicreview_analysis_textarea($component, $sids = array()) {
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
    if (drupal_strlen(trim($data['data'])) > 0) {
      $nonblanks++;
      $wordcount += str_word_count(trim($data['data']));
    }
    $submissions++;
  }

  $rows[0] = array(t('Left Blank'), ($submissions - $nonblanks));
  $rows[1] = array(t('User entered value'), $nonblanks);
  $rows[2] = array(t('Average submission length in words (ex blanks)'), ($nonblanks != 0 ? number_format($wordcount/$nonblanks, 2) : '0'));
  return $rows;
}

/**
 * Implements _systematicreview_table_component().
 */
function _systematicreview_table_textarea($component, $value) {
  return empty($value[0]) ? '' : check_plain($value[0]);
}

/**
 * Implements _systematicreview_csv_headers_component().
 */
function _systematicreview_csv_headers_textarea($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $component['name'];
  return $header;
}

/**
 * Implements _systematicreview_csv_data_component().
 */
function _systematicreview_csv_data_textarea($component, $export_options, $value) {
  return empty($value[0]) ? '' : $value[0];
}
