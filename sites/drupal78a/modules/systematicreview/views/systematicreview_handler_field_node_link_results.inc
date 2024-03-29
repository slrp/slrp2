<?php

/**
 * @file
 * Views handler to display a results link for SystematicReview submissions.
 */

/**
 * Field handler to present a link node edit.
 */
class systematicreview_handler_field_node_link_results extends views_handler_field_node_link {
  function construct() {
    parent::construct();
    $this->additional_fields['uid'] = 'uid';
    $this->additional_fields['type'] = 'type';
    $this->additional_fields['format'] = array('table' => 'node_revisions', 'field' => 'format');
  }

  function render($values) {
    // ensure user has access to edit this node.
    $node = new stdClass();
    $node->nid = $values->{$this->aliases['nid']};
    $node->uid = $values->{$this->aliases['uid']};
    $node->type = $values->{$this->aliases['type']};
    $node->format = $values->{$this->aliases['format']};
    $node->status = 1; // unpublished nodes ignore access control
    if (!systematicreview_results_access($node)) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('results');
    return l($text, "node/$node->nid/systematicreview-results");
  }
}

