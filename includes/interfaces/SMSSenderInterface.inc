<?php

/**
 * @file
 *  Interface for the SMS sender classes.
 */

interface SMSSenderInterface {

  /**
   * Sends a text message to a phone number.
   */
  public function send($phone, $text);
}