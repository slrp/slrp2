<?php

/**
 * @file
 * SystematicReview module markup component.
 */

/**
 * Implements _systematicreview_defaults_component().
 */
function _systematicreview_defaults_markup() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
      'format' => NULL,
    ),
  );
}

/**
 * Implements _systematicreview_edit_component().
 */
function _systematicreview_edit_markup($component) {
  $form = array();
  $form['value'] = array(
    '#type' => 'text_format',
    '#title' => t('Value'),
    '#default_value' => $component['value'],
    '#description' => t('Markup allows you to enter custom HTML or PHP logic into your form.') . theme('systematicreview_token_help'),
    '#weight' => -1,
    '#format' => $component['extra']['format'],
    '#element_validate' => array('_systematicreview_edit_markup_validate'),
  );

  $form['extra']['description'] = array(); // No description for markup.
  $form['display'] = array('#type' => 'markup'); // Hide the display options.

  return $form;
}

/**
 * Element validate handler; Set the text format value.
 */
function _systematicreview_edit_markup_validate($form, &$form_state) {
  if (is_array($form_state['values']['value'])) {
    $form_state['values']['extra']['format'] = $form_state['values']['value']['format'];
    $form_state['values']['value'] = $form_state['values']['value']['value'];
  }
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_render_markup($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'markup',
    '#title' => $filter ? NULL : $component['name'],
    '#weight' => $component['weight'],
    '#markup' => $filter ? _systematicreview_filter_values(check_markup($component['value'], $component['extra']['format'], '', TRUE), NULL, NULL, NULL, FALSE) : $component['value'],
    '#format' => $component['extra']['format'],
    '#theme_wrappers' => array('systematicreview_element'),
    '#systematicreview_component' => $component,
  );

  $element['#value'] = $element['#markup'];

  return $element;
}

/**
 * Implements _systematicreview_display_component().
 */
function _systematicreview_display_markup($component, $value, $format = 'html') {
  return array();
}
