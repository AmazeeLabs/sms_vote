<?php

/**
 * @file
 *  An abstract class that should be a base class for all the other storage
 *  classes.
 */

abstract class SMSVoteTmpStorage implements SMSVoteTmpStorageInterface {

  /**
   * The maximum amount of points of the voting system.
   */
  protected $maximum_points;

  /**
   * Class constructor.
   *
   * @param int $maximum_points
   */
  public function __construct($maximum_points) {
    $this->maximum_points = $maximum_points;
  }

  /**
   * @see SMSVoteTmpStorageInterface::storePoints()
   */
  public function storePoints($nid, $points) {
    // We make sure that we do not store more points that we are allowed to.
    // So this method will return either the $points parameter if we can store
    // that many for the node, or will return the maximum number of points that
    // the node can have if the $points is bigger that the allowed value.

    // The classes which extend this base class should always call the parent
    // class to actually get the correct number of points.
    $maximum_points_allowed = $this->getAvailablePointsForNode($nid);
    if ($maximum_points_allowed < $points) {
      return $maximum_points_allowed;
    }
    // If we are here, we can add the points to the node.
    return $points;
  }

  /**
   * @see SMSVoteTmpStorageInterface::getAvailablePointsForNode()
   */
	public function getAvailablePointsForNode($nid) {
    $available_points = $this->getAvailablePoints();
    $node_points = $this->getPoints($nid);
    $total_points = $available_points + $node_points;
    // Make sure we are not giving more points that we are allowed.
    if ($total_points > $this->maximum_points) {
      $total_points = $this->maximum_points;
    }
    return $total_points;
  }
}
