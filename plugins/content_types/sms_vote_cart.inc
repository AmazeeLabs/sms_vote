<?php
/**
 * @file
 *   Displays the voting cart.
 */

$plugin = array(
  'title' => t('SMS Voting cart'),
  'description' => t('Display the SMS voting cart'),
  'single' => TRUE,
  'content_types' => array('sms_vote_cart'),
  'render callback' => 'sms_vote_cart_render',
  'defaults' => array(),
  'edit form' => 'sms_vote_cart_render_config_form',
  'category' => array(t('Custom'), -9),
);

/**
 * Pane render function
 */
function sms_vote_cart_render($subtype, $conf, $args, $context) {
  $block = new stdClass;
  // We show the cart only if the setting is enabled.
  $sms_vote_settings = sms_vote_get_settings();
  if (sms_vote_is_cart_enabled()) {
    module_load_include('inc', 'sms_vote', 'sms_vote.cart');
    $storage = SMSVoteTmpStorageFactory::getStorage();
    $all_points = $storage->getAllPoints();
    // We need this div to appear all the time.
    $block->content['#markup'] = '<div class="sms-cart-form-wrapper">';
    if (!empty($all_points)) {
      $form = drupal_get_form('sms_vote_voting_cart');
      $block->content['#markup'] .= render($form);
    }
    $block->content['#markup'] .= '</div>';
  }
  return $block;
}

/**
 * Configuration form, not needed at the moment.
 */
function sms_vote_cart_render_config_form($form, &$form_state) {
  return $form;
}
