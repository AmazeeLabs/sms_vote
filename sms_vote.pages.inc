<?php

/**
 * @file
 *  Page callbacks for the SMS Vote module.
 */

/**
 * Page callback for the sms vote page.
 */
function sms_vote_vote_page($node, $js = NULL, $step = NULL) {
  if (!$js) {
    // @todo: We have to handle this nicer.
    return drupal_get_form('sms_vote_vote_form', array($node));
  }
  ctools_include('modal');
  ctools_include('ajax');
  $form_info = array(
    'id' => 'sms_vote',
    'path' => "sms_vote/" . $node->nid . "/" . ($js ? 'ajax' : 'nojs') . "/%step",
    'show cancel' => TRUE,
    'next callback' =>  'sms_vote_next_process',
    'order' => array(
      'vote' => t('Vote'),
      'enter_phone' => t('Enter phone number'),
      'confirm_code' => t('Confirm Code'),
    ),
    'forms' => array(
      'vote' => array(
        'form id' => 'sms_vote_vote_form',
      ),
      'enter_phone' => array(
        'form id' => 'sms_vote_checkout_form',
        'include' => array(drupal_get_path('module', 'sms_vote') . '/sms_vote.checkout.inc')
      ),
      'confirm_code' => array(
        'form id' => 'sms_vote_checkout_form',
        'include' => array(drupal_get_path('module', 'sms_vote') . '/sms_vote.checkout.inc')
      ),
    ),
  );
  $form_state = array(
    'ajax' => $js,
    'build_info' => array(
      'args' => array(0 => array($node)),
    )
  );
  if (empty($step)) {
    $step = 'vote';
    $sms_vote_settings = sms_vote_get_settings();
    // If we are on the vote step and the maximum points we can give is 1, and
    // we do not have the cart enabled, the we just skip this step.
    // @todo: If it would make sense to have a different value than '1' point
    // then we can also do that with a setting. It would be something like:
    // "Give all the points to one vote".
    if (!sms_vote_is_cart_enabled() && $sms_vote_settings['max_points'] == 1) {
      // The next step is the one where we enter the phone.
      $step = 'enter_phone';
      $storage = SMSVoteTmpStorageFactory::getStorage();
      // Make sure we have no other votes in the storage.
      $storage->clearStorage();
      // And store all the points.
      $storage->storePoints($node->nid, $sms_vote_settings['max_points']);
    }
  }
  ctools_include('wizard');
  $form = ctools_wizard_multistep_form($form_info, $step, $form_state);

  $commands = array();
  if (!empty($form_state['dismiss_and_update_cart'])) {
    $commands += sms_vote_commands_dismiss_modal_and_show_messages();
    module_load_include('inc', 'sms_vote', 'sms_vote.cart');
    $cart_form = drupal_get_form('sms_vote_voting_cart');
    $commands[] = ajax_command_html('.sms-cart-form-wrapper', drupal_render($cart_form));
  }
  elseif (!empty($form_state['cancel']) || !empty($form_state['complete'])) {
    // Dismiss the modal and show the messages on the site.
    $commands += sms_vote_commands_dismiss_modal_and_show_messages();
    // Even that we should not have this case happening, because if we reach to
    // the complete state in this wizard this means we do not use a cart, we
    // still remove any cart contents (which is in the .voting block).
    if (!empty($form_state['complete'])) {
      $commands[] = ajax_command_remove('.voting-block');
    }
  }
  else {
    $output = drupal_render($form);
    $commands = ctools_modal_form_render($form_state, $output);
  }
  print ajax_render($commands);
}

/**
 * The 'next callback' in a ctools wizard form.
 */
function sms_vote_next_process(&$form_state) {
  // When clicking on the 'Continue' on a vote form, and we have the cart
  // enabled then we want to dismiss the modal window and update the cart.
  $sms_vote_settings = sms_vote_get_settings();
  if (sms_vote_is_cart_enabled()) {
    $form_state['dismiss_and_update_cart'] = TRUE;
  }
}

/**
 * Page callback to cancel a vote.
 */
function sms_vote_vote_cancel_page($node, $js = NULL) {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $storage->removePoints($node->nid);
  if (!$js) {
    drupal_goto('node/' . $node->nid);
  }
  ctools_include('modal');

  $output = array();
  // Make sure we dismiss any modal window.
  $output[] = ctools_modal_command_dismiss();

  // Update the voting cart, if we use the cart.
  if (sms_vote_is_cart_enabled()) {
    $points = $storage->getAllPoints();
    if (!empty($points)) {
      module_load_include('inc', 'sms_vote', 'sms_vote.cart');
      $form = drupal_get_form('sms_vote_voting_cart');
      $output[] = ajax_command_html('.sms-cart-form-wrapper', drupal_render($form));
    }
    else {
      $output[] = ajax_command_html('.sms-cart-form-wrapper', '');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for the vote form.
 */
function sms_vote_vote_form($form, &$form_state, $nodes) {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $vote_widget = sms_vote_widget($nodes);
  $form += $vote_widget;
  // Make sure this file is included in ajax requests.
  form_load_include($form_state, 'inc', 'sms_vote', 'sms_vote.pages');
  if (!empty($vote_widget)) {
    // If we have anything in the form, we show the submit button, otherwise we
    // just show a message that the user cannot vote anymore.
    $submit_title = t('Vote');
    foreach ($nodes as $node) {
      if ($storage->getPoints($node->nid)) {
        $submit_title = t('Update vote');
        break;
      }
    }
    $form['buttons']['next']['#value'] = $submit_title;
  }
  else {
    unset($form['buttons']['next']);
    $available_points = $storage->getAvailablePoints();
    if (!$available_points) {
      $form['no_votes'] = array('#markup' => '<div>' . t('You cannot vote anymore because you used all your available points. Please use the cart to change the points distribution or to submit the votes.') . '</div>');
    }
    else {
      // We should not be here, if we do there was something we could not handle
      // it.
      $form['no_votes'] = array('#markup' => '<div>' . t('You cannot vote anymore because of some error. Please contact our site team.') . '</div>');
    }
  }
  return $form;
}

/**
 * Submit handler for the voting form, or any form that contain the
 * sms_vote_widget.
 *
 * Important is that this has to be specifically added as a submit handler. It
 * is added automatically only for the sms_vote_vote_form() form.
 */
function sms_vote_vote_form_submit(&$form, &$form_state) {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  // If we do not use a cart, then we clear the storage.
  if (!sms_vote_is_cart_enabled()) {
    $storage->clearStorage();
  }
  foreach ($form_state['values']['nids'] as $nid) {
    $storage->storePoints($nid, $form_state['values']['vote_' . $nid]);
  }
}
