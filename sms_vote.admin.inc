<?php

/**
 * @file
 *  Admin menu callbacks for the SMS Vote module.
 */

/**
 * Form constructor for the voting settings form.
 */
function sms_vote_admin($form, &$form_state) {
  $sms_vote_settings = sms_vote_get_settings();
  $form['sms_vote_settings']['max_points'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum points'),
    '#description' => t('The maximum number of points a user can give for a vote. This is also the exact number of points that an user must distribute (if "Enable voting cart" and "Force distributing all the points" are enabled below) among one or more nodes.'),
    '#default_value' => $sms_vote_settings['max_points'],
  );
  $form['sms_vote_settings']['code_digits'] = array(
    '#type' => 'textfield',
    '#title' => t('Code digits number'),
    '#description' => t('How many digits should the confirmation code have.'),
    '#default_value' => $sms_vote_settings['code_digits'],
  );
  $form['sms_vote_settings']['max_fails'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum fails'),
    '#description' => t('The maximum number of times the user can input an incorrect code.'),
    '#default_value' => $sms_vote_settings['max_fails'],
  );
  $form['sms_vote_settings']['special_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone number for predefined codes'),
    '#description' => t('The special telephone number that can be used with the predefined codes.'),
    '#default_value' => $sms_vote_settings['special_number'],
  );
  $form['sms_vote_settings']['enable_cart'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable voting cart'),
    '#description' => t('If the cart is enabled, then the user will be able to submit points to more than one node in the same voting session.'),
    '#default_value' => $sms_vote_settings['enable_cart'],
  );
  $form['sms_vote_settings']['force_distribute_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force distributing all the points'),
    '#description' => t('If this is enabled the user must distribute all the points in the "Maximum points" field, but only when the voting cart is enabled.'),
    '#default_value' => $sms_vote_settings['force_distribute_all'],
    '#states' => array(
      'disabled' => array(
        ':input[name="sms_vote_settings[enable_cart]"]' => array('checked' => FALSE)
      ),
    ),
  );
  $form['sms_vote_settings']['activate_ajax'] = array(
    '#type' => 'checkbox',
    '#title' => t('Activate AJAX'),
    '#description' => t('If this is checked, the entire voting process will use AJAX for navigation.'),
    '#default_value' => $sms_vote_settings['activate_ajax'],
  );
  $form['sms_vote_settings']['ajax_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ajax settings'),
    '#states' => array(
      'visible' => array(
        ':input[name="sms_vote_settings[activate_ajax]"]' => array('checked' => TRUE)
      ),
    ),
  );
  $form['sms_vote_settings']['ajax_settings']['show_success_in_modal'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show the success message in the modal window'),
    '#default_value' => $sms_vote_settings['ajax_settings']['show_success_in_modal'],
  );
  $form['sms_vote_settings']['ajax_settings']['close_automatically'] = array(
    '#type' => 'checkbox',
    '#title' => t('Close the modal window automatically'),
    '#default_value' => $sms_vote_settings['ajax_settings']['close_automatically'],
  );
  $form['sms_vote_settings']['ajax_settings']['close_automatically_seconds'] = array(
    '#type' => 'textfield',
    '#title' => t('After seconds'),
    '#title_display' => 'invisible',
    '#field_prefix' => t('after'),
    '#field_suffix' => t('seconds'),
    '#size' => 3,
    '#maxlength' => 3,
    '#default_value' => $sms_vote_settings['ajax_settings']['close_automatically_seconds'],
    '#states' => array(
      'visible' => array(
        ':input[name="sms_vote_settings[ajax_settings][close_automatically]"]' => array('checked' => TRUE)
      ),
    ),
  );
  $form['#tree'] = TRUE;
  return system_settings_form($form);
}

/**
 * Validate handler for the voting settings form.
 */
function sms_vote_admin_validate(&$form, &$form_state) {
  sms_vote_admin_check_numeric_and_positive($form_state['values']['sms_vote_settings']['max_points'], t('The maximum number of points has to be numeric and greater than 1.'), $form['sms_vote_settings']['max_points']);
  sms_vote_admin_check_numeric_and_positive($form_state['values']['sms_vote_settings']['max_fails'], t('The maximum number of fails has to be numeric and greater than 1.'), $form['sms_vote_settings']['max_fails']);
  sms_vote_admin_check_numeric_and_positive($form_state['values']['sms_vote_settings']['code_digits'], t('The number of digits has to be numeric and greater than 1.'), $form['sms_vote_settings']['code_digits']);
  if (!empty($form_state['values']['sms_vote_settings']['activate_ajax']) && !empty($form_state['values']['sms_vote_settings']['ajax_settings']['close_automatically'])) {
    $seconds = $form_state['values']['sms_vote_settings']['ajax_settings']['close_automatically_seconds'];
    if (!is_numeric($seconds) || $seconds < 0) {
      form_error($form['sms_vote_settings']['ajax_settings']['close_automatically_seconds'], t('The number of seconds must be numeric and greater or equal to 0.'));
    }
  }
}

/**
 * Helper validation function that checks if a value is numeric and bigger than
 * 1, and if yes it makes sure it is an integer.
 */
function sms_vote_admin_check_numeric_and_positive(&$value, $error_message, &$form_element) {
  if (!is_numeric($value) || $value < 1) {
    form_error($form_element, $error_message);
  }
  else {
    $value = (int)$value;
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
  $codes = SMSVotePredefinedCodes::loadPredefinedCodes();
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
  SMSVotePredefinedCodes::generateAndStoreCodes($form_state['values']['amount']);
  drupal_set_message(t('The codes have been generated.'));
}
