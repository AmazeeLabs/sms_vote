<?php

/**
 * @file
 *  Factory class for creating Vote Storage objects.
 */

class SMSVoteTmpStorageFactory {

  /**
   * @return SMSVoteTmpStorage
   */
  public static function getStorage() {
    static $storage;
    if (!isset($storage)) {
      try {
        $class = variable_get('sms_vote_tmp_storage_class', 'SMSVoteTmpSessionAPIStorage');
        if (!class_exists($class)) {
          // @todo: we should maybe have a different exception here.
          throw new Exception(t('The vote tmp storage class was not found: !class', array('!class' => $class)));
        }
        $sms_vote_settings = sms_vote_get_settings();
        $storage = new $class($sms_vote_settings['max_points']);
      }
      catch (Exception $e) {
        watchdog_exception('sms_vote', $e);
        return null;
      }
    }
    return $storage;
  }
}
