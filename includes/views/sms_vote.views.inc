<?php

/**
 * @file
 *  Views integration of the sms vote module.
 */

/**
 * Implements hook_views_data().
 */
function sms_vote_views_data() {
  $data = array();

  $data['sms_vote']['table']['group']  = t('SMS Vote');

  $data['sms_vote']['table']['base'] = array(
    'field' => 'svid',
    'title' => t('SMS Vote'),
    'help' => t('Am SMS Vote entity'),
  );
  $data['sms_vote']['table']['entity type'] = 'sms_vote';

  // The SMS Vote id.
  $data['sms_vote']['svid'] = array(
    'title' => t('SMS Vote ID'),
    'help' => t('The unique internal identifier of the SMS Vote.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // The phone number.
  $data['sms_vote']['phone'] = array(
    'title' => t('Phone'),
    'help' => t('Phone'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // The code sent by SMS.
  $data['sms_vote']['code'] = array(
    'title' => t('Code'),
    'help' => t('Code'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // The code sent by SMS.
  $data['sms_vote']['language'] = array(
    'title' => t('Language'),
    'help' => t('Language'),
    'field' => array(
      'handler' => 'views_handler_field_locale_language',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_locale_language',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_locale_language',
    ),
  );

  // The status of the vote.
  $data['sms_vote']['status'] = array(
    'title' => t('Vote status'),
    'help' => t('Vote status'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      // Note that you can override the field-wide label:
      'label' => t('Active'),
      // This setting is used by the boolean filter handler, as possible option.
      'type' => 'yes-no',
      // use boolean_field = 1 instead of boolean_field <> 0 in WHERE statment.
      'use equal' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // When the vote entity was created.
  $data['sms_vote']['created'] = array(
    'title' => t('Created'),
    'help' => t('Created'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  // When the vote entity was confirmed.
  $data['sms_vote']['confirmed'] = array(
    'title' => t('Confirmed'),
    'help' => t('Confirmed'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  $data['sms_vote']['validation_fails'] = array(
    'title' => t('Validation fails'),
    'help' => t('How many times the vode validation failed.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // The Vote Points entity.
  $data['sms_vote_points']['table']['group']  = t('SMS Vote Points');

  $data['sms_vote_points']['table']['base'] = array(
    'field' => 'svpid',
    'title' => t('SMS Vote Points'),
    'help' => t('Am SMS Vote Points entity'),
  );
  $data['sms_vote_points']['table']['entity type'] = 'sms_vote_points';

  $data['sms_vote_points']['table']['join'] = array(
    'sms_vote' => array(
      'left_field' => 'svid',
      'field' => 'svid',
    ),
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  // Vote ID table field.
  $data['sms_vote_points']['svid'] = array(
    'title' => t('Referenced Vote'),
    'help' => t('The vote entity referenced by this points entity.'),
    'relationship' => array(
      'base' => 'sms_vote', // The name of the table to join with.
      'base field' => 'svid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Referenced Vote Entity'),
    ),
  );

  // Node ID table field.
  $data['sms_vote_points']['nid'] = array(
    'title' => t('Reference content'),
    'help' => t('The content to which the vote points are assigned.'),
    'relationship' => array(
      'base' => 'node', // The name of the table to join with.
      'base field' => 'nid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Reference content'),
    ),
  );

  // The SMS Vote id
  $data['sms_vote_points']['svpid'] = array(
    'title' => t('SMS Vote Points ID'),
    'help' => t('The unique internal identifier of the SMS Vote Points.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // The number of points.
  $data['sms_vote_points']['points'] = array(
    'title' => t('Points'),
    'help' => t('Number of vote points.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );


  // The Vote total points table.
  $data['sms_vote_total_points']['table']['group']  = t('SMS Vote Total Points');

  $data['sms_vote_total_points']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('SMS Vote Total Points'),
    'help' => t('Total points for nodes.'),
  );

  $data['sms_vote_total_points']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  // Node ID table field.
  $data['sms_vote_total_points']['nid'] = array(
    'title' => t('Referenced content'),
    'help' => t('The content to which the total vote points are assigned.'),
    'relationship' => array(
      'base' => 'node', // The name of the table to join with.
      'base field' => 'nid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Referenced content'),
    ),
  );

  // The number of points.
  $data['sms_vote_total_points']['points'] = array(
    'title' => t('Total Points'),
    'help' => t('Number of total vote points.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  return $data;
}
