<?php

/**
 * @file
 * SystematicReview module fieldset component.
 */

/**
 * Implements _systematicreview_defaults_component().
 */
function _systematicreview_defaults_fieldset() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
      'title_display' => 0,
      'collapsible' => 0,
      'collapsed' => 0,
      'description' => '',
    ),
  );
}

/**
 * Implements _systematicreview_edit_component().
 */
function _systematicreview_edit_fieldset($component) {
  $form = array();
  $form['display']['collapsible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $component['extra']['collapsible'],
    '#description' => t('If this fieldset is collapsible, the user may open or close the fieldset.'),
    '#weight' => 0,
    '#parents' => array('extra', 'collapsible'),
  );
  $form['display']['collapsed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Collapsed by Default'),
    '#default_value' => $component['extra']['collapsed'],
    '#description' => t('Collapsible fieldsets are "open" by default. Select this option to default the fieldset to "closed."'),
    '#weight' => 3,
    '#parents' => array('extra', 'collapsed'),
  );
  return $form;
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_render_fieldset($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'fieldset',
    '#title' => $filter ? _systematicreview_filter_xss($component['name']) : $component['name'],
    '#weight' => $component['weight'],
    '#description' => $filter ? _systematicreview_filter_descriptions($component['extra']['description']) : $component['extra']['description'],
    '#collapsible' => $component['extra']['collapsible'],
    '#collapsed' => $component['extra']['collapsed'],
    '#attributes' => array('class' => array('systematicreview-component-fieldset'), 'id' => 'systematicreview-component-' . $component['form_key']),
    '#pre_render' => array('systematicreview_fieldset_prerender'),
    '#systematicreview_component' => $component,
  );

  // Hide the fieldset title if #title_display is 'none'.
  if (!empty($component['extra']['title_display']) && $component['extra']['title_display'] == 'none') {
    $element['#title'] = NULL;
  }

  return $element;
}

/**
 * Pre-render function to set a fieldset ID.
 */
function systematicreview_fieldset_prerender($element) {
  $element['#attributes']['id'] = 'systematicreview-component-' . str_replace('_', '-', implode('--', array_slice($element['#parents'], 1)));
  return $element;
}

/**
 * Implements _systematicreview_display_component().
 */
function _systematicreview_display_fieldset($component, $value, $format = 'html') {
  if ($format == 'text') {
    $element = array(
      '#title' => $component['name'],
      '#weight' => $component['weight'],
      '#theme_wrappers' => array('systematicreview_element_text'),
    );
  }
  else {
    $element = _systematicreview_render_fieldset($component, $value);
  }

  $element['#systematicreview_component'] = $component;
  $element['#format'] = $format;

  return $element;
}
