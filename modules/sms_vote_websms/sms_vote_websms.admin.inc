<?php

/**
 * @file
 *  Admin menu callbacks for the web sms module.
 */

/**
 * Form constructor for the web sms settings.
 */
function sms_vote_websms_admin() {
  $form['sms_vote_websms_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Web SMS username'),
    '#default_value' => variable_get('sms_vote_websms_username', ''),
    '#required' => TRUE,
  );
  $form['sms_vote_websms_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Web SMS password'),
    '#default_value' => variable_get('sms_vote_websms_password', ''),
    '#required' => TRUE,
  );
  $form['sms_vote_websms_gateway_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Web SMS gateway'),
    '#default_value' => variable_get('sms_vote_websms_gateway_url', 'https://api.websms.com/'),
    '#required' => TRUE,
  );
  $form['sms_vote_websms_testmode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Test mode'),
    '#description' => t('If checked, no sms will be actually sent.'),
    '#default_value' => variable_get('sms_vote_websms_testmode', TRUE),
  );
  $form['sms_vote_websms_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),
    '#description' => t('If checked, all success SMS send attempts will be recorded to the watchdog.'),
    '#default_value' => variable_get('sms_vote_websms_debug', FALSE),
  );
  return system_settings_form($form);
}
