<?php

/**
 * @file
 * SystematicReview module grid component.
 */

// Grid depends on functions provided by select.
systematicreview_component_include('select');

/**
 * Implements _systematicreview_defaults_component().
 */
function _systematicreview_defaults_grid() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'mandatory' => 0,
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
      'options' => '',
      'questions' => '',
      'optrand' => 0,
      'qrand' => 0,
      'title_display' => 0,
      'custom_option_keys' => 0,
      'custom_question_keys' => 0,
      'description' => '',
    ),
  );
}


/**
 * Implements _systematicreview_theme_component().
 */
function _systematicreview_theme_grid() {
  return array(
    'systematicreview_grid' => array(
      'render element' => 'element',
      'file' => 'components/grid.inc',
    ),
    'systematicreview_display_grid' => array(
      'render element' => 'element',
      'file' => 'components/grid.inc',
    ),
  );
}

/**
 * Implements _systematicreview_edit_component().
 */
function _systematicreview_edit_grid($component) {
  $form = array();

  if (module_exists('options_element')) {
    $form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Options'),
      '#collapsible' => TRUE,
      '#description' => t('Options to select across the top. Usually these are ratings such as "poor" through "excellent" or "strongly disagree" through "strongly agree".'),
      '#attributes' => array('class' => array('systematicreview-options-element')),
      '#element_validate' => array('_systematicreview_edit_validate_options'),
    );
    $form['options']['options'] = array(
      '#type' => 'options',
      '#options' => _systematicreview_select_options_from_text($component['extra']['options'], TRUE),
      '#optgroups' => FALSE,
      '#default_value' => FALSE,
      '#optgroups' => FALSE,
      '#key_type' => 'mixed',
      '#key_type_toggle' => t('Customize option keys (Advanced)'),
      '#key_type_toggled' => $component['extra']['custom_option_keys'],
    );

    $form['questions'] = array(
      '#type' => 'fieldset',
      '#title' => t('Questions'),
      '#collapsible' => TRUE,
      '#description' => t('Questions list down the side of the grid.'),
      '#attributes' => array('class' => array('systematicreview-options-element')),
      '#element_validate' => array('_systematicreview_edit_validate_options'),
    );
    $form['questions']['options'] = array(
      '#type' => 'options',
      '#options' => _systematicreview_select_options_from_text($component['extra']['questions'], TRUE),
      '#optgroups' => FALSE,
      '#default_value' => FALSE,
      '#optgroups' => FALSE,
      '#key_type' => 'mixed',
      '#key_type_toggle' => t('Customize question keys (Advanced)'),
      '#key_type_toggled' => $component['extra']['custom_question_keys'],
    );
  }
  else {
    $form['extra']['options'] = array(
      '#type' => 'textarea',
      '#title' => t('Options'),
      '#default_value' => $component['extra']['options'],
      '#description' => t('Options to select across the top. One option per line. <strong>Key-value pairs MUST be specified as "safe_key|Some readable option"</strong>. Use of only alphanumeric characters and underscores is recommended in keys.') . theme('systematicreview_token_help'),
      '#cols' => 60,
      '#rows' => 5,
      '#weight' => -3,
      '#required' => TRUE,
      '#wysiwyg' => FALSE,
      '#element_validate' => array('_systematicreview_edit_validate_select'),
    );
    $form['extra']['questions'] = array(
      '#type' => 'textarea',
      '#title' => t('Questions'),
      '#default_value' => $component['extra']['questions'],
      '#description' => t('Questions list down the side of the grid. One question per line. <strong>Key-value pairs MUST be specified as "safe_key|Some readable option"</strong>. Use of only alphanumeric characters and underscores is recommended in keys.') . theme('systematicreview_token_help'),
      '#cols' => 60,
      '#rows' => 5,
      '#weight' => -2,
      '#required' => TRUE,
      '#wysiwyg' => FALSE,
      '#element_validate' => array('_systematicreview_edit_validate_select'),
    );
  }

  $form['display']['optrand'] = array(
    '#type' => 'checkbox',
    '#title' => t('Randomize Options'),
    '#default_value' => $component['extra']['optrand'],
    '#description' => t('Randomizes the order of options on the top when they are displayed in the form.'),
    '#parents' => array('extra', 'optrand')
  );
  $form['display']['qrand'] = array(
    '#type' => 'checkbox',
    '#title' => t('Randomize Questions'),
    '#default_value' => $component['extra']['qrand'],
    '#description' => t('Randomize the order of the questions on the side when they are displayed in the form.'),
    '#parents' => array('extra', 'qrand')
  );
  return $form;
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_render_grid($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'systematicreview_grid',
    '#title' => $filter ? _systematicreview_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#required' => $component['mandatory'],
    '#weight' => $component['weight'],
    '#description' => $filter ? _systematicreview_filter_descriptions($component['extra']['description']) : $component['extra']['description'],
    '#grid_questions' => _systematicreview_select_options_from_text($component['extra']['questions'], TRUE),
    '#grid_options' => _systematicreview_select_options_from_text($component['extra']['options'], TRUE),
    '#optrand' => $component['extra']['optrand'],
    '#qrand' => $component['extra']['qrand'],
    '#theme' => 'systematicreview_grid',
    '#theme_wrappers' => array('systematicreview_element'),
    '#process' => array('systematicreview_expand_grid'),
    '#systematicreview_component' => $component,
  );

  if ($value) {
    $element['#default_value'] = $value;
  }

  return $element;
}

/**
 * A Form API #process function for SystematicReview grid fields.
 */
function systematicreview_expand_grid($element) {
  $options = $element['#grid_options'];
  $questions = $element['#grid_questions'];

  if (!empty($element['#optrand'])) {
    _systematicreview_shuffle_options($options);
  }

  if (!empty($element['#qrand'])) {
    _systematicreview_shuffle_options($questions);
  }

  foreach ($questions as $key => $question) {
    if ($question != '') {
      $element[$key] = array(
        '#title' => $question,
        '#required' => $element['#required'],
        '#options' => $options,
        '#type' => 'radios',
        '#process' => array('form_process_radios', 'systematicreview_expand_select_ids'),

        // SystematicReview handles validation manually.
        '#validated' => TRUE,
        '#systematicreview_validated' => FALSE,
      );
    }
  }

  $value = isset($element['#default_value']) ? $element['#default_value'] : array();
  foreach (element_children($element) as $key) {
    if (isset($value[$key])) {
      $element[$key]['#default_value'] = ($value[$key] !== '') ? $value[$key] : FALSE;
    }
    else {
      $element[$key]['#default_value'] = FALSE;
    }
  }

  return $element;
}

/**
 * Implements _systematicreview_display_component().
 */
function _systematicreview_display_grid($component, $value, $format = 'html') {
  $questions = _systematicreview_select_options_from_text($component['extra']['questions'], TRUE);
  $options = _systematicreview_select_options_from_text($component['extra']['options'], TRUE);

  $element = array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#format' => $format,
    '#grid_questions' => $questions,
    '#grid_options' => $options,
    '#theme' => 'systematicreview_display_grid',
    '#theme_wrappers' => $format == 'html' ? array('systematicreview_element') : array('systematicreview_element_text'),
    '#sorted' => TRUE,
    '#systematicreview_component' => $component,
  );

  foreach ($questions as $key => $question) {
    if ($question !== '') {
      $element[$question] = array(
        '#title' => $question,
        '#value' => isset($value[$key]) ? $value[$key] : NULL,
      );
    }
  }

  return $element;
}

/**
 * Format the text output for this component.
 */
function theme_systematicreview_display_grid($variables) {
  $element = $variables['element'];

  $component = $element['#systematicreview_component'];
  $format = $element['#format'];

  if ($format == 'html') {
    $rows = array();
    $header = array(array('data' => '', 'class' => array('systematicreview-grid-question')));
    foreach ($element['#grid_options'] as $option) {
      $header[] = array('data' => _systematicreview_filter_xss($option), 'class' => array('checkbox', 'systematicreview-grid-option'));
    }
    foreach (element_children($element) as $key) {
      $row = array();
      $row[] = array('data' => _systematicreview_filter_xss($element[$key]['#title']), 'class' => array('systematicreview-grid-question'));
      foreach ($element['#grid_options'] as $option_value => $option_label) {
        if (strcmp($option_value, $element[$key]['#value']) == 0) {
          $row[] = array('data' => '<strong>X</strong>', 'class' => array('checkbox', 'systematicreview-grid-option'));
        }
        else {
          $row[] = array('data' => '&nbsp;', 'class' => array('checkbox', 'systematicreview-grid-option'));
        }
      }
      $rows[] = $row;
    }

    $option_count = count($header) - 1;
    $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('systematicreview-grid', 'systematicreview-grid-' . $option_count))));
  }
  else {
    $items = array();
    foreach (element_children($element) as $key) {
      $items[] = ' - ' . $element[$key]['#title'] . ': ' . (isset($element['#grid_options'][$element[$key]['#value']]) ? $element['#grid_options'][$element[$key]['#value']] : '');
    }
    $output = implode("\n", $items);
  }

  return $output;
}

/**
 * Implements _systematicreview_analysis_component().
 */
function _systematicreview_analysis_grid($component, $sids = array()) {
  // Generate the list of options and questions.
  $options = _systematicreview_select_options_from_text($component['extra']['options'], TRUE);
  $questions = _systematicreview_select_options_from_text($component['extra']['questions'], TRUE);

  // Generate a lookup table of results.
  $query = db_select('systematicreview_submitted_data', 'wsd')
    ->fields('wsd', array('no', 'data'))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid'])
    ->condition('data', '', '<>')
    ->groupBy('no')
    ->groupBy('data');
  $query->addExpression('COUNT(sid)', 'datacount');

  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }

  $result = $query->execute();
  $counts = array();
  foreach ($result as $data) {
    $counts[$data->no][$data->data] = $data->datacount;
  }

  // Create an entire table to be put into the returned row.
  $rows = array();
  $header = array('');

  // Add options as a header row.
  foreach ($options as $option) {
    $header[] = _systematicreview_filter_xss($option);
  }

  // Add questions as each row.
  foreach ($questions as $qkey => $question) {
    $row = array(_systematicreview_filter_xss($question));
    foreach ($options as $okey => $option) {
      $row[] = !empty($counts[$qkey][$okey]) ? $counts[$qkey][$okey] : 0;
    }
    $rows[] = $row;
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('systematicreview-grid'))));


  return array(array(array('data' => $output, 'colspan' => 2)));
}

/**
 * Implements _systematicreview_table_component().
 */
function _systematicreview_table_grid($component, $value) {
  $questions = _systematicreview_select_options_from_text($component['extra']['questions'], TRUE);
  $output = '';
  // Set the value as a single string.
  if (is_array($value)) {
    foreach ($value as $item => $value) {
      if ($value !== '') {
        $output .= _systematicreview_filter_xss($questions[$item]) . ': ' . _systematicreview_filter_xss($value) . '<br />';
      }
    }
  }
  else {
    $output = !isset($value[0]) ? '' : _systematicreview_filter_xss($value[0]);
  }
  return $output;
}

/**
 * Implements _systematicreview_csv_headers_component().
 */
function _systematicreview_csv_headers_grid($component, $export_options) {
  $header = array();
  $header[0] = array('');
  $header[1] = array($component['name']);
  $items = _systematicreview_select_options_from_text($component['extra']['questions'], TRUE);
  $count = 0;
  foreach ($items as $key => $item) {
    // Empty column per sub-field in main header.
    if ($count != 0) {
      $header[0][] = '';
      $header[1][] = '';
    }
    // The value for this option.
    $header[2][] = $item;
    $count++;
  }

  return $header;
}

/**
 * Implements _systematicreview_csv_data_component().
 */
function _systematicreview_csv_data_grid($component, $export_options, $value) {
  $questions = _systematicreview_select_options_from_text($component['extra']['questions'], TRUE);
  $options = _systematicreview_select_options_from_text($component['extra']['options'], TRUE);
  $return = array();
  foreach ($questions as $key => $question) {
    if (isset($value[$key]) && isset($options[$value[$key]])) {
      $return[] = $export_options['select_keys'] ? $value[$key] : $options[$value[$key]];
    }
    else {
      $return[] = '';
    }
  }
  return $return;
}

function theme_systematicreview_grid($variables) {
  $element = $variables['element'];

  $rows = array();
  $header = array(array('data' => '', 'class' => array('systematicreview-grid-question')));
  $first = TRUE;
  foreach (element_children($element) as $key) {
    $question_element = $element[$key];

    // Set the header for the table.
    if ($first) {
      foreach ($question_element['#options'] as $option) {
        $header[] = array('data' => _systematicreview_filter_xss($option), 'class' => array('checkbox', 'systematicreview-grid-option'));
      }
      $first = FALSE;
    }

    // Create a row with the question title.
    $row = array(array('data' => _systematicreview_filter_xss($question_element['#title']), 'class' => array('systematicreview-grid-question')));

    // Render each radio button in the row.
    $radios = form_process_radios($question_element);
    foreach (element_children($radios) as $key) {
      $radios[$key]['#title_display'] = 'invisible';
      $row[] = array('data' => drupal_render($radios[$key]), 'class' => array('checkbox', 'systematicreview-grid-option'));
    }
    $rows[] = $row;
  }

  $option_count = count($header) - 1;
  return theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('systematicreview-grid', 'systematicreview-grid-' . $option_count))));
}
