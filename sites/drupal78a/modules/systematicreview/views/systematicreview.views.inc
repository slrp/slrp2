<?php

/**
 * @file
 * Views hooks implemented for the SystematicReview module.
 */

function systematicreview_views_data() {
  /**
   * Submissions table definitions.
   */
  $data['systematicreview_submissions']['table']['group'] = t('SystematicReview submissions');
  $data['systematicreview_submissions']['table']['base'] = array(
    'field' => 'sid',
    'title' => t('SystematicReview submissions'),
    'help' => t('Submissions generated from SystematicReview forms.'),
  );

  // sid
  $data['systematicreview_submissions']['sid'] = array(
    'title' => t('Sid'),
    'help' => t('The submission ID of the submission.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'title' => t('Sid'),
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // nid
  $data['systematicreview_submissions']['nid'] = array(
    'title' => t('Node'),
    'help' => t('The node these submissions were generated from.'),
    'relationship' => array(
      'base' => 'node',
      'handler' => 'views_handler_relationship',
      'label' => t('Node'),
      'skip base' => array('node', 'node_revisions'),
    ),
  );

  // uid
  $data['systematicreview_submissions']['uid'] = array(
    'title' => t('User'),
    'help' => t('The users who submitted these submissions.'),
    'relationship' => array(
      'base' => 'users',
      'handler' => 'views_handler_relationship',
      'label' => t('User'),
      'skip base' => array('users', 'node', 'node_revisions'),
    ),
  );

  // is_draft
  $data['systematicreview_submissions']['is_draft'] = array(
    'title' => t('Draft'),
    'help' => t('Whether or not the submission is a draft.'),
    'field' => array(
      'handler' => 'systematicreview_handler_field_is_draft',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'systematicreview_handler_filter_is_draft',
    ),
    'sort' => array(
       'handler' => 'views_handler_sort',
    ),
  );

  // submitted
  $data['systematicreview_submissions']['submitted'] = array(
    'title' => t('Submitted'),
    'help' => t('The date this submission was submitted.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'title' => t('Submitted'),
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  // remote_addr
  $data['systematicreview_submissions']['remote_addr'] = array(
    'title' => t('Remote address'),
    'help' => t('The remote IP address of the user that submitted this submission.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'title' => t('Remote address'),
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // view link
  $data['systematicreview_submissions']['view_submission'] = array(
    'field' => array(
      'title' => t('View link'),
      'help' => t('Provide a simple link to view the submission.'),
      'handler' => 'systematicreview_handler_field_submission_link',
      'link_type' => 'view',
    ),
  );

  // edit link
  $data['systematicreview_submissions']['edit_submission'] = array(
    'field' => array(
      'title' => t('Edit link'),
      'help' => t('Provide a simple link to edit the submission.'),
      'handler' => 'systematicreview_handler_field_submission_link',
      'link_type' => 'edit',
    ),
  );

  // delete link
  $data['systematicreview_submissions']['delete_submission'] = array(
    'field' => array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the submission.'),
      'handler' => 'systematicreview_handler_field_submission_link',
      'link_type' => 'delete',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function systematicreview_views_data_alter(&$data) {
  // Submission count (node).
  $data['node']['systematicreview_submission_count_node'] = array(
    'field' => array(
      'title' => t('SystematicReview submission count'),
      'help' => t('The number of systematicreview submissions on this node.'),
      'handler' => 'systematicreview_handler_field_submission_count',
      'count_type' => 'node',
    ),
  );

  // Submission count (user).
  $data['users']['systematicreview_submission_count_user'] = array(
    'field' => array(
      'title' => t('SystematicReview submission count'),
      'help' => t('The number of systematicreview submissions for this user.'),
      'handler' => 'systematicreview_handler_field_submission_count',
      'count_type' => 'users',
    ),
  );

  // Link for editing the systematicreview.
  $data['node']['systematicreview_edit'] = array(
    'field' => array(
      'title' => t('SystematicReview edit link'),
      'help' => t('Provide a simple link to edit the systematicreview components and configuration.'),
      'handler' => 'systematicreview_handler_field_node_link_edit',
    ),
  );

  // Link for viewing systematicreview results.
  $data['node']['systematicreview_results'] = array(
    'field' => array(
      'title' => t('SystematicReview results link'),
      'help' => t('Provide a simple link to view the results of a systematicreview.'),
      'handler' => 'systematicreview_handler_field_node_link_results',
    ),
  );

  // SystematicReview form content.
  $data['node']['systematicreview_form_body'] = array(
    'field' => array(
      'title' => t('SystematicReview form body'),
      'help' => t('The SystematicReview form body display for this node.'),
      'handler' => 'systematicreview_handler_field_form_body',
    ),
  );
}

/**
 * Implements hook_views_handlers().
 */
function systematicreview_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'systematicreview') . '/views',
    ),
    'handlers' => array(
      'systematicreview_handler_field_submission_link' => array(
        'parent' => 'views_handler_field',
        'file' => 'systematicreview_handler_field_submission_link.inc',
      ),
      'systematicreview_handler_field_submission_count' => array(
        'parent' => 'views_handler_field',
        'file' => 'systematicreview_handler_field_submission_count.inc',
      ),
      'systematicreview_handler_field_node_link_edit' => array(
        'parent' => 'views_handler_field_node_link',
      ),
      'systematicreview_handler_field_node_link_results' => array(
        'parent' => 'views_handler_field_node_link',
      ),
      'systematicreview_handler_field_form_body' => array(
        'parent' => 'views_handler_field',
        'file' => 'systematicreview_handler_field_form_body.inc',
      ),
      'systematicreview_handler_field_is_draft' => array(
        'parent' => 'views_handler_field',
        'file' => 'systematicreview_handler_field_is_draft.inc',
      ),
      'systematicreview_handler_filter_is_draft' => array(
        'parent' => 'views_handler_filter_in_operator',
        'file' => 'systematicreview_handler_filter_is_draft.inc',
      ),
    ),
  );
}