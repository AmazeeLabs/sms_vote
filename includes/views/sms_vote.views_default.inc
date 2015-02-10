<?php

/**
 * @file
 *  Provides default views.
 */

function sms_vote_views_default_views() {
  $views = array();

  $view = new view();
  $view->name = 'votes';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'sms_vote';
  $view->human_name = 'Votes';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Votes';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer sms vote';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'svid' => 'svid',
    'phone' => 'phone',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'svid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'phone' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no votes yet.';
  $handler->display->display_options['empty']['area']['format'] = 'expert_editor';
  /* Field: Bulk operations: SMS Vote */
  $handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['table'] = 'sms_vote';
  $handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_operations'] = array(
    'action::views_bulk_operations_delete_item' => array(
      'selected' => 1,
      'postpone_processing' => 0,
      'skip_confirmation' => 0,
      'override_label' => 0,
      'label' => '',
    ),
    'action::views_bulk_operations_script_action' => array(
      'selected' => 0,
      'postpone_processing' => 0,
      'skip_confirmation' => 0,
      'override_label' => 0,
      'label' => '',
    ),
    'action::views_bulk_operations_modify_action' => array(
      'selected' => 0,
      'postpone_processing' => 0,
      'skip_confirmation' => 0,
      'override_label' => 0,
      'label' => '',
      'settings' => array(
        'show_all_tokens' => 1,
        'display_values' => array(
          '_all_' => '_all_',
        ),
      ),
    ),
    'action::views_bulk_operations_argument_selector_action' => array(
      'selected' => 0,
      'skip_confirmation' => 0,
      'override_label' => 0,
      'label' => '',
      'settings' => array(
        'url' => '',
      ),
    ),
    'action::system_send_email_action' => array(
      'selected' => 0,
      'postpone_processing' => 0,
      'skip_confirmation' => 0,
      'override_label' => 0,
      'label' => '',
    ),
    'action::panelizer_set_status_action' => array(
      'selected' => 0,
      'postpone_processing' => 0,
      'skip_confirmation' => 0,
      'override_label' => 0,
      'label' => '',
    ),
  );
  /* Field: SMS Vote: Phone */
  $handler->display->display_options['fields']['phone']['id'] = 'phone';
  $handler->display->display_options['fields']['phone']['table'] = 'sms_vote';
  $handler->display->display_options['fields']['phone']['field'] = 'phone';
  /* Field: SMS Vote: Vote status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'sms_vote';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['type'] = 'custom';
  $handler->display->display_options['fields']['status']['type_custom_true'] = 'Active';
  $handler->display->display_options['fields']['status']['type_custom_false'] = 'Pending';
  $handler->display->display_options['fields']['status']['not'] = 0;
  /* Filter criterion: SMS Vote: Phone */
  $handler->display->display_options['filters']['phone']['id'] = 'phone';
  $handler->display->display_options['filters']['phone']['table'] = 'sms_vote';
  $handler->display->display_options['filters']['phone']['field'] = 'phone';
  $handler->display->display_options['filters']['phone']['operator'] = 'contains';
  $handler->display->display_options['filters']['phone']['exposed'] = TRUE;
  $handler->display->display_options['filters']['phone']['expose']['operator_id'] = 'phone_op';
  $handler->display->display_options['filters']['phone']['expose']['label'] = 'Phone';
  $handler->display->display_options['filters']['phone']['expose']['operator'] = 'phone_op';
  $handler->display->display_options['filters']['phone']['expose']['identifier'] = 'phone';
  $handler->display->display_options['filters']['phone']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    4 => 0,
    5 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/config/system/sms_vote/votes';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Votes';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['votes'] = array(
    t('Master'),
    t('Votes'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('Ç first'),
    t('Ü previous'),
    t('next Ý'),
    t('last È'),
    t('There are no votes yet.'),
    t('SMS Vote'),
    t('Phone'),
    t('Vote status'),
    t('Active'),
    t('Pending'),
    t('Page'),
  );

  $views[$view->name] = $view;

  return $views;
}
