<?php

/**
 * @file
 * SystematicReview module page break component.
 */

/**
 * Implements _systematicreview_defaults_component().
 */
function _systematicreview_defaults_pagebreak() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'extra' => array(),
  );
}

/**
 * Implements _systematicreview_theme_component().
 */
function _systematicreview_theme_pagebreak() {
  return array(
    'systematicreview_display_pagebreak' => array(
      'render element' => 'element',
      'file' => 'components/pagebreak.inc',
    ),
  );
}

/**
 * Implements _systematicreview_edit_component().
 */
function _systematicreview_edit_pagebreak($component) {
  $form = array();
  // Force the parent to always be root.
  $form['position']['pid'] = array(
    '#type' => 'hidden',
    '#value' => '0',
  );

  $form['extra']['description'] = array();  // No description.
  $form['display'] = array('#type' => 'markup'); // Hide the display options.

  return $form;
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_render_pagebreak($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#type' => 'hidden',
    '#value' => $component['name'],
    '#weight' => $component['weight'],
  );
  return $element;
}

/**
 * Implements _systematicreview_render_component().
 */
function _systematicreview_display_pagebreak($component, $value = NULL, $format = 'html') {
  $element = array(
    '#theme' => 'systematicreview_display_pagebreak',
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#format' => $format,
    '#systematicreview_component' => $component,
  );
  return $element;
}

/**
 * Format the text output data for this component.
 */
function theme_systematicreview_display_pagebreak($variables) {
  $element = $variables['element'];

  return $element['#format'] == 'html' ? '<h2 class="systematicreview-page">' . check_plain($element['#title']) . '</h2>' : "--" . $element['#title'] . "--\n";
}
