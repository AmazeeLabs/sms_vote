<?php
/**
 * @file
 *   Displays the link for the sms vote form.
 */

$plugin = array(
  'title' => t('SMS Vote link'),
  'description' => t('Displays the link for the sms vote form.'),
  'single' => TRUE,
  'category' => array(t('Custom'), -9),
  'required context' => new ctools_context_required(t('Node'), 'node'),
  'render callback' => 'sms_vote_link_render',
  'defaults' => array(),
  'edit form' => 'sms_vote_link_render_config_form',
);

/**
 * Pane render function
 */
function sms_vote_link_render($subtype, $conf, $args, $context) {
  $block = new stdClass;
  $node = isset($context->data) ? $context->data : NULL;
  if (!empty($node)) {
    $vote_title = t('Vote for "!node"', array('!node' => $node->title));
    $query = drupal_get_destination();
    $block->content = sms_vote_l($vote_title, 'sms_vote/' . $node->nid . '/nojs', array('ui' => 'modal', 'query' => $query));
  }

  return $block;
}

/**
 * Configuration form, not needed at the moment.
 */
function sms_vote_link_render_config_form($form, &$form_state) {
  return $form;
}
