<?php

/**
 * @file
 * Provides interface and database handling for e-mail settings of a systematic review.
 *
 * @author Jason Zhang
 */

/**
 * Overview form of all components for this systematicreview.
 */
function systematicreview_emails_form($form, $form_state, $node) {
  module_load_include('inc', 'systematicreview', 'includes/systematicreview.components');

  $form['#attached']['library'][] = array('systematicreview', 'admin');

  $form['#tree'] = TRUE;
  $form['#node'] = $node;
  $form['components'] = array();

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  foreach ($node->systematicreview['emails'] as $eid => $email) {
    $email_addresses = array_filter(explode(',', check_plain($email['email'])));
    foreach ($email_addresses as $key => $email_address) {
      $email_addresses[$key] = systematicreview_format_email_address($email_address, NULL, $node, NULL, FALSE);
    }

    $form['emails'][$eid]['email'] = array(
      '#markup' => implode('<br />', $email_addresses),
    );
    $form['emails'][$eid]['subject'] = array(
      '#markup' => check_plain(systematicreview_format_email_subject($email['subject'], $node)),
    );
    $form['emails'][$eid]['from'] = array(
      '#markup' => check_plain(systematicreview_format_email_address($email['from_address'], $email['from_name'], $node, NULL, FALSE)),
    );
  }

  $form['add'] = array(
    '#theme' => 'systematicreview_email_add_form',
    '#tree' => FALSE,
  );

  $form['add']['email_option'] = array(
    '#type' => 'radios',
    '#options' => array(
      'custom' => t('Address'),
      'component' => t('Component value'),
    ),
    '#default_value' => 'custom',
  );

  $form['add']['email_custom'] = array(
    '#type' => 'textfield',
    '#size' => 24,
    '#maxlength' => 500,
  );

  $form['add']['email_component'] = array(
    '#type' => 'select',
    '#options' => systematicreview_component_list($node, 'email_address', FALSE),
  );

  if (empty($form['add']['email_component']['#options'])) {
    $form['add']['email_component']['#options'][''] = t('No available components');
    $form['add']['email_component']['#disabled'] = TRUE;
  }

  $form['add_button'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#weight' => 45,
  );

  $form['#validate'] = array('systematicreview_email_address_validate');

  return $form;
}

/**
 * Theme the node components form. Use a table to organize the components.
 *
 * @param $form
 *   The form array.
 * @return
 *   Formatted HTML form, ready for display.
 */
function theme_systematicreview_emails_form($variables) {
  $form = $variables['form'];
  $node = $form['#node'];

  $header = array(t('E-mail to'), t('Subject'), t('From'), array('data' => t('Operations'), 'colspan' => 2));
  $rows = array();

  if (!empty($form['emails'])) {
    foreach (element_children($form['emails']) as $eid) {
      // Add each component to a table row.
      $rows[] = array(
        drupal_render($form['emails'][$eid]['email']),
        drupal_render($form['emails'][$eid]['subject']),
        drupal_render($form['emails'][$eid]['from']),
        l(t('Edit'), 'node/' . $node->nid . '/systematicreview/emails/' . $eid),
        l(t('Delete'), 'node/' . $node->nid . '/systematicreview/emails/' . $eid . '/delete'),
      );
    }
  }
  else {
    $rows[] = array(array('data' => t('Currently not sending e-mails, add an e-mail recipient below.'), 'colspan' => 5));
  }

  // Add a row containing form elements for a new item.
  $row_data = array(
    array('colspan' => 3, 'data' => drupal_render($form['add'])),
    array('colspan' => 2, 'data' => drupal_render($form['add_button'])),
  );
  $rows[] = array('data' => $row_data, 'class' => array('systematicreview-add-form'));

  $output = '';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'systematicreview-emails')));
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Theme the add new e-mail settings form on the node/x/systematicreview/emails page.
 */
function theme_systematicreview_email_add_form($variables) {
  $form = $variables['form'];

  // Add a default value to the custom e-mail textfield.
  $form['email_custom']['#attributes']['rel'] = t('email@example.com');
  $form['email_custom']['#attributes']['class'] = array('systematicreview-set-active', 'systematicreview-default-value');
  $form['email_option']['custom']['#theme_wrappers'] = array('systematicreview_inline_radio');
  $form['email_option']['custom']['#inline_element'] = drupal_render($form['email_custom']);

  // Render the component value.
  $form['email_component']['#attributes']['class'] = array('systematicreview-set-active');
  $form['email_option']['component']['#theme_wrappers'] = array('systematicreview_inline_radio');
  $form['email_option']['component']['#inline_element'] = drupal_render($form['email_component']);

  return drupal_render_children($form);
}

/**
 * Submit handler for systematicreview_emails_form().
 */
function systematicreview_emails_form_submit($form, &$form_state) {
  if ($form_state['values']['email_option'] == 'custom') {
    $email = $form_state['values']['email_custom'];
  }
  else {
    $email = $form_state['values']['email_component'];
  }
  $form_state['redirect'] = array('node/' . $form['#node']->nid . '/systematicreview/emails/new', array('query' => array('option' => $form_state['values']['email_option'], 'email' => trim($email))));
}

/**
 * Form for configuring an e-mail setting and template.
 */
function systematicreview_email_edit_form($form, $form_state, $node, $email = array()) {
  module_load_include('inc', 'systematicreview', 'includes/systematicreview.components');

  $form['#attached']['library'][] = array('systematicreview', 'admin');
  $form['#attached']['js'][] = array('data' => array('systematicreview' => array('revertConfirm' => t('Are you sure you want to revert any changes to your template back to the default?'))), 'type' => 'setting');

  $form['#tree'] = TRUE;
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );
  $form['eid'] = array(
    '#type' => 'value',
    '#value' => isset($email['eid']) ? $email['eid'] : NULL,
  );

  // All these fields work essentially the same, with a radio button set,
  // a textfield for custom values, and a select list for a component.
  foreach (array('email', 'subject', 'from_address', 'from_name') as $field) {
    switch ($field) {
      case 'email':
        $default_value = NULL;
        $title = t('E-mail to address');
        $description = t('Form submissions will be e-mailed to this address. Any email, select, or hidden form element may be selected as the recipient address. Multiple e-mail addresses may be separated by commas.');
        break;
      case 'subject':
        $default_value = _systematicreview_filter_values(systematicreview_variable_get('systematicreview_default_subject'), $node);
        $title = t('E-mail subject');
        $description = t('Any textfield, select, or hidden form element may be selected as the subject for e-mails.');
        break;
      case 'from_address':
        $default_value = _systematicreview_filter_values(systematicreview_variable_get('systematicreview_default_from_address'), $node);
        $title = t('E-mail from address');
        $description = t('Any email, select, or hidden form element may be selected as the sender\'s e-mail address.');
        break;
      case 'from_name':
        $default_value = _systematicreview_filter_values(systematicreview_variable_get('systematicreview_default_from_name'), $node);
        $title = t('E-mail from name');
        $description = t('Any textfield, select, or hidden form element may be selected as the sender\'s name for e-mails.');
        break;
    }

    $form[$field . '_option'] = array(
      '#title' => $title,
      '#type' => 'radios',
      '#default_value' => is_numeric($email[$field]) ? 'component' : ((empty($default_value) || ($email[$field] != 'default' && isset($email[$field]))) ? 'custom' : 'default'),
      '#description' => $description,
    );
    if (!empty($default_value)) {
      $form[$field . '_option']['#options']['default'] = t('Default: %value', array('%value' => $default_value));
    }
    $form[$field . '_option']['#options']['custom'] = t('Custom');
    $form[$field . '_option']['#options']['component'] = t('Component');

    $form[$field . '_custom'] = array(
      '#type' => 'textfield',
      '#size' => 40,
      '#default_value' => (!is_numeric($email[$field]) && $email[$field] != 'default') ? $email[$field] : NULL,
      '#maxlength' => $field == 'email' ? 500 : 255,
    );
    $options = systematicreview_component_list($node, $field == 'from_address' || $field == 'email' ? 'email_address' : 'email_name', FALSE);
    $form[$field . '_component'] = array(
      '#type' => 'select',
      '#default_value' =>  is_numeric($email[$field]) ? $email[$field] : NULL,
      '#options' => empty($options) ? array('' => t('No available components')) : $options,
      '#disabled' => empty($options) ? TRUE : FALSE,
      '#weight' => 6,
    );
  }

  // Do not show the "E-mail from name" if using the short e-mail format.
  if (variable_get('systematicreview_email_address_format', 'long') == 'short') {
    $form['from_name_option']['#access'] = FALSE;
    $form['from_name_custom']['#access'] = FALSE;
    $form['from_name_component']['#access'] = FALSE;
  }

  // Add the template fieldset.
  $form['template'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail template'),
    '#collapsible' => TRUE,
    '#collapsed' => !empty($email['cid']) && empty($email['template']),
    '#description' => t('An e-mail template can customize the display of e-mails.'),
    '#weight' => 15,
    '#tree' => FALSE,
    '#attributes' => array('id' => 'systematicreview-template-fieldset'),
  );

  $form['template']['template_option'] = array(
    '#type' => 'select',
    '#options' => array(
      'default' => t('Default template'),
      'custom' => t('Custom template'),
    ),
    '#default_value' => $email['template'] == 'default' ? 'default' : 'custom',
  );

  $default_template = theme(array('systematicreview_mail_' . $node->nid, 'systematicreview_mail', 'systematicreview_mail_message'), array('node' => $node, 'email' => $email));
  $template = $email['template'] == 'default' ? $default_template : $email['template'];
  $form['template']['template'] = array(
    '#type' => 'textarea',
    '#rows' => max(10, min(20, count(explode("\n", $template)))),
    '#default_value' => $template,
    '#wysiwyg' => systematicreview_email_html_capable() ? NULL : FALSE,
  );

  $form['template']['html'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send e-mail as HTML'),
    '#default_value' => $email['html'],
    '#access' => systematicreview_email_html_capable() && !variable_get('systematicreview_format_override', 0),
  );

  $form['template']['attachments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include files as attachments'),
    '#default_value' => $email['attachments'],
    '#access' => systematicreview_email_html_capable(),
  );

  $form['template']['tokens'] = array(
    '#markup' => theme('systematicreview_token_help', array('groups' => 'all')),
  );

  $form['template']['components'] = array(
    '#type' => 'select',
    '#title' => t('Included e-mail values'),
    '#options' => systematicreview_component_list($node, 'email', TRUE),
    '#default_value' => array_diff(array_keys($node->systematicreview['components']), $email['excluded_components']),
    '#multiple' => TRUE,
    '#size' => 10,
    '#description' => t('The selected components will be included in the %email_values token. Individual values may still be printed if explicitly specified as a %email[key] in the template.'),
    '#process' => array('systematicreview_component_select'),
  );

  // TODO: Allow easy re-use of existing templates.
  $form['templates']['#tree'] = TRUE;
  $form['templates']['default'] = array(
    '#type' => 'textarea',
    '#value' => $default_template,
    '#resizable' => FALSE,
    '#weight' => 19,
    '#wysiwyg' => FALSE,
  );

  // Add the submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save e-mail settings'),
    '#weight' => 20,
  );

  $form['#validate'] = array('systematicreview_email_address_validate', 'systematicreview_email_edit_form_validate');

  return $form;
}

/**
 * Theme the SystematicReview mail settings section of the node form.
 */
function theme_systematicreview_email_edit_form($variables) {
  $form = $variables['form'];

  // Loop through fields, rendering them into radio button options.
  foreach (array('email', 'subject', 'from_address', 'from_name') as $field) {
    foreach (array('custom', 'component') as $option) {
      $form[$field . '_' . $option]['#attributes']['class'] = array('systematicreview-set-active');
      $form[$field . '_option'][$option]['#theme_wrappers'] = array('systematicreview_inline_radio');
      $form[$field . '_option'][$option]['#inline_element'] = drupal_render($form[$field . '_' . $option]);
    }
    if (isset($form[$field . '_option']['#options']['default'])) {
      $form[$field . '_option']['default']['#theme_wrappers'] = array('systematicreview_inline_radio');
    }
  }

  $details = '';
  $details .= drupal_render($form['subject_option']);
  $details .= drupal_render($form['from_address_option']);
  $details .= drupal_render($form['from_name_option']);
  $form['details'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail header details'),
    '#weight' => 10,
    '#children' => $details,
    '#collapsible' => FALSE,
    '#parents' => array('details'),
    '#groups' => array('details' => array()),
    '#attributes' => array(),
  );

  // Ensure templates are completely hidden.
  $form['templates']['#prefix'] = '<div id="systematicreview-email-templates" style="display: none">';
  $form['templates']['#suffix'] = '</div>';

  // Re-sort the elements since we added the details fieldset.
  $form['#sorted'] = FALSE;
  $children = element_children($form, TRUE);
  return drupal_render_children($form, $children);
}

/**
 * Validate handler for systematicreview_email_edit_form() and systematicreview_emails_form().
 */
function systematicreview_email_address_validate($form, &$form_state) {
  if ($form_state['values']['email_option'] == 'custom') {
    $email = trim($form_state['values']['email_custom']);
    if (empty($email)) {
      form_set_error('email_custom', t('When adding a new custom e-mail, the e-mail field is required.'));
    }
    else {
      $emails = array_filter(explode(',', $email));
      foreach ($emails as $email) {
        if (!valid_email_address(trim($email))) {
          form_set_error('email_custom', t('The entered e-mail address "@email" does not appear valid.', array('@email' => $email)));
        }
      }
    }
  }
}

/**
 * Validate handler for systematicreview_email_edit_form().
 */
function systematicreview_email_edit_form_validate($form, &$form_state) {
  if ($form_state['values']['from_address_option'] == 'custom' && !valid_email_address($form_state['values']['from_address_custom'])) {
    form_set_error('from_address_custom', t('The entered e-mail address "@email" does not appear valid.', array('@email' => $form_state['values']['from_address_custom'])));
  }
}

/**
 * Submit handler for systematicreview_email_edit_form().
 */
function systematicreview_email_edit_form_submit($form, &$form_state) {
  // Ensure a systematicreview record exists.
  $node = $form_state['values']['node'];
  systematicreview_ensure_record($node);

  // Merge the e-mail, name, address, and subject options into single values.
  $email = array(
    'eid' => $form_state['values']['eid'],
    'nid' => $node->nid,
  );

  foreach (array('email', 'from_name', 'from_address', 'subject') as $field) {
    $option = $form_state['values'][$field . '_option'];
    if ($option == 'default') {
      $email[$field] = 'default';
    }
    else {
      $email[$field] = $form_state['values'][$field . '_' . $option];
    }
  }

  // Ensure templates are unaffected by differences in line breaks.
  $form_state['values']['template'] = str_replace(array("\r", "\n"), array('', "\n"), $form_state['values']['template']);
  $form_state['values']['templates']['default'] = str_replace(array("\r", "\n"), array('', "\n"), $form_state['values']['templates']['default']);

  // Set the template value.
  if (strcmp(trim($form_state['values']['templates']['default']), trim($form_state['values']['template'])) == 0) {
    $email['template'] = 'default';
  }
  else {
    $email['template'] = $form_state['values']['template'];
  }

  // Save the attachment and HTML options provided by MIME mail.
  $email['html'] = empty($form_state['values']['html']) ? 0 : 1;
  $email['attachments'] = empty($form_state['values']['attachments']) ? 0 : 1;

  // Save the list of included components.
  // We actually maintain an *exclusion* list, so any new components will
  // default to being included in the %email_values token until unchecked.
  $included = array_keys(array_filter((array) $form_state['values']['components']));
  $excluded = array_diff(array_keys($node->systematicreview['components']), $included);
  $email['excluded_components'] = $excluded;

  if (empty($form_state['values']['eid'])) {
    drupal_set_message(t('Email settings added.'));
    $form_state['values']['eid'] = systematicreview_email_insert($email);
  }
  else {
    drupal_set_message(t('Email settings updated.'));
    systematicreview_email_update($email);
  }

  // Clear the entity cache if Entity Cache module is installed.
  if (module_exists('entitycache')) {
    cache_clear_all($node->nid, 'cache_entity_node');
  }

  $form_state['redirect'] = array('node/' . $node->nid . '/systematicreview/emails');
}

/**
 * Form for deleting an e-mail setting.
 */
function systematicreview_email_delete_form($form, $form_state, $node, $email) {
  $eid = $email['eid'];

  $form['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );
  $form['email'] = array(
    '#type' => 'value',
    '#value' => $email,
  );

  $question = t('Delete e-mail settings?');
  if (is_numeric($email['email'])) {
    $description = t('This will immediately delete the e-mail settings based on the @component component.', array('@component' => $email['email']));
  }
  else {
    $description = t('This will immediately delete the e-mail settings sending to the @address address.', array('@address' => $email['email']));
  }

  return confirm_form($form, $question, 'node/' . $node->nid . '/systematicreview/emails', $description, t('Delete'));
}

/**
 * Submit handler for systematicreview_component_delete_form().
 */
function systematicreview_email_delete_form_submit($form, &$form_state) {
  // Delete the e-mail settings.
  $node = $form_state['values']['node'];
  $email = $form_state['values']['email'];
  systematicreview_email_delete($node, $email);
  drupal_set_message(t('E-mail settings deleted.'));

  // Check if this systematicreview still contains any information.
  unset($node->systematicreview['emails'][$email['eid']]);
  systematicreview_check_record($node);

  // Clear the entity cache if Entity Cache module is installed.
  if (module_exists('entitycache')) {
    cache_clear_all($node->nid, 'cache_entity_node');
  }

  $form_state['redirect'] = 'node/' . $node->nid . '/systematicreview/emails';
}

/**
 * Load an e-mail setting from the database or initialize a new e-mail.
 */
function systematicreview_email_load($eid, $nid) {
  $node = node_load($nid);
  if ($eid == 'new') {
    $email = array(
      'email' => '',
      'subject' => 'default',
      'from_name' => 'default',
      'from_address' => 'default',
      'template' => 'default',
      'excluded_components' => array(),
      'html' => variable_get('systematicreview_default_format', 0),
      'attachments' => 0,
    );
  }
  else {
    $email = isset($node->systematicreview['emails'][$eid]) ? $node->systematicreview['emails'][$eid] : FALSE;
    if (variable_get('systematicreview_format_override', 0)) {
      $email['html'] = variable_get('systematicreview_default_format', 0);
    }
  }

  return $email;
}

/**
 * Insert a new e-mail setting into the database.
 *
 * @param $email
 *   An array of settings for sending an e-mail.
 */
function systematicreview_email_insert($email) {
  // TODO: This is not race-condition safe. Switch to using transactions?
  if (!isset($email['eid'])) {
    $next_id_query = db_select('systematicreview_emails')->condition('nid', $email['nid']);
    $next_id_query->addExpression('MAX(eid) + 1', 'eid');
    $email['eid'] = $next_id_query->execute()->fetchField();
    if ($email['eid'] == NULL) {
      $email['eid'] = 1;
    }
  }

  $email['excluded_components'] = implode(',', $email['excluded_components']);
  $success = drupal_write_record('systematicreview_emails', $email);

  return $success ? $email['eid'] : FALSE;
}

/**
 * Update an existing e-mail setting with new values.
 *
 * @param $email
 *   An array of settings for sending an e-mail containing a nid, eid, and all
 *   other fields from the e-mail form.
 */
function systematicreview_email_update($email) {
  $email['excluded_components'] = implode(',', $email['excluded_components']);
  return drupal_write_record('systematicreview_emails', $email, array('nid', 'eid'));
}

/**
 * Delete an e-mail setting.
 */
function systematicreview_email_delete($node, $email) {
  db_delete('systematicreview_emails')
    ->condition('nid', $node->nid)
    ->condition('eid', $email['eid'])
    ->execute();
}
