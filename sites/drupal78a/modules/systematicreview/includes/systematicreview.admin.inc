<?php

/**
 * @file
 * Administration pages provided by SystematicReview module.
 */

/**
 * Menu callback for admin/config/content/systematicreview.
 */
function systematicreview_admin_settings() {
  module_load_include('inc', 'systematicreview', 'includes/systematicreview.export');

  $node_types = node_type_get_names();
  $form['node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('SystematicReview-enabled content types'),
    '#description' => t('SystematicReview allows you to enable the systematicreview components for any content type.  Choose the types on which you would like to associate systematicreview components.'),
    '#options' => $node_types,
    '#default_value' => systematicreview_variable_get('systematicreview_node_types'),
  );

  $form['components'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available components'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('These are the available field types for your installation of SystematicReview. You may disable any of these components by unchecking its corresponding box. Only checked components will be available in existing or new systematicreviews.'),
  );

  // Add each component to the form:
  $form['components'] = array('#tree' => TRUE);
  $component_types = systematicreview_components(TRUE);
  foreach ($component_types as $key => $component) {
    $form['components'][$key] = array(
      '#title' => $component['label'],
      '#description' => $component['description'],
      '#type' => 'checkbox',
      '#return_value' => 1,
      '#default_value' => $component['enabled'],
    );
  }

  $form['email'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default e-mail values'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['email']['systematicreview_default_from_address']  = array(
    '#type' => 'textfield',
    '#title' => t('From address'),
    '#default_value' => variable_get('systematicreview_default_from_address', variable_get('site_mail', ini_get('sendmail_from'))),
    '#description' => t('The default sender address for emailed systematicreview results; often the e-mail address of the maintainer of your forms.'),
  );

  $form['email']['systematicreview_default_from_name']  = array(
    '#type' => 'textfield',
    '#title' => t('From name'),
    '#default_value' => variable_get('systematicreview_default_from_name', variable_get('site_name', '')),
    '#description' => t('The default sender name which is used along with the default from address.'),
  );

  $form['email']['systematicreview_default_subject']  = array(
    '#type' => 'textfield',
    '#title' => t('Default subject'),
    '#default_value' => variable_get('systematicreview_default_subject', t('Form submission from: %title')),
    '#description' => t('The default subject line of any e-mailed results.'),
  );

  $form['email']['systematicreview_default_format']  = array(
    '#type' => 'radios',
    '#title' => t('Format'),
    '#options' => array(
      0 => t('Plain text'),
      1 => t('HTML'),
    ),
    '#default_value' => variable_get('systematicreview_default_format', 0),
    '#description' => t('The default format for new e-mail settings. SystematicReview e-mail options take precedence over the settings for system-wide e-mails configured in MIME mail.'),
    '#access' => systematicreview_email_html_capable(),
  );

  $form['email']['systematicreview_format_override']  = array(
    '#type' => 'radios',
    '#title' => t('Format override'),
    '#options' => array(
      0 => t('Per-systematicreview configuration of e-mail format'),
      1 => t('Send all e-mails in the default format'),
    ),
    '#default_value' => variable_get('systematicreview_format_override', 0),
    '#description' => t('Force all systematicreview e-mails to be sent in the default format.'),
    '#access' => systematicreview_email_html_capable(),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['systematicreview_use_cookies']  = array(
    '#type' => 'checkbox',
    '#checked_value' => 1,
    '#title' => t('Allow cookies for tracking submissions'),
    '#default_value' => variable_get('systematicreview_use_cookies', 0),
    '#description' => t('<a href="http://www.wikipedia.org/wiki/HTTP_cookie">Cookies</a> can be used to help prevent the same user from repeatedly submitting a systematicreview. This feature is not needed for limiting submissions per user, though it can increase accuracy in some situations. Besides cookies, SystematicReview also uses IP addresses and site usernames to prevent repeated submissions.'),
  );

  $form['advanced']['systematicreview_search_index']  = array(
    '#type' => 'checkbox',
    '#checked_value' => 1,
    '#title' => t('Include systematicreview forms in search index'),
    '#default_value' => variable_get('systematicreview_search_index', 1),
    '#description' => t('When selected, all SystematicReview nodes will have their form components indexed by the search engine.'),
    '#access' => module_exists('search'),
  );

  $form['advanced']['systematicreview_email_address_format'] = array(
    '#type' => 'radios',
    '#title' => t('E-mail address format'),
    '#options' => array(
      'long' => t('Long format: "Example Name" &lt;name@example.com&gt;'),
      'short' => t('Short format: name@example.com'),
    ),
    '#default_value' => variable_get('systematicreview_email_address_format', 'long'),
    '#description' => t('Most servers support the "long" format which will allow for more friendly From addresses in e-mails sent. However many Windows-based servers are unable to send in the long format. Change this option if experiencing problems sending e-mails with SystematicReview.'),
  );

  $form['advanced']['systematicreview_export_format'] = array(
    '#type' => 'radios',
    '#title' => t('Default export format'),
    '#options' => systematicreview_export_list(),
    '#default_value' => variable_get('systematicreview_export_format', 'delimited'),
  );

  $form['advanced']['systematicreview_csv_delimiter']  = array(
    '#type' => 'select',
    '#title' => t('Default export delimiter'),
    '#description' => t('This is the delimiter used in the CSV/TSV file when downloading SystematicReview results. Using tabs in the export is the most reliable method for preserving non-latin characters. You may want to change this to another character depending on the program with which you anticipate importing results.'),
    '#default_value' => variable_get('systematicreview_csv_delimiter', '\t'),
    '#options' => array(
      ','  => t('Comma (,)'),
      '\t' => t('Tab (\t)'),
      ';'  => t('Semicolon (;)'),
      ':'  => t('Colon (:)'),
      '|'  => t('Pipe (|)'),
      '.'  => t('Period (.)'),
      ' '  => t('Space ( )'),
    ),
  );

  $form['advanced']['systematicreview_submission_access_control']  = array(
    '#type' => 'radios',
    '#title' => t('Submission access control'),
    '#options' => array(
      '1' => t('Select the user roles that may submit each individual systematicreview'),
      '0' => t('Disable SystematicReview submission access control'),
    ),
    '#default_value' => variable_get('systematicreview_submission_access_control', 1),
    '#description' => t('By default, the configuration form for each systematicreview allows the administrator to choose which roles may submit the form. You may want to allow users to always submit the form if you are using a separate node access module to control access to systematicreview nodes themselves.'),
  );

  $form = system_settings_form($form);
  $form['#theme'] = 'systematicreview_admin_settings';
  array_unshift($form['#submit'], 'systematicreview_admin_settings_submit');

  return $form;
}

/**
 * Submit handler for the systematicreview_admin_settings() form.
 */
function systematicreview_admin_settings_submit($form, &$form_state) {
  $disabled_components = array();
  foreach ($form_state['values']['components'] as $name => $enabled) {
    if (!$enabled) {
      $disabled_components[] = $name;
    }
  }
  // Update $form_state and let system_settings_form_submit() handle saving.
  $form_state['values']['systematicreview_disabled_components'] = $disabled_components;
  unset($form_state['values']['components']);

  // Change the name of the node type variable and clean it up.
  $form_state['values']['systematicreview_node_types'] = array_keys(array_filter($form_state['values']['node_types']));
  unset($form_state['values']['node_types']);
}

/**
 * Theme the output of the systematicreview_admin_settings() form.
 */
function theme_systematicreview_admin_settings($variables) {
  $form = $variables['form'];

  // Format the components into a table.
  foreach (element_children($form['components']) as $key) {
    $row = array();
    $row[] = $form['components'][$key]['#title'];
    $row[] = $form['components'][$key]['#description'];
    $form['components'][$key]['#title'] = NULL;
    $form['components'][$key]['#description'] = NULL;
    $row[] = array('data' => drupal_render($form['components'][$key]), 'align' => 'center');
    $rows[] = $row;
  }
  $header = array(t('Name'), t('Description'), array('data' => t('Enabled'), 'class' => array('checkbox')));

  // Create the table inside the form.
  $form['components']['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return drupal_render_children($form);
}

/**
 * Menu callback for admin/content/systematicreview. Displays all systematic reviews on the site.
 */
function systematicreview_admin_content() {
  $systematicreview_types = systematicreview_variable_get('systematicreview_node_types');

  $nodes = array();
  if ($systematicreview_types) {
    $nodes = db_select('node', 'n')
      ->fields('n')
      ->condition('n.type', $systematicreview_types, 'IN')
      ->execute()
      ->fetchAllAssoc('nid');
  }

  return theme('systematicreview_admin_content', array('nodes' => $nodes));
}

/**
 * Create a comma-separate list of content types that are systematicreview enabled.
 */
function systematicreview_admin_type_list() {
  $systematicreview_types = systematicreview_variable_get('systematicreview_node_types');
  $systematicreview_type_list = '';
  $systematicreview_type_count = count($systematicreview_types);
  foreach ($systematicreview_types as $n => $type) {
    $systematicreview_type_list .= l(node_type_get_name($type), 'node/add/' . $type);
    if ($n + 1 < $systematicreview_type_count) {
      $systematicreview_type_list .= $systematicreview_type_count == 2 ? ' ' : ', ';
    }
    if ($n + 2 == $systematicreview_type_count) {
      $systematicreview_type_list .= t('or') . ' ';
    }
  }

  return $systematicreview_type_list;
}

/**
 * Generate a list of all systematicreviews avaliable on this site.
 */
function theme_systematicreview_admin_content($variables) {
  $nodes = $variables['nodes'];
  $header = array(
    t('Title'),
    array('data' => t('View'), 'colspan' => '4'),
    array('data' => t('Operations'), 'colspan' => '2')
  );

  $rows = array();
  foreach ($nodes as $node) {
    $rows[] = array(
      l($node->title, 'node/' . $node->nid),
      l(t('Submissions'), 'node/' . $node->nid . '/systematicreview-results'),
      l(t('Analysis'), 'node/' . $node->nid . '/systematicreview-results/analysis'),
      l(t('Table'), 'node/' . $node->nid . '/systematicreview-results/table'),
      l(t('Download'), 'node/' . $node->nid . '/systematicreview-results/download'),
      node_access('update', $node) ? l(t('Edit'), 'node/' . $node->nid . '/edit') : '',
      user_access('delete all systematicreview submissions') ? l(t('Clear'), 'node/' . $node->nid . '/systematicreview-results/clear') : '',
    );
  }

  if (empty($rows)) {
    $systematicreview_types = systematicreview_variable_get('systematicreview_node_types');
    if (empty($systematicreview_types)) {
      $message = t('SystematicReview is currently not enabled on any content types.') . ' ' . t('Visit the <a href="!url">SystematicReview settings</a> page and enable SystematicReview on at least one content type.', array('!url' => url('admin/config/content/systematicreview')));
    }
    else {
      $systematicreview_type_list = systematicreview_admin_type_list();
      $message = t('There are currently no systematic reviews on your site. Create a !types piece of content.', array('!types' => $systematicreview_type_list));
    }

    $rows[] = array(
      array('data' => $message, 'colspan' => 7),
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}
