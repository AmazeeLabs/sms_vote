<?php

/**
 * @file
 *  Page callbacks for the SMS Vote module.
 */

/**
 * Page callback for the sms vote page.
 */
function sms_vote_vote_page($node, $js = NULL) {
  if (!$js) {
    return drupal_get_form('sms_vote_vote_form', array($node));
  }
  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Vote'),
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array(0 => array($node)),
    )
  );
  $output = ctools_modal_form_wrapper('sms_vote_vote_form', $form_state);
  if (!empty($form_state['executed'])) {
    // After we voted, we just dismiss the modal and update the cart.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    $form = drupal_get_form('sms_vote_voting_cart');
    $output[] = ajax_command_html('.sms-cart-form-wrapper', drupal_render($form));
  }
  print ajax_render($output);
  exit;
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
  $form = sms_vote_widget($nodes);
  // Make sure this file is included in ajax requests.
  form_load_include($form_state, 'inc', 'sms_vote', 'sms_vote.pages');
  // If we have anything in the form, we show the submit button, otherwise we
  // just show a message that the user cannot vote anymore.
  $submit_title = t('Vote');
  foreach ($nodes as $node) {
    if ($storage->getPoints($node->nid)) {
      $submit_title = t('Update vote');
      break;
    }
  }
  if (!empty($form)) {
    $form['submit_vote'] = array(
      '#type' => 'submit',
      '#value' => $submit_title,
    );
  }
  else {
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
    if (!$available_points) {
      $form['submit_votes'] = array(
        '#type' => 'submit',
        '#value' => t('Submit votes'),
        '#ajax' => array(
          'callback' => 'sms_vote_cart_submit_votes_ajax_submit',
        ),
        '#submit' => array('sms_vote_cart_submit_votes_submit'),
      );
    }
    else {
      $form['available_points'] = array(
        '#markup' => '<div id="available-points">' . t('Available points to give: @number', array('@number' => format_plural($available_points, '@count point', '@count points'))) . '</div>',
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
  foreach ($nodes as $node) {
    $max_points_for_node = $storage->getAvailablePointsForNode($node->nid);
    // We show the widget only if we can add points for the node.
    if (!empty($max_points_for_node)) {
      $lang_code = field_language('node', $node, 'field_song');
      $nids[] = $node->nid;
      $current_points = $storage->getPoints($node->nid);
      $form['vote_' . $node->nid] = array(
        '#type' => 'select',
        '#id' => 'vote-widget-' . time() . rand(1000, 99999),
        '#options' => drupal_map_assoc(range(1, $max_points_for_node)),
        '#title' => l(t('@song <span>by @band</span>', array('@song' => $node->field_song[$lang_code][0]['value'], '@band' => $node->title)), 'node/' . $node->nid, array('html' => true)),
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

/**
 * Checkout page for votes.
 */
function sms_vote_checkout_page() {
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $available_points = $storage->getAvailablePoints();
  // If we have any points left, redirect to the frontpage with a message that
  // we must give all the points.
  if (!empty($available_points)) {
    drupal_set_message(t('You still have @number points to give before you can checkout.', array('@number' => $available_points)), 'warning');
    drupal_goto('<front>');
  }
  return array(
    'checkout_review' => array(
      '#theme' => 'sms_vote_checkout_review',
    ),
    'checkout_form' => drupal_get_form('sms_vote_checkout_form')
  );
}

/**
 * Form constructor for the votes checkout form.
 */
function sms_vote_checkout_form($form, &$form_state) {
  // Make sure this file is included in ajax requests.
  form_load_include($form_state, 'inc', 'sms_vote', 'sms_vote.pages');
  $form['phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone'),
    '#description' => t('Please format the phone either like 07xxxxxxxx or using the 10+ digits international format, without any separators between digits and starting with "+" (for example +417xxxxxxxx)'),
    '#required' => TRUE,
  );
  if (!empty($form_state['svid'])) {
    $form['resend'] = array(
      '#type' => 'submit',
      '#value' => t('Resend code'),
      // When resending the code, we want just the phone to be validated.
      '#limit_validation_errors' => array(array('phone')),
      '#submit' => array('sms_vote_checkout_resend_submit'),
      '#ajax' => array(
        'callback' => 'sms_vote_checkout_form_ajax_submit',
        'wrapper' => 'sms-vote-checkout-form',
      ),
      '#attributes' => array(
        'class' => array('resend-code')
      )
    );
    $form['code'] = array(
      '#type' => 'textfield',
      '#title' => t('Code'),
      '#required' => TRUE,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#ajax' => array(
      'callback' => 'sms_vote_checkout_form_ajax_submit',
      'wrapper' => 'sms-vote-checkout-form',
    )
  );
  $form['#id'] = 'sms-vote-checkout-form';
  return $form;
}

/**
 * Validate handler for the checkout form.
 */
function sms_vote_checkout_form_validate(&$form, &$form_state) {
  // Check if we still have points to give.
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $available_points = $storage->getAvailablePoints();
  if (!empty($available_points)) {
    form_set_error('', t('You still have @number points to give before you can checkout.', array('@number' => $available_points)));
    // No need to continue with any further validation as the user has to go
    // first anyway to vote again.
    return FALSE;
  }

  // Make sure that the telephone number is unique.
  $svid = 0;
  if (!empty($form_state['svid'])) {
    $svid = $form_state['svid'];
    $vote_entity = entity_load_single('sms_vote', $svid);
    $max_validation_fails = variable_get('sms_vote_max_fails', 3);
    // If the user exceded the maximum number of failures, stop here.
    if ($vote_entity->validation_fails >= $max_validation_fails) {
      form_set_error('code', t('You entered a wrong code too many times. Please contact our team for further instructions.'));
      return FALSE;
    }
  }
  // We work only with normalized phone numbers. We always use the international
  // 10+ digits version of the phone number.
  $phone = sms_vote_normalize_phone($form_state['values']['phone']);
  if (empty($phone)) {
    form_set_error('phone', t('The phone number is not valid.'));
  }
  else {
    form_set_value($form['phone'], $phone, $form_state);
    // We validate the phone to be unique only if it is not a special one,
    // because that one can be used multiple times.
    if (!sms_vote_is_special_number_phone($phone)) {
      $query = db_select('sms_vote', 'sv')->fields('sv', array('svid', 'phone', 'status'));
      $query->condition('sv.phone', $phone)->condition('sv.svid', $svid, '<>')->condition('sv.status', '1');
      $result = $query->execute()->fetch();
      if (!empty($result)) {
        form_set_error('phone', t('This phone number was already used for voting.'));
        // @todo: we need to handle the case when the user wants to resend the code
        // here.
      }
    }
    // If we have an svid, we must check the code.
    if (!empty($svid)) {
      $vote_entity = entity_load_single('sms_vote', $svid);
      $valid_codes = sms_vote_get_valid_codes_for_vote($vote_entity);
      if (empty($valid_codes) || !in_array($form_state['values']['code'], $valid_codes)) {
        form_set_error('code', t('The code you entered is incorrect.'));
        // Update the validation fails time.
        $vote_entity->validation_fails++;
        entity_save('sms_vote', $vote_entity);
      }
    }
  }
}

/**
 * Submit handler for the checkout form.
 */
function sms_vote_checkout_form_submit(&$form, &$form_state) {
  if (empty($form_state['svid'])) {
    // This is the case when we save the votes into the database and (maybe also
    // clear the cache) send the SMS with the code.
    $vote_entity = entity_create('sms_vote', array('phone' => $form_state['values']['phone'], 'status' => 0, 'code' => SMSVotePredefinedCodes::generateCode()));
    $saved = entity_save('sms_vote', $vote_entity);
    if ($saved !== FALSE) {
      $form_state['svid'] = $vote_entity->svid;
    }

    // Send the SMS with the code, but only if the phone is not the special one
    // for which we need to use a predefined code.
    if (!sms_vote_is_special_number_phone($vote_entity->phone)) {
      $sms_sender = SMSVoteSenderFactory::getSender();
      $sms_sender->send($vote_entity->phone, sms_vote_format_sms_message($vote_entity->code));
      drupal_set_message(t('An SMS has been sent to you with a confirmation code. Please enter it to submit the votes.'));
      watchdog('sms_vote_send_code', 'A code has been sent to the phone number "@phone" with the code "@code"', array('@phone' => $vote_entity->phone, '@code' => $vote_entity->code));
    }
    else {
      drupal_set_message(t('Please enter the code that you received from our site team.'));
    }
    $form_state['rebuild'] = TRUE;
  }
  else {
    $vote_entity = entity_load_single('sms_vote', $form_state['svid']);
    if (!empty($vote_entity)) {
      $storage = SMSVoteTmpStorageFactory::getStorage();
      $votes = $storage->getAllPoints();
      // Save now also the vote points entities.
      foreach ($votes as $nid => $points) {
        $vote_points_entity = entity_create('sms_vote_points', array('svid' => $form_state['svid'], 'nid' => $nid, 'points' => $points));
        entity_save('sms_vote_points', $vote_points_entity);
      }
      // Update the entity status.
      $vote_entity->status = 1;
      // In case we are here and the submitted code is different than the vote
      // entity code, this means a special code may be used, so we delete the
      // code from the predefined codes table.
      // We put this condition to avoid the case when a code for a vote entity
      // is the same as one of the predefined codes, and this would result in
      // deleting the predefined code when it was actually not used on purpose.
      if ($vote_entity->code != $form_state['values']['code']) {
        SMSVotePredefinedCodes::deletePredefinedCode($form_state['values']['code']);
      }
      // Update also the code used with for this vote.
      $vote_entity->code = $form_state['values']['code'];
      $vote_entity->confirmed = REQUEST_TIME;
      entity_save('sms_vote', $vote_entity);
      drupal_set_message(t('The votes have been saved.'));
      // Clear everything from the temporary storage.
      $storage->clearStorage();
      $form_state['redirect'] = '<front>';
      $form_state['finished'] = TRUE;
    }
    // @todo: maybe we should raise an exception if the entity could not be
    // loaded.
  }
}

/**
 * Ajax callback for the submit button on the checkout form.
 */
function sms_vote_checkout_form_ajax_submit($form, $form_state) {
  if (!empty($form_state['finished'])) {
    ctools_include('ajax');
    $output = array();
    // Remove the voting block and show the messages.
    $output[] = ajax_command_remove('.voting-block');
    // Remove also any messages.
    $output[] = ajax_command_remove('div.messages');
    $output[] = ajax_command_prepend('div.main-content', theme('status_messages'));
    $output[] = ajax_command_invoke('html', 'trigger', array('SMSVote_success'));

    print ajax_render($output);
    exit();
  }
  return drupal_rebuild_form('sms_vote_checkout_form', $form_state, $form);
}

/**
 * Submit handler for the resend button on the checkout form.
 */
function sms_vote_checkout_resend_submit(&$form, &$form_state) {
  $vote_entity = entity_load_single('sms_vote', $form_state['svid']);
  if (!empty($vote_entity) && !sms_vote_is_special_number_phone($form_state['values']['phone'])) {
    $vote_entity->phone = $form_state['values']['phone'];
    entity_save('sms_vote', $vote_entity);

    // Send the sms again.
    $sms_sender = SMSVoteSenderFactory::getSender();
    $sms_sender->send($vote_entity->phone, sms_vote_format_sms_message($vote_entity->code));
    drupal_set_message(t('The confirmation code has been resent.'));
    // Log the event.
    watchdog('sms_vote_resend_code', 'A code has been resent to the phone number "@phone" with the code "@code"', array('@phone' => $vote_entity->phone, '@code' => $vote_entity->code));
  }
  elseif (sms_vote_is_special_number_phone($form_state['values']['phone'])) {
    drupal_set_message(t('Please contact our site team to get a new code.'));
  }
  $form_state['rebuild'] = TRUE;
}
