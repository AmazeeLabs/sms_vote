<?php

/**
 * @file
 *  Interface for storing the temporary values for votes, until they get saved
 *  into the database.
 */

interface SMSVoteTmpStorageInterface {

  /**
   * Stores a number of points for a node id.
   *
   * @param int $nid
   *  The node id.
   *
   * @param int $points
   *  The number of points.
   *
   * @return int
   *  The points that were actually stored for the node.
   */
  public function storePoints($nid, $points);

  /**
   * Removes the points for a node.
   *
   * @param int $nid
   *  The node id.
   */
  public function removePoints($nid);

  /**
   * Returns the number of points a node has.
   *
   * @param int $nid
   *  The node id.
   */
  public function getPoints($nid);

  /**
   * Returns an array with all the available points stored in the storage.
   */
  public function getAllPoints();

  /**
   * Returns all the available points that a user can give.
   */
  public function getAvailablePoints();

  /**
   * Returns the available points that a user can give to a node.
   *
   * @param int $nid
   *  The node id.
   */
  public function getAvailablePointsForNode($nid);

  /**
   * Clears everything from the temporary storage.
   */
  public function clearStorage();
}