<?php

/**
 * @file
 *  Points storage class that uses the session for storing the votes.
 */

class SMSVoteTmpSessionStorage extends SMSVoteTmpStorage {

  /**
   * @see SMSVoteTmpStorageInterface::storePoints()
   */
  public function storePoints($nid, $points) {
    $points = parent::storePoints($nid, $points);
    $_SESSION['sms_vote'][$nid] = $points;
    return $points;
  }

  /**
   * @see SMSVoteTmpStorageInterface::removePoints()
   */
  public function removePoints($nid) {
    unset($_SESSION['sms_vote'][$nid]);
    if (empty($_SESSION['sms_vote'])) {
      unset($_SESSION['sms_vote']);
    }
  }

  /**
   * @see SMSVoteTmpStorageInterface::getPoints()
   */
  public function getPoints($nid) {
    if (isset($_SESSION['sms_vote']) && isset($_SESSION['sms_vote'][$nid])) {
      return $_SESSION['sms_vote'][$nid];
    }
    return 0;
  }

  /**
   * @see SMSVoteTmpStorageInterface::getAllPoints()
   */
  public function getAllPoints() {
    if (isset($_SESSION['sms_vote'])) {
      return $_SESSION['sms_vote'];
    }
    return array();
  }


  /**
   * @see SMSVoteTmpStorageInterface::getAvailablePoints()
   */
  public function getAvailablePoints() {
    $left = $this->maximum_points;
    // We subtract from the maximum points allowed the points that the user has
    // in the session, because they are already given.
    if (isset($_SESSION['sms_vote'])) {
      foreach ($_SESSION['sms_vote'] as $points) {
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
   * @see SMSVoteTmpStorageInterface::storeProperty()
   */
  public function storeProperty($key, $value) {
    $_SESSION[$key] = $value;
  }

  /**
   * @see SMSVoteTmpStorageInterface::getProperty()
   */
  public function getProperty($key) {
    return isset($_SESSION[$key]) ? $_SESSION[$key] : NULL;
  }

  /**
   * @see SMSVoteTmpStorageInterface::clearStorage()
   */
  public function clearStorage() {
    unset($_SESSION['sms_vote']);
  }
}
