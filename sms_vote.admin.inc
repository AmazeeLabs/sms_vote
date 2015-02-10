<?php

/**
 * @file
 *  Admin menu callbacks for the SMS Vote module.
 */

/**
 * Form constructor for the voting settings form.
 */
function sms_vote_admin($form, &$form_state) {
  $form['sms_vote_max_points'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum points'),
    '#description' => t('The maximum number of points a user can give for a vote. This is also the exact number of points that an user must distribute among one or more bands.'),
    '#default_value' => variable_get('sms_vote_max_points', 4),
  );
  $form['sms_vote_code_digits'] = array(
    '#type' => 'textfield',
    '#title' => t('Code digits number'),
    '#description' => t('How many digits should the confirmation code have.'),
    '#default_value' => variable_get('sms_vote_code_digits', 4),
  );
  $form['sms_vote_max_fails'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum fails'),
    '#description' => t('The maximum number of times the user can input an incorrect code.'),
    '#default_value' => variable_get('sms_vote_max_fails', 3),
  );
  $form['sms_vote_special_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone number for predefined codes'),
    '#description' => t('The special telephone number that can be used with the predefined codes.'),
    '#default_value' => variable_get('sms_vote_special_number', ''),
  );
  return system_settings_form($form);
}

/**
 * Validate handler for the voting settings form.
 */
function sms_vote_admin_validate(&$form, &$form_state) {
  sms_vote_admin_check_numeric_and_positive('sms_vote_max_points', t('The maximum number of points has to be numeric and greater than 1.'), $form_state);
  sms_vote_admin_check_numeric_and_positive('sms_vote_max_fails', t('The maximum number of fails has to be numeric and greater than 1.'), $form_state);
  sms_vote_admin_check_numeric_and_positive('sms_vote_code_digits', t('The number of digits has to be numeric and greater than 1.'), $form_state);
}

/**
 * Helper validation function that checks if a value is numeric and bigger than
 * 1, and if yes it makes sure it is an integer.
 */
function sms_vote_admin_check_numeric_and_positive($error_field, $error_message, &$form_state) {
  if (!is_numeric($form_state['values'][$error_field]) || $form_state['values'][$error_field] < 1) {
    form_set_error($error_field, $error_message);
  }
  else {
    $form_state['values'][$error_field] = (int)$form_state['values'][$error_field];
  }
}

/**
 * Updates the status of a code.
 */
function sms_vote_code_status_change($status, $code, $js = NULL) {
  // The status can be only 'available' or 'reserve';
  if ($status != 'available' && $status != 'reserve') {
    return MENU_NOT_FOUND;
  }
  if ($status == 'available') {
    $code->reserved = 0;
  }
  else {
    $code->reserved = 1;
  }
  drupal_write_record('sms_vote_predefined_codes', $code, array('cid'));
  if (!$js) {
    drupal_goto('admin/config/system/sms_vote/codes');
  }
  // Replace the status text and link for the code.
  // Something is not working correct here, so at the moment we do not use ajax
  // on these links.
  $commands = array();
  //$commands[] = ajax_command_replace('#code-status-' .$code->cid, _sms_vote_get_reserved_text($code));
  //$commands[] = ajax_command_replace('#link-status-change-' .$code->cid, _sms_vote_get_status_change_link($code));
  ajax_deliver(array('#type' => 'ajax', '#commands' => $commands));
  exit();
}

/**
 * Menu callback for the predefined codes page.
 */
function sms_vote_admin_predefined_codes() {
  $codes = sms_vote_get_predefined_codes();
  $header = array(t('Code'), t('Status'), t('Actions'));
  $rows = array();
  foreach ($codes as $code) {
    $row = array();
    $color = !empty($code->reserved) ? 'red' : 'green';
    $row['code'] = '<span style="color:' . $color . '">' . $code->code . '</span>';
    $row['reserved'] = _sms_vote_get_reserved_text($code);
    $actions = array();
    $actions[] = _sms_vote_get_status_change_link($code);
    $row['actions'] = implode('|', $actions);
    $rows[] = $row;
  }
  return array(
    'predefined_codes' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#caption' => t('Predefined codes'),
    ),
    'generate_codes_form' => drupal_get_form('sms_vote_admin_generate_codes_form'),
  );
}

/**
 * Helpder function to return the link for changing the status of a code.
 *
 * @param object $code
 *  A code object that maps over the sms_vote_predefine_codes table.
 */
function _sms_vote_get_status_change_link($code) {
  if (!empty($code->reserved)) {
    return l(t('Make available'), 'sms_vote/code/status/available/' . $code->cid, array('attributes' => array('id' => 'link-status-change-' . $code->cid)));
  }
  else {
    return l(t('Reserve'), 'sms_vote/code/status/reserve/' . $code->cid, array('attributes' => array('id' => 'link-status-change-' . $code->cid)));
  }
}

/**
 * Helper function to return the status of a code as text (Reserved / Available)
 *
 * @param object $code
 *  A code object that maps over the sms_vote_predefine_codes table.
 */
function _sms_vote_get_reserved_text($code) {
  $reserved = !empty($code->reserved) ? t('Reserved') : t('Available');
  $color = !empty($code->reserved) ? 'red' : 'green';
  // Wrapp it with an id so that we can use the ajax to replace the value.
  $reserved = '<span id="code-status-' . $code->cid . '" style="color:' . $color .'">' . $reserved . '</span>';
  return $reserved;
}

/**
 * Form constructor for generating codes.
 */
function sms_vote_admin_generate_codes_form() {
  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Generate codes'),
    '#description' => t('How many codes to generate.'),
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate'),
  );
  return $form;
}

/**
 * Submit hanlder for the codes generator form.
 */
function sms_vote_admin_generate_codes_form_submit($form, &$form_state) {
  sms_vote_store_codes(sms_vote_generate_codes($form_state['values']['amount']));
  drupal_set_message(t('The codes have been generated.'));
}
