<?php

/**
 * @file
 *  Temporary votes storage that uses the ctools cache.
 */

class SMSVoteTmpCtoolsCacheStorage extends SMSVoteTmpStorage {

  public function __construct($maximum_points){
    parent::__construct($maximum_points);
    // Make sure we load the ctools object cache library.
    ctools_include('object-cache');
    $this->sid = session_api_get_sid();
  }

  /**
   * @see SMSVoteTmpStorageInterface::storePoints()
   */
  public function storePoints($nid, $points) {
    $points = parent::storePoints($nid, $points);
    $cache = ctools_object_cache_get('sms_vote', 'sms_vote', FALSE, $this->sid);
    if (empty($cache)) {
      $cache = array();
    }
    $cache[$nid] = $points;
    ctools_object_cache_set('sms_vote', 'sms_vote', $cache, $this->sid);
    return $points;
  }

	/**
   * @see SMSVoteTmpStorageInterface::removePoints()
   */
  public function removePoints($nid) {
    $cache = ctools_object_cache_get('sms_vote', 'sms_vote', FALSE, $this->sid);
    unset($cache[$nid]);
    if (empty($cache)) {
      ctools_object_cache_clear('sms_vote', 'sms_vote', $this->sid);
    }
    else {
      ctools_object_cache_set('sms_vote', 'sms_vote', $cache, $this->sid);
    }
  }

	/**
   * @see SMSVoteTmpStorageInterface::getPoints()
   */
  public function getPoints($nid) {
    $cache = ctools_object_cache_get('sms_vote', 'sms_vote', FALSE, $this->sid);
    if (!empty($cache[$nid])) {
      return $cache[$nid];
    }
    return 0;
  }

	/**
   * @see SMSVoteTmpStorageInterface::getAllPoints()
   */
  public function getAllPoints() {
    $cache = ctools_object_cache_get('sms_vote', 'sms_vote', FALSE, $this->sid);
    if (!empty($cache)) {
      return $cache;
    }
    return array();
  }

	/**
   * @see SMSVoteTmpStorageInterface::getAvailablePoints()
   */
  public function getAvailablePoints() {
    $left = $this->maximum_points;
    $cache = ctools_object_cache_get('sms_vote', 'sms_vote', FALSE, $this->sid);
    // We subtract from the maximum points allowed the points that the user has
    // in the session, because they are already given.
    if (!empty($cache)) {
      foreach ($cache as $points) {
        $left -= $points;
      }
    }
    // Although this should not happen, make sure that the number of left points
    // is not negative.
    if ($left < 0) {
      $left = 0;
    }
    return $left;
  }

  /**
   * @see SMSVoteTmpStorageInterface::clearStorage()
   */
  public function clearStorage() {
    ctools_object_cache_clear('sms_vote', 'sms_vote', $this->sid);
  }
}
