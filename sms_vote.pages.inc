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
    'finish callback' => 'sms_vote_finish_process',
    'cancel callback' => 'sms_vote_cancel_process',
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
  if (empty($step)) {
    $step = 'vote';
  }
  $form_state = array(
    'ajax' => $js,
    'build_info' => array(
      'args' => array(0 => array($node)),
    )
  );
  ctools_include('wizard');
  $form = ctools_wizard_multistep_form($form_info, $step, $form_state);

  $commands = array();
  if (!empty($form_state['dismiss_and_update_cart'])) {
    $commands[] = ctools_modal_command_dismiss();
    $cart_form = drupal_get_form('sms_vote_voting_cart');
    $commands[] = ajax_command_html('.sms-cart-form-wrapper', drupal_render($cart_form));
  }
  elseif (!empty($form_state['cancel']) || !empty($form_state['complete'])) {
    // Dismiss the modal and show the messages on the site.
    $commands[] = ctools_modal_command_dismiss();
    $commands[] = ajax_command_remove('div.messages');
    $commands[] = ajax_command_prepend('div.main-content,div.pane-page-content', theme('status_messages'));
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
 * Flags the voting (checkout) process as finished.
 * It is a 'finish callback' in a ctools wizard form.
 */
function sms_vote_finish_process(&$form_state) {
  $form_state['complete'] = TRUE;
}

/**
 * Flags the voting (checkout) process as canceled.
 * It is a 'cancel callback' in a ctools wizard form.
 */
function sms_vote_cancel_process(&$form_state) {
  // @todo: delete the votes for the node.
  $form_state['cancel'] = TRUE;
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

  // Update the voting cart.
  $points = $storage->getAllPoints();
  if (!empty($points)) {
    $form = drupal_get_form('sms_vote_voting_cart');
    $output[] = ajax_command_html('.sms-cart-form-wrapper', drupal_render($form));
  }
  else {
    $output[] = ajax_command_html('.sms-cart-form-wrapper', '');
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
 * Submit handler for the voting form.
 */
function sms_vote_vote_form_submit(&$form, &$form_state) {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  foreach ($form_state['values']['nids'] as $nid) {
    $storage->storePoints($nid, $form_state['values']['vote_' . $nid]);
  }
}

/**
 * Form constructor for the voting cart.
 */
function sms_vote_voting_cart($form, &$form_state) {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $points = $storage->getAllPoints();
  $form = array();
  // Make sure this file is included in ajax requests.
  form_load_include($form_state, 'inc', 'sms_vote', 'sms_vote.pages');
  if (!empty($points)) {
    $nodes = array();
    foreach ($points as $nid => $value) {
      $nodes[] = node_load($nid);
    }
    $form = sms_vote_widget($nodes);
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
    // If we do not have any available points, then show the "Submit vote"
    // button so that the user can finish the vote process.
    if (!empty($available_points)) {
      $form['available_points'] = array(
        '#markup' => '<div id="available-points">' . t('Available points to give: @number', array('@number' => format_plural($available_points, '@count point', '@count points'))) . '</div>',
      );
    }
    // We show the submit votes button if we do not have any points to give, or
    // if the distribution of all points is not required.
    $sms_vote_settings = sms_vote_get_settings();
    if (empty($available_points) || empty($sms_vote_settings['force_distribute_all'])) {
      $form['submit_votes'] = array(
        '#type' => 'submit',
        '#value' => t('Submit votes'),
        '#ajax' => array(
          'callback' => 'sms_vote_cart_submit_votes_ajax_submit',
        ),
        '#submit' => array('sms_vote_cart_submit_votes_submit'),
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

/**
 * Submit handler for the 'Submit votes' button.
 */
function sms_vote_cart_submit_votes_submit($form, &$form_state) {
  // Just redirect the user to the checkout page.
  $form_state['redirect'] = 'sms_vote/checkout';
}

/**
 * Ajax submit handler for the the 'Submit votes' button.
 */
function sms_vote_cart_submit_votes_ajax_submit($form, &$form_state) {
  $commands = array();
  $checkout_review = theme('sms_vote_checkout_review');
  $checkout_form = drupal_get_form('sms_vote_checkout_form');
  $commands[] = ajax_command_replace('#sms-vote-voting-cart', $checkout_review . drupal_render($checkout_form));
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Vote widget for a set of nodes that can be placed on a form.
 *
 * @param array $nodes
 *  An array with node objects.
 */
function sms_vote_widget($nodes) {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $remove_points_title = t('Remove points');
  $form = array();
  ctools_include('ajax');
  foreach ($nodes as $node) {
    $max_points_for_node = $storage->getAvailablePointsForNode($node->nid);
    // We show the widget only if we can add points for the node.
    if (!empty($max_points_for_node)) {
      $nids[] = $node->nid;
      $current_points = $storage->getPoints($node->nid);
      $form['vote_' . $node->nid] = array(
        '#type' => 'select',
        '#id' => 'vote-widget-' . time() . rand(1000, 99999),
        '#options' => drupal_map_assoc(range(1, $max_points_for_node)),
        '#title' => l($node->title, 'node/' . $node->nid, array('html' => TRUE)),
        '#default_value' => !empty($current_points) ? $current_points : 1,
        '#prefix' => !empty($current_points) ? '<div class="voting-choice">' : '',
        '#suffix' => !empty($current_points) ? '<div class="cancel-vote">' . ctools_ajax_text_button($remove_points_title, 'sms_vote/cancel/' . $node->nid . '/nojs', $remove_points_title) . '</div></div>' : ''
      );
    }
  }
  if (!empty($nids)) {
    $form['nids'] = array(
      '#type' => 'value',
      '#value' => $nids,
    );
  }
  return $form;
}
