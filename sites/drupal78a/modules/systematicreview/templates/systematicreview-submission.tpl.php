<?php

/**
 * @file
 * Customize the display of a systematic review submission.
 *
 * Available variables:
 * - $node: The node object for this systematic review.
 * - $submission: The SystematicReview submission array.
 * - $email: If sending this submission in an e-mail, the e-mail configuration
 *   options.
 * - $format: The format of the submission being printed, either "html" or
 *   "text".
 * - $renderable: The renderable submission array, used to print out individual
 *   parts of the submission, just like a $form array.
 */
?>

<?php print drupal_render_children($renderable); ?>
