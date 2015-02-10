<?php

/**
 * @file
 *  Temporary votes storage that uses the session_api id to store data in a
 *  custom table.
 */

class SMSVoteTmpSessionAPIStorage extends SMSVoteTmpStorage {

  protected $sid;

  public function __construct($maximum_points){
    parent::__construct($maximum_points);
    // Get our session id here.
    $this->sid = session_api_get_sid();
  }

  /**
   * @see SMSVoteTmpStorageInterface::storePoints()
   */
  public function storePoints($nid, $points) {
    $points = parent::storePoints($nid, $points);
    $cache = $this->getSessionData();
    if (empty($cache['vote_cart'])) {
      $cache = array();
    }
    $cache['vote_cart'][$nid] = $points;
    $this->storeSessionData($cache);
    return $points;
  }

	/**
   * @see SMSVoteTmpStorageInterface::removePoints()
   */
  public function removePoints($nid) {
    $cache = $this->getSessionData();
    unset($cache['vote_cart'][$nid]);
    if (empty($cache['vote_cart'])) {
      unset($cache['vote_cart']);
    }
    $this->storeSessionData($cache);
  }

	/**
   * @see SMSVoteTmpStorageInterface::getPoints()
   */
  public function getPoints($nid) {
    $cache = $this->getSessionData();
    if (!empty($cache['vote_cart'][$nid])) {
      return $cache['vote_cart'][$nid];
    }
    return 0;
  }

	/**
   * @see SMSVoteTmpStorageInterface::getAllPoints()
   */
  public function getAllPoints() {
    $cache = $this->getSessionData();
    if (!empty($cache['vote_cart'])) {
      return $cache['vote_cart'];
    }
    return array();
  }

	/**
   * @see SMSVoteTmpStorageInterface::getAvailablePoints()
   */
  public function getAvailablePoints() {
    $left = $this->maximum_points;
    $cache = $this->getSessionData();
    // We subtract from the maximum points allowed the points that the user has
    // in the session, because they are already given.
    if (!empty($cache['vote_cart'])) {
      foreach ($cache['vote_cart'] as $points) {
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
    $this->clearSessionData();
  }

  /**
   * The next methods could may be part of another class.
   */

  /**
   * Loads the data from the current session.
   */
  protected function getSessionData() {
    $session_data = &drupal_static(__CLASS__ . ':session_data', array());
    if (!isset($session_data[$this->sid])) {
      $session_data[$this->sid] = array();
      $query = db_select('sms_vote_session', 'svc')->fields('svc')->condition('svc.session_api_id', $this->sid);
      $result = $query->execute()->fetch();
      if (!empty($result->data)) {
        $session_data[$this->sid] = unserialize($result->data);
      }
    }
    return $session_data[$this->sid];
  }

  /**
   * Stores the data in the current session.
   */
  protected function storeSessionData($data) {
    $this->clearSessionData();
    if (!empty($data)) {
      db_insert('sms_vote_session')->fields(array('session_api_id' => $this->sid, 'data' => serialize($data)))->execute();
    }
  }

  /**
   * Clears the data from the current session.
   */
  protected function clearSessionData() {
    db_delete('sms_vote_session')->condition('session_api_id', $this->sid)->execute();
    drupal_static_reset(__CLASS__ . ':session_data');
  }
}
