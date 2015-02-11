<?php

/**
 * @file
 *  Template file for the votes review.
 *
 * Available variables:
 *  - $votes: and array where each element contains another array of 2 elements:
 *    - node: the full node object to show
 *    - points: the number of points for this node.
 */

print '<div class="checkout-review">';
  foreach ($votes as $vote) {
    print '<div>' . l($vote['node']->title, 'node/' . $vote['node']->nid, array('html' => TRUE))  .' <span>' . format_plural($vote['points'], '@count point', '@count points') . '</span></div>';
  }
print '</div>';
