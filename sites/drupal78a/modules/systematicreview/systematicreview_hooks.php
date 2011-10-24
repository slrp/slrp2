<?php

/**
 * @file
 * Sample hooks demonstrating usage in SystematicReview.
 */

/**
 * @defgroup systematicreview_hooks SystematicReview Module Hooks
 * @{
 * SystematicReview's hooks enable other modules to intercept events within Systematic Review, such
 * as the completion of a submission or adding validation. Systematic Review's hooks also
 * allow other modules to provide additional components for use within forms.
 */

/**
 * Define callbacks that can be used as select list options.
 *
 * @return
 *   An array of callbacks that can be used for select list options. This array
 *   should be keyed by the "name" of the pre-defined list. The values should
 *   be an array with the following additional keys:
 *     - title: The translated title for this list.
 *     - options callback: The name of the function that will return the list.
 *     - options arguments: Any additional arguments to send to the callback.
 *     - file: Optional. The file containing the options callback, relative to
 *       the module root.
 */
function hook_systematicreview_select_options_info() {
  $items = array();

  $items['days'] = array(
    'title' => t('Days of the week'),
    'options callback' => 'systematicreview_options_days',
    'file' => 'includes/systematicreview.options.inc',
  );

  return $items;
}

/**
 * Respond to the loading of SystematicReview submissions.
 *
 * @param $submissions
 *   An array of SystematicReview submissions that are being loaded, keyed by the
 *   submission ID. Modifications to the submissions are done by reference.
 */
function hook_systematicreview_submission_load(&$submissions) {
  foreach ($submissions as $sid => $submission) {
    $submissions[$sid]->new_property = 'foo';
  }
}

/**
 * Modify a SystematicReview submission, prior to saving it in the database.
 *
 * @param $node
 *   The SystematicReview node on which this submission was made.
 * @param $submission
 *   The SystematicReview submission that is about to be saved to the database.
 */
function hook_systematicreview_submission_presave($node, &$submission) {
  // Update some component's value before it is saved.
  $component_id = 4;
  $submission->data[$component_id]['value'][0] = 'foo';
}

/**
 * Respond to a SystematicReview submission being inserted.
 *
 * Note that this hook is called after a submission has already been saved to
 * the database. If needing to modify the submission prior to insertion, use
 * hook_systematicreview_submission_presave().
 *
 * @param $node
 *   The SystematicReview node on which this submission was made.
 * @param $submission
 *   The SystematicReview submission that was just inserted into the database.
 */
function hook_systematicreview_submission_insert($node, $submission) {
  // Insert a record into a 3rd-party module table when a submission is added.
  db_insert('mymodule_table')
    ->fields(array(
      'nid' => $node->nid,
      'sid' => $submission->sid,
      'foo' => 'foo_data',
    ))
    ->execute();
}

/**
 * Respond to a SystematicReview submission being updated.
 *
 * Note that this hook is called after a submission has already been saved to
 * the database. If needing to modify the submission prior to updating, use
 * hook_systematicreview_submission_presave().
 *
 * @param $node
 *   The SystematicReview node on which this submission was made.
 * @param $submission
 *   The SystematicReview submission that was just updated in the database.
 */
function hook_systematicreview_submission_update($node, $submission) {
  // Update a record in a 3rd-party module table when a submission is updated.
  db_update('mymodule_table')
    ->fields(array(
      'foo' => 'foo_data',
    ))
    ->condition('nid', $node->nid)
    ->condition('sid', $submission->sid)
    ->execute();
}

/**
 * Respond to a SystematicReview submission being deleted.
 *
 * @param $node
 *   The SystematicReview node on which this submission was made.
 * @param $submission
 *   The SystematicReview submission that was just deleted from the database.
 */
function hook_systematicreview_submission_delete($node, $submission) {
  // Delete a record from a 3rd-party module table when a submission is deleted.
  db_delete('mymodule_table')
    ->condition('nid', $node->nid)
    ->condition('sid', $submission->sid)
    ->execute();
}

/**
 * Provide a list of actions that can be executed on a submission.
 *
 * Some actions are displayed in the list of submissions such as edit, view, and
 * delete. All other actions are displayed only when viewing the submission.
 * These additional actions may be specified in this hook. Examples included
 * directly in the SystematicReview module include PDF, print, and resend e-mails. Other
 * modules may extend this list by using this hook.
 *
 * @param $node
 *   The SystematicReview node on which this submission was made.
 * @param $submission
 *   The SystematicReview submission on which the actions may be performed.
 */
function hook_systematicreview_submission_actions($node, $submission) {
  if (systematicreview_results_access($node)) {
    $actions['myaction'] = array(
      'title' => t('Do my action'),
      'href' => 'node/' . $node->nid . '/submission/' . $submission->sid . '/myaction',
      'query' => drupal_get_destination(),
    );
  }

  return $actions;
}

/**
 * Alter the display of a SystematicReview submission.
 *
 * This function applies to both e-mails sent by SystematicReview and normal display of
 * submissions when viewing through the adminsitrative interface.
 *
 * @param $renderable
 *   The SystematicReview submission in a renderable array, similar to FormAPI's
 *   structure. This variable must be passed in by-reference. Important
 *   properties of this array include #node, #submission, #email, and #format,
 *   which can be used to find the context of the submission that is being
 *   rendered.
 */
function hook_systematicreview_submission_render_alter(&$renderable) {
  // Remove page breaks from sent e-mails.
  if (isset($renderable['#email'])) {
    foreach (element_children($renderable) as $key) {
      if ($renderable[$key]['#component']['type'] == 'pagebreak') {
        unset($renderable[$key]);
      }
    }
  }
}

/**
 * Modify a loaded SystematicReview component.
 *
 * IMPORTANT: This hook does not actually exist because components are loaded
 * in bulk as part of systematicreview_node_load(). Use hook_node_load() to modify loaded
 * components when the node is loaded. This example is provided merely to point
 * to hook_node_load().
 *
 * @see hook_nodeapi()
 * @see systematicreview_node_load()
 */
function hook_systematicreview_component_load() {
  // This hook does not exist. Instead use hook_node_load().
}

/**
 * Modify a SystematicReview component before it is saved to the database.
 *
 * Note that most of the time this hook is not necessary, because SystematicReview will
 * automatically add data to the component based on the component form. Using
 * hook_form_alter() will be sufficient in most cases.
 *
 * @see hook_form_alter()
 * @see systematicreview_component_edit_form()
 *
 * @param $component
 *   The SystematicReview component being saved.
 */
function hook_systematicreview_component_presave(&$component) {
  $component['extra']['new_option'] = 'foo';
}

/**
 * Respond to a SystematicReview component being inserted into the database.
 */
function hook_systematicreview_component_insert($component) {
  // Insert a record into a 3rd-party module table when a component is inserted.
  db_insert('mymodule_table')
    ->fields(array(
      'nid' => $component['nid'],
      'cid' => $component['cid'],
      'foo' => 'foo_data',
    ))
    ->execute();
}

/**
 * Respond to a SystematicReview component being updated in the database.
 */
function hook_systematicreview_component_update($component) {
  // Update a record in a 3rd-party module table when a component is updated.
  db_update('mymodule_table')
    ->fields(array(
      'foo' => 'foo_data',
    ))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid'])
    ->execute();
}

/**
 * Respond to a SystematicReview component being deleted.
 */
function hook_systematicreview_component_delete($component) {
  // Delete a record in a 3rd-party module table when a component is deleted.
  db_delete('mymodule_table')
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid'])
    ->execute();
}

/**
 * Define components to SystematicReview.
 *
 * @return
 *   An array of components, keyed by machine name. Required properties are
 *   "label" and "description". The "features" array defines which capabilities
 *   the component has, such as being displayed in e-mails or csv downloads.
 *   A component like "markup" for example would not show in these locations.
 *   The possible features of a component include:
 *
 *     - csv
 *     - email
 *     - email_address
 *     - email_name
 *     - required
 *     - conditional
 *     - spam_analysis
 *     - group
 *
 *   Note that most of these features do not indicate the default state, but 
 *   determine if the component can have this property at all. Setting
 *   "required" to TRUE does not mean that a component's fields will always be 
 *   required, but instead give the option to the administrator to choose the
 *   requiredness. See the example implementation for details on how these
 *   features may be set.
 *
 *   An optional "file" may be specified to be loaded when the component is
 *   needed. A set of callbacks will be established based on the name of the
 *   component. All components follow the pattern:
 *
 *   _systematicreview_[callback]_[component]
 *
 *   Where [component] is the name of the key of the component and [callback] is
 *   any of the following:
 *
 *     - defaults
 *     - edit
 *     - render
 *     - display
 *     - submit
 *     - delete
 *     - help
 *     - theme
 *     - analysis
 *     - table
 *     - csv_headers
 *     - csv_data
 *
 * See the sample component implementation for details on each one of these
 * callbacks.
 *
 * @see systematicreview_components()
 */
function hook_systematicreview_component_info() {
  $components = array();

  $components['textfield'] = array(
    'label' => t('Textfield'),
    'description' => t('Basic textfield type.'),
    'features' => array(
      // Add content to CSV downloads. Defaults to TRUE.
      'csv' => TRUE,

      // Show this component in e-mailed submissions. Defaults to TRUE.
      'email' => TRUE,

      // Allow this component to be used as an e-mail FROM or TO address.
      // Defaults to FALSE.
      'email_address' => FALSE,

      // Allow this component to be used as an e-mail SUBJECT or FROM name.
      // Defaults to FALSE.
      'email_name' => TRUE,

      // This component may be toggled as required or not. Defaults to TRUE.
      'required' => TRUE,

      // This component has a title that can be toggled as displayed or not.
      'title_display' => TRUE,

      // This component has a title that can be displayed inline.
      'title_inline' => TRUE,

      // If this component can be used as a conditional SOURCE. All components
      // may always be displayed conditionally, regardless of this setting.
      // Defaults to TRUE.
      'conditional' => TRUE,

      // If this component allows other components to be grouped within it 
      // (like a fieldset or tabs). Defaults to FALSE.
      'group' => FALSE,

      // If this component can be used for SPAM analysis, usually with Mollom.
      'spam_analysis' => FALSE,

      // If this component saves a file that can be used as an e-mail
      // attachment. Defaults to FALSE.
      'attachment' => FALSE,
    ),
    'file' => 'components/textfield.inc',
  );

  return $components;
}

/**
 * Alter the list of available SystematicReview components.
 *
 * @param $components
 *   A list of existing components as defined by hook_systematicreview_component_info().
 *
 * @see hook_systematicreview_component_info()
 */
function hook_systematicreview_component_info_alter(&$components) {
  // Completely remove a component.
  unset($components['grid']);

  // Change the name of a component.
  $components['textarea']['label'] = t('Text box');
}

/**
 * @}
 */

/**
 * @defgroup systematicreview_component Sample SystematicReview Component
 * @{
 * In each of these examples, the word "component" should be replaced with the,
 * name of the component type (such as textfield or select). These are not
 * actual hooks, but instead samples of how SystematicReview integrates with its own
 * built-in components.
 */

/**
 * Specify the default properties of a component.
 *
 * @return
 *   An array defining the default structure of a component.
 */
function _systematicreview_defaults_component() {
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
      'description' => '',
    ),
  );
}

/**
 * Generate the form for editing a component.
 *
 * Create a set of form elements to be displayed on the form for editing this
 * component. Use care naming the form items, as this correlates directly to the
 * database schema. The component "Name" and "Description" fields are added to
 * every component type and are not necessary to specify here (although they
 * may be overridden if desired).
 *
 * @param $component
 *   A SystematicReview component array.
 * @return
 *   An array of form items to be displayed on the edit component page
 */
function _systematicreview_edit_component($component) {
  $form = array();

  // Disabling the description if not wanted.
  $form['description'] = array();

  // Most options are stored in the "extra" array, which stores any settings
  // unique to a particular component type.
  $form['extra']['options'] = array(
    '#type' => 'textarea',
    '#title' => t('Options'),
    '#default_value' => $component['extra']['options'],
    '#description' => t('Key-value pairs may be entered separated by pipes. i.e. safe_key|Some readable option') . theme('systematicreview_token_help'),
    '#cols' => 60,
    '#rows' => 5,
    '#weight' => -3,
    '#required' => TRUE,
  );
  return $form;
}

/**
 * Render a SystematicReview component to be part of a form.
 *
 * @param $component
 *   A SystematicReview component array.
 * @param $value
 *   If editing an existing submission or resuming a draft, this will contain
 *   an array of values to be shown instead of the default in the component
 *   configuration. This value will always be an array, keyed numerically for
 *   each value saved in this field.
 * @param $filter
 *   Whether or not to filter the contents of descriptions and values when
 *   rendering the component. Values need to be unfiltered to be editable by
 *   Form Builder.
 *
 * @see _systematicreview_client_form_add_component()
 */
function _systematicreview_render_component($component, $value = NULL, $filter = TRUE) {
  $form_item = array(
    '#type' => 'textfield',
    '#title' => $filter ? _systematicreview_filter_xss($component['name']) : $component['name'],
    '#required' => $component['mandatory'],
    '#weight' => $component['weight'],
    '#description'   => $filter ? _systematicreview_filter_descriptions($component['extra']['description']) : $component['extra']['description'],
    '#default_value' => $filter ? _systematicreview_filter_values($component['value']) : $component['value'],
    '#prefix' => '<div class="systematicreview-component-textfield" id="systematicreview-component-' . $component['form_key'] . '">',
    '#suffix' => '</div>',
  );

  if (isset($value)) {
    $form_item['#default_value'] = $value[0];
  }

  return $form_item;
}

/**
 * Display the result of a submission for a component.
 *
 * The output of this function will be displayed under the "Results" tab then
 * "Submissions". This should output the saved data in some reasonable manner.
 *
 * @param $component
 *   A SystematicReview component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the systematicreview_submitted_data database table schema.
 * @param $format
 *   Either 'html' or 'text'. Defines the format that the content should be
 *   returned as. Make sure that returned content is run through check_plain()
 *   or other filtering functions when returning HTML.
 * @return
 *   A renderable element containing at the very least these properties:
 *    - #title
 *    - #weight
 *    - #component
 *    - #format
 *    - #value
 *   SystematicReview also uses #theme_wrappers to output the end result to the user,
 *   which will properly format the label and content for use within an e-mail
 *   (such as wrapping the text) or as HTML (ensuring consistent output).
 */
function _systematicreview_display_component($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'systematicreview_display_textfield',
    '#theme_wrappers' => $format == 'html' ? array('systematicreview_element') : array('systematicreview_element_text'),
    '#post_render' => array('systematicreview_element_wrapper'),
    '#field_prefix' => $component['extra']['field_prefix'],
    '#field_suffix' => $component['extra']['field_suffix'],
    '#component' => $component,
    '#format' => $format,
    '#value' => isset($value[0]) ? $value[0] : '',
  );
}

/**
 * A hook for changing the input values before saving to the database.
 *
 * SystematicReview expects a component to consist of a single field, or a single array 
 * of fields. If you have a component that requires a deeper form tree
 * you must flatten the data into a single array using this callback 
 * or by setting #parents on each field to avoid data loss and/or unexpected
 * behavior. 
 *
 * Note that Systematic Review will save the result of this function directly into the
 * database.
 *
 * @param $component
 *   A SystematicReview component array.
 * @param $value
 *   The POST data associated with the user input.
 * @return
 *   An array of values to be saved into the database. Note that this should be
 *   a numerically keyed array.
 */
function _systematicreview_submit_component($component, $value) {
  // Clean up a phone number into 123-456-7890 format.
  if ($component['extra']['phone_number']) {
    $matches = array();
    $number = preg_replace('[^0-9]', $value[0]);
    if (strlen($number) == 7) {
      $number = substr($number, 0, 3) . '-' . substr($number, 3, 4);
    }
    else {
      $number = substr($number, 0, 3) . '-' . substr($number, 3, 3) . '-' . substr($number, 6, 4);
    }
  }

  $value[0] = $number;
  return $value;
}

/**
 * Delete operation for a component or submission.
 *
 * @param $component
 *   A SystematicReview component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the systematicreview_submitted_data database schema.
 */
function _systematicreview_delete_component($component, $value) {
  // Delete corresponding files when a submission is deleted.
  $filedata = unserialize($value['0']);
  if (isset($filedata['filepath']) && is_file($filedata['filepath'])) {
    unlink($filedata['filepath']);
    db_query("DELETE FROM {files} WHERE filepath = '%s'", $filedata['filepath']);
  }
}

/**
 * Module specific instance of hook_help().
 *
 * This allows each SystematicReview component to add information into hook_help().
 */
function _systematicreview_help_component($section) {
  switch ($section) {
    case 'admin/config/content/systematicreview#grid_description':
      return t('Allows creation of grid questions, denoted by radio buttons.');
  }
}

/**
 * Module specific instance of hook_theme().
 *
 * This allows each SystematicReview component to add information into hook_theme().
 */
function _systematicreview_theme_component() {
  return array(
    'systematicreview_grid' => array(
      'arguments' => array('grid_element' => NULL),
    ),
    'systematicreview_mail_grid' => array(
      'arguments' => array('component' => NULL, 'value' => NULL),
    ),
  );
}

/**
 * Calculate and returns statistics about results for this component.
 *
 * This takes into account all submissions to this systematicreview. The output of this
 * function will be displayed under the "Results" tab then "Analysis".
 *
 * @param $component
 *   An array of information describing the component, directly correlating to
 *   the systematicreview_component database schema.
 * @param $sids
 *   An optional array of submission IDs (sid). If supplied, the analysis will
 *   be limited to these sids.
 * @param $single
 *   Boolean flag determining if the details about a single component are being
 *   shown. May be used to provided detailed information about a single
 *   component's analysis, such as showing "Other" options within a select list.
 * @return
 *   An array of data rows, each containing a statistic for this component's
 *   submissions.
 */
function _systematicreview_analysis_component($component, $sids = array(), $single = FALSE) {
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
    $header[] = $option;
  }

  // Add questions as each row.
  foreach ($questions as $qkey => $question) {
    $row = array($question);
    foreach ($options as $okey => $option) {
      $row[] = !empty($counts[$qkey][$okey]) ? $counts[$qkey][$okey] : 0;
    }
    $rows[] = $row;
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('systematicreview-grid'))));

  return array(array(array('data' => $output, 'colspan' => 2)));
}

/**
 * The output of this function will be displayed under the "Results" tab then
 * "Table".
 *
 * @param $component
 *   A SystematicReview component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the systematicreview_submitted_data database schema.
 * @return
 *   Textual output formatted for human reading.
 */
function _systematicreview_table_component($component, $value) {
  $questions = array_values(_systematicreview_component_options($component['extra']['questions']));
  $output = '';
  // Set the value as a single string.
  if (is_array($value)) {
    foreach ($value as $item => $value) {
      if ($value !== '') {
        $output .= $questions[$item] . ': ' . check_plain($value) . '<br />';
      }
    }
  }
  else {
    $output = check_plain(!isset($value['0']) ? '' : $value['0']);
  }
  return $output;
}

/**
 * Return the header for this component to be displayed in a CSV file.
 *
 * The output of this function will be displayed under the "Results" tab then
 * "Download".
 *
 * @param $component
 *   A SystematicReview component array.
 * @param $export_options
 *   An array of options that may configure export of this field.
 * @return
 *   An array of data to be displayed in the first three rows of a CSV file, not
 *   including either prefixed or trailing commas.
 */
function _systematicreview_csv_headers_component($component, $export_options) {
  $header = array();
  $header[0] = array('');
  $header[1] = array($component['name']);
  $items = _systematicreview_component_options($component['extra']['questions']);
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
 * The output of this function will be displayed under the "Results" tab then
 * "Download".
 *
 * @param $component
 *   A SystematicReview component array.
 * @param $export_options
 *   An array of options that may configure export of this field.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the systematicreview_submitted_data database schema.
 * @return
 *   An array of items to be added to the CSV file. Each value within the array
 *   will be another column within the file. This function is called once for
 *   every row of data.
 */
function _systematicreview_csv_data_component($component, $export_options, $value) {
  $questions = array_keys(_systematicreview_select_options($component['extra']['questions']));
  $return = array();
  foreach ($questions as $key => $question) {
    $return[] = isset($value[$key]) ? $value[$key] : '';
  }
  return $return;
}

/**
 * @}
 */
