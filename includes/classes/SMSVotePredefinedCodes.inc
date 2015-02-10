<?php

/**
 * @file
 *  Class with utils methods for the predefined codes.
 */

class SMSVotePredefinedCodes {

  /**
   * Generates and stores a set of predefined codes.
   *
   * @param int $amount
   *  How many codes to generate.
   *
   * @param array $node_types
   *  An array of node types for which the codes will be valid.
   */
  public static function generateAndStoreCodes($amount) {
    $sms_vote_settings = sms_vote_get_settings();
    SMSVotePredefinedCodes::storeCodes(SMSVotePredefinedCodes::generateCodes($amount, $sms_vote_settings['code_digits']));
  }

  /**
   * Generates an amount of codes to be stored as predefined codes.
   *
   * @param int $amount
   *  The amount of codes to be generated.
   */
  public static function generateCodes($amount, $digits_number) {
    $codes = array();
    $predefined_codes = array();
    $index = 0;
    $saved_codes = SMSVotePredefinedCodes::loadPredefinedCodes();
    foreach ($saved_codes as $saved_code) {
      $predefined_codes[] = $saved_code->code;
    }
    while ($index < $amount) {
      $code = SMSVotePredefinedCodes::generateCode($digits_number);
      // Make sure the code is unique.
      if (!in_array($code, $codes) && !in_array($code, $predefined_codes)) {
        $index++;
        $codes[] = $code;
      }
    }
    return $codes;
  }

  /**
   * Stores an array of codes into the predefined codes table.
   *
   * @param array $codes
   *  An array with codes to be saved.
   */
  public static function storeCodes($codes) {
    $query = db_insert('sms_vote_predefined_codes')->fields(array('code', 'reserved'));
    foreach ($codes as $code) {
      $query->values(array($code, 0));
    }
    $query->execute();
  }

  /**
   * Loads a predefined code by its id.
   *
   * @param int $cid
   *  The id of the code.
   */
  public static function loadPredefinedCode($cid) {
    $query = db_select('sms_vote_predefined_codes', 'svpc')->fields('svpc')->condition('svpc.cid', $cid);
    return $query->execute()->fetch();
  }

  /**
   * Returns the predefined codes.
   */
  public static function loadPredefinedCodes() {
    return db_select('sms_vote_predefined_codes', 'svpc')->fields('svpc')->execute()->fetchAll();
  }

  /**
   * Deletes a code from the predefined codes table.
   *
   * @param string $code
   *  The code to delete.
   */
  public static function deletePredefinedCode($code) {
    db_delete('sms_vote_predefined_codes')->condition('code', $code)->execute();
  }

  /**
   * Returns a random generated code.
   *
   * @param int $digits
   *  How many digits should the code have.s
   */
  public static function generateCode($digits = NULL) {
    if (empty($digits)) {
      $sms_vote_settings = sms_vote_get_settings();
      $digits = $sms_vote_settings['code_digits'];
    }
    $code = '';
    for ($index = 1; $index <= $digits; $index++) {
      $code .= rand(0, 9);
    }
    return $code;
  }
}
