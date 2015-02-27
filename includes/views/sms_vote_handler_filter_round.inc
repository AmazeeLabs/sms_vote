<?php

/**
 * Filter handler for rounds.
 */

class sms_vote_handler_filter_round extends views_handler_filter_in_operator {
  var $value_form_type = 'select';

  /**
   * @see views_handler_filter_in_operator::get_value_options()
   */
  function get_value_options() {
    $rounds = sms_vote_round_load_all();
    foreach ($rounds as $round) {
      $this->value_options[$round->svrid] = $round->title;
    }
  }
}