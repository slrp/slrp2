<?php

/**
 * @file
 * Customize the display of a complete systematicreview.
 *
 * This file may be renamed "systematicreview-form-[nid].tpl.php" to target a specific
 * systematicreview on your site. Or you can leave it "systematicreview-form.tpl.php" to affect
 * all systematicreviews on your site.
 *
 * Available variables:
 * - $form: The complete form array.
 * - $nid: The node ID of the SystematicReview.
 *
 * The $form array contains two main pieces:
 * - $form['submitted']: The main content of the user-created form.
 * - $form['details']: Internal information stored by SystematicReview.
 */
?>
<?php
  // If editing or viewing submissions, display the navigation at the top.
  if (isset($form['submission_info']) || isset($form['navigation'])) {
    print drupal_render($form['navigation']);
    print drupal_render($form['submission_info']);
  }

  // Print out the main part of the form.
  // Feel free to break this up and move the pieces within the array.
  print drupal_render($form['submitted']);

  // Always print out the entire $form. This renders the remaining pieces of the
  // form that haven't yet been rendered above.
  print drupal_render_children($form);

  // Print out the navigation again at the bottom.
  if (isset($form['submission_info']) || isset($form['navigation'])) {
    unset($form['navigation']['#printed']);
    print drupal_render($form['navigation']);
  }
