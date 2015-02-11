<?php

/**
 * @file
 *  Contains the cart related functions.
 */

/**
 * Form constructor for the voting cart.
 */
function sms_vote_voting_cart($form, &$form_state) {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $points = $storage->getAllPoints();
  $form = array();
  // Make sure this file is included in ajax requests.
  form_load_include($form_state, 'inc', 'sms_vote', 'sms_vote.cart');
  form_load_include($form_state, 'inc', 'sms_vote', 'sms_vote.pages');
  if (!empty($points)) {
    $nodes = array();
    foreach ($points as $nid => $value) {
      $nodes[] = node_load($nid);
    }
    $form += sms_vote_widget($nodes);
    // Attach the ajax behaviors on the select elements.
    foreach ($form['nids']['#value'] as $nid) {
      $form['vote_' . $nid]['#ajax'] = array(
        'callback' => 'sms_vote_cart_form_ajax_submit',
        'wrapper' => 'sms-vote-voting-cart',
      );
    }
    $form['update'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#name' => 'update',
      // The button should be hidden if we have js. If it is not hidden, then
      // it should submit using the same handler as the single vote form.
      '#submit' => array('sms_vote_vote_form_submit'),
    );
    $available_points = $storage->getAvailablePoints();
    // Show the available points to give if any.
    if (!empty($available_points)) {
      $form['available_points'] = array(
        '#markup' => '<div id="available-points">' . t('Available points to give: @number', array('@number' => format_plural($available_points, '@count point', '@count points'))) . '</div>',
      );
    }
    // We show the submit votes button if we do not have any points to give, or
    // if the distribution of all points is not required.
    $sms_vote_settings = sms_vote_get_settings();
    if (empty($available_points) || empty($sms_vote_settings['force_distribute_all'])) {
      ctools_include('ajax');
      ctools_include('modal');
      ctools_modal_add_js();
      $submit_title = t('Submit votes');
      $form['submit_votes'] = array(
        '#type' => 'item',
        '#markup' => ctools_modal_text_button($submit_title, 'sms_vote/checkout/nojs', $submit_title, 'ctools-modal-modal-popup-small'),
      );
    }
  }
  $form['#id'] = 'sms-vote-voting-cart';
  $form['#prefix'] = '<div class="voting-block"><h2>' . t('Your votes') . '</h2>';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * Ajax submit handler for the cart form..
 */
function sms_vote_cart_form_ajax_submit($form, $form_state) {
  sms_vote_vote_form_submit($form, $form_state);
  $rebuild_form = drupal_rebuild_form('sms_vote_voting_cart', $form_state, $form);
  // We do not need the prefix and the suffix now.
  unset($rebuild_form['#prefix'], $rebuild_form['#suffix']);
  return $rebuild_form;
}

