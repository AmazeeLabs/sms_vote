<?php

/**
 * @file
 *  Controller for the SMS Vote entities.
 */

/**
 * SMS Vote entity controller class.
 *
 */
class SMSVoteEntityController extends EntityAPIController {

  /**
   * @see EntityAPIController::create()
   */
  public function create(array $values = array()) {
    // Put the default round id if not specified.
    if (!isset($values['round_id'])) {
      $values['round_id'] = variable_get('sms_vote_active_round', 0);
    }
    return parent::create($values);
  }
}
