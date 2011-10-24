<?php

/**
 * @file
 * Customize confirmation screen after successful submission.
 *
 * This file may be renamed "systematicreview-confirmation-[nid].tpl.php" to target a
 * specific systematicreview e-mail on your site. Or you can leave it
 * "systematicreview-confirmation.tpl.php" to affect all systematicreview confirmations on your
 * site.
 *
 * Available variables:
 * - $node: The node object for this systematicreview.
 * - $confirmation_message: The confirmation message input by the systematicreview author.
 * - $sid: The unique submission ID of this submission.
 */
?>

<div class="systematicreview-confirmation">
  <?php if ($confirmation_message): ?>
    <?php print $confirmation_message ?>
  <?php else: ?>
    <p><?php print t('Thank you, your submission has been received.'); ?></p>
  <?php endif; ?>
</div>

<div class="links">
  <a href="<?php print url('node/'. $node->nid) ?>"><?php print t('Go back to the form') ?></a>
</div>
