<?php

/**
 * @file
 *  Factory class to create sms sender objects.
 */

class SMSVoteSenderFactory {

  /**
   * @return SMSDefaultSender
   */
  public static function getSender() {
    static $sender;
    if (!isset($sender)) {
      try {
        $class = variable_get('sms_vote_send_sms_class', '');
        if (empty($class) || !class_exists($class)) {
          // @todo: we should maybe have a different exception here.
          throw new Exception(t('The SMS sender class was not found found: !class', array('!class' => $class)));
        }
        $sender = new $class();
      }
      catch (Exception $e) {
        watchdog_exception('sms_vote', $e);
        return null;
      }
    }
    return $sender;
  }
}