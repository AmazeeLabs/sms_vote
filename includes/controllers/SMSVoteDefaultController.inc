<?php

/**
 * @file
 *  Base controller class for the SMS vote entities.
 */

/**
 * Default controller class for the SMS vote entities.
 * If the performance will be a big issue, we can disable the hooks invoked by
 * these entities, although that should really not affect so much the performance.
 */
class SMSVoteDefaultController extends DrupalDefaultEntityController implements EntityAPIControllerInterface {

	/**
   * @see EntityAPIControllerInterface::delete()
   */
  public function delete($ids) {
    $entities = $ids ? $this->load($ids) : FALSE;
    if (!$entities) {
      // Do nothing, in case invalid or no ids have been passed.
      return;
    }

    $transaction = db_transaction();
    try {
      db_delete($this->entityInfo['base table'])
        ->condition($this->idKey, array_keys($entities), 'IN')
        ->execute();
      // Reset the cache as soon as the changes have been applied.
      $this->resetCache($ids);

      foreach ($entities as $id => $entity) {
        $this->invoke('delete', $entity);
      }
      // Ignore slave server temporarily.
      db_ignore_slave();

      return TRUE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

	/**
   * @see EntityAPIControllerInterface::invoke()
   */
  public function invoke($hook, $entity) {
    module_invoke_all($this->entityType . '_' . $hook, $entity);
    // Invoke the respective entity level hook.
    if ($hook == 'presave' || $hook == 'insert' || $hook == 'update' || $hook == 'delete') {
      module_invoke_all('entity_' . $hook, $entity, $this->entityType);
    }
  }

	/**
   * @see EntityAPIControllerInterface::save()
   */
  public function save($entity) {
    $transaction = db_transaction();
    try {
      // Load the stored entity, if any.
      if (!empty($entity->{$this->idKey}) && !isset($entity->original)) {
        // In order to properly work in case of name changes, load the original
        // entity using the id key if it is available.
        $entity->original = entity_load_unchanged($this->entityType, $entity->{$this->idKey});
      }

      $this->invoke('presave', $entity);

      if (empty($entity->{$this->idKey}) || !empty($entity->is_new)) {
        // For new entities, create the row in the base table, then save the
        // revision.
        $op = 'insert';
        $return = drupal_write_record($this->entityInfo['base table'], $entity);
      }
      else {
        $op = 'update';
        $return = drupal_write_record($this->entityInfo['base table'], $entity, $this->idKey);
      }

      // Update the static cache so that the next entity_load() will return this
      // newly saved entity.
      $this->entityCache[$entity->{$this->idKey}] = $entity;

      $this->invoke($op, $entity);

      // Ignore slave server temporarily.
      db_ignore_slave();
      unset($entity->is_new);
      unset($entity->original);
      unset($entity->revision);

      return $return;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

	/**
   * @see EntityAPIControllerInterface::create()
   */
  public function create(array $values = array()) {
    // Add is_new property if it is not set.
    $values += array('is_new' => TRUE, 'created' => REQUEST_TIME);

    // If there is a class for this entity type, instantiate it now.
    if (isset($this->entityInfo['entity class']) && $class = $this->entityInfo['entity class']) {
      $entity = new $class($values, $this->entityType);
    }
    else {
      // Otherwise use a good old stdClass.
      $entity = (object) $values;
    }

    // Allow other modules to alter the created entity.
    // We will enable this if really needed.
    //drupal_alter('sms_vote_entity_create', $this->entityType, $entity);

    return $entity;
  }

	/**
   * @see EntityAPIControllerInterface::export()
   */
  public function export($entity, $prefix = '') {
    throw new Exception('Not implemented');
  }

	/**
   * @see EntityAPIControllerInterface::import()
   */
  public function import($export) {
    throw new Exception('Not implemented');
  }

	/**
   * @see EntityAPIControllerInterface::buildContent()
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL) {
    throw new Exception('Not implemented');
  }

	/**
   * @see EntityAPIControllerInterface::view()
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    throw new Exception('Not implemented');
  }
}
