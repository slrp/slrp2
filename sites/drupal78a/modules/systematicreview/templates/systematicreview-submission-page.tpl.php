<?php

/**
 * @file
 * Customize the navigation shown when editing or viewing submissions.
 *
 * Available variables:
 * - $node: The node object for this systematic review.
 * - $mode: Either "form" or "display". May be other modes provided by other
 *          modules, such as "print" or "pdf".
 * - $submission: The SystematicReview submission array.
 * - $submission_content: The contents of the systematic review submission.
 * - $submission_navigation: The previous submission ID.
 * - $submission_information: The next submission ID.
 */
?>

<?php if ($mode == 'display' || $mode == 'form'): ?>
  <div class="clearfix">
    <?php print $submission_actions; ?>
    <?php print $submission_navigation; ?>
  </div>
<?php endif; ?>

<?php print $submission_information; ?>

<div class="systematicreview-submission">
  <?php print render($submission_content); ?>
</div>

<?php if ($mode == 'display' || $mode == 'form'): ?>
  <div class="clearfix">
    <?php print $submission_navigation; ?>
  </div>
<?php endif; ?>
