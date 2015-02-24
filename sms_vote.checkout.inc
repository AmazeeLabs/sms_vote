<?php

/**
 * @file
 *  Contains the checkout related functions.
 */

/**
 * Checkout page for votes.
 */
function sms_vote_checkout_page($js = NULL, $step = NULL) {
  // Make sure first that we gave all the points if required.
  $sms_vote_settings = sms_vote_get_settings();
  if (!empty($sms_vote_settings['force_distribute_all'])) {
    $storage = SMSVoteTmpStorageFactory::getStorage();
    $available_points = $storage->getAvailablePoints();
    // If we have any points left, redirect to the frontpage with a message that
    // we must distribute all the points.
    if (!empty($available_points)) {
      drupal_set_message(t('You still have @number points to give before you can checkout.', array('@number' => $available_points)), 'warning');
      if (!$js) {
        drupal_goto('<front>');
      }
      else {
        $commands = sms_vote_commands_dismiss_modal_and_show_messages();
        print ajax_render($commands);
      }
      return;
    }
  }
  // If we are here, we can show the checkout form.
  $form_info = array(
    'id' => 'sms_vote_checkout',
    'path' => "sms_vote/checkout/" . ($js ? 'ajax' : 'nojs') . "/%step",
    'show cancel' => TRUE,
    'order' => array(
      'enter_phone' => t('Enter phone number'),
      'confirm_code' => t('Confirm Code'),
    ),
    'forms' => array(
      'enter_phone' => array(
        'form id' => 'sms_vote_checkout_form',
      ),
      'confirm_code' => array(
        'form id' => 'sms_vote_checkout_form',
      ),
    ),
  );

  if (empty($step)) {
    $step = 'enter_phone';
  }
  $form_state = array(
    'ajax' => $js,
  );
  ctools_include('wizard');
  $form = ctools_wizard_multistep_form($form_info, $step, $form_state);
  if (!$js) {
    if (!empty($form_state['cancel']) || !empty($form_state['complete'])) {
      drupal_goto();
    }
    else {
      return $form;
    }
  }
  else {
    ctools_include('modal');
    ctools_include('ajax');
    // If we cancel or complete the process, we just dismiss the modal and show
    // the messages.
    if (!empty($form_state['cancel']) || !empty($form_state['complete'])) {
      // Dismiss the modal and show the messages on the site.
      $commands = sms_vote_commands_dismiss_modal_and_show_messages();
      if (!empty($form_state['complete'])) {
        $commands[] = ajax_command_remove('.voting-block');
      }
    }
    else {
      // Otherwise, we just render the form.
      $output = drupal_render($form);
      $commands = ctools_modal_form_render($form_state, $output);
    }
    print ajax_render($commands);
  }
}

/**
 * Form constructor for the votes checkout form.
 */
function sms_vote_checkout_form($form, &$form_state) {
  // Make sure this file is included in ajax requests.
  form_load_include($form_state, 'inc', 'sms_vote', 'sms_vote.checkout');
  $form['phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone'),
    '#description' => t('Please format the phone either like 07xxxxxxxx or using the 10+ digits international format, without any separators between digits and starting with "+" (for example +417xxxxxxxx)'),
    '#required' => TRUE,
  );
  $storage = SMSVoteTmpStorageFactory::getStorage();
  $svid = $storage->getProperty('svid');
  if (!empty($svid)) {
    $form_state['svid'] = $svid;
    $vote_entity = entity_load_single('sms_vote', $form_state['svid']);
    $form['phone']['#default_value'] = $vote_entity->phone;
    $form['resend'] = array(
      '#type' => 'submit',
      '#value' => t('Resend code'),
      // When resending the code, we want just the phone to be validated.
      '#limit_validation_errors' => array(array('phone')),
      '#submit' => array('sms_vote_checkout_resend_submit'),
      '#attributes' => array(
        'class' => array('resend-code')
      )
    );
    $form['code'] = array(
      '#type' => 'textfield',
      '#title' => t('Code'),
      '#required' => TRUE,
    );
    $form['buttons']['return']['#value'] = t('Submit code');
  }
  else {
    $form['buttons']['next']['#value'] = t('Send SMS');
  }
  // If we have the cart enabled, then we show a review of the votes.
  if (sms_vote_is_cart_enabled()) {
    $form['#prefix'] = theme('sms_vote_checkout_review');
  }
  $form['#id'] = 'sms-vote-checkout-form';
  return $form;
}

/**
 * Validate handler for the checkout form.
 */
function sms_vote_checkout_form_validate(&$form, &$form_state) {
  // Check if we still have points to give in case we must distribute them all.
  $sms_vote_settings = sms_vote_get_settings();
  if (!empty($sms_vote_settings['force_distribute_all'])) {
    $storage = SMSVoteTmpStorageFactory::getStorage();
    $available_points = $storage->getAvailablePoints();
    if (!empty($available_points)) {
      form_set_error('', t('You still have @number points to give before you can checkout.', array('@number' => $available_points)));
      // No need to continue with any further validation as the user has to go
      // first anyway to vote again.
      return FALSE;
    }
  }

  // Check if the user tried to enter a wrong code too many times.
  $svid = 0;
  if (!empty($form_state['svid']) && $form_state['values']['op'] != $form_state['values']['resend']) {
    $svid = $form_state['svid'];
    $vote_entity = entity_load_single('sms_vote', $svid);
    $max_validation_fails = $sms_vote_settings['max_fails'];
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
    // If we have an svid, we must check the code, but only if we do not resend
    // the code.
    if (!empty($svid) && $form_state['values']['op'] != $form_state['values']['resend']) {
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
    global $language;
    // This is the case when we save the votes into the database and (maybe also
    // clear the cache) send the SMS with the code.
    // @todo: we should make sure here that the generated code is not a
    // predefined code. This can generate, in an edge case, a code not being
    // deleted from the database. Below we will make a check when we delete a
    // predefined code that the code is different than the one generated for the
    // vote. But, in case the generated code is the same, then the predefined
    // code will not be deleted (this should have a very low percentage of
    // happening, but it could). It will not harm the system, the only problem
    // will be that the predefined code will not be deleted, so it can be used
    // again by another person, if she knows it.
    $vote_entity = entity_create('sms_vote', array('phone' => $form_state['values']['phone'], 'status' => 0, 'created' => REQUEST_TIME, 'language' => $language->language, 'code' => SMSVotePredefinedCodes::generateCode()));
    $saved = entity_save('sms_vote', $vote_entity);
    if ($saved !== FALSE) {
      $form_state['svid'] = $vote_entity->svid;
      $storage = SMSVoteTmpStorageFactory::getStorage();
      $storage->storeProperty('svid', $vote_entity->svid);
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
