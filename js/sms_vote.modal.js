/**
 * @file
 *
 * Adds some additional modal commands.
 */

(function ($) {
  Drupal.sms_vote = Drupal.sms_vote || {};
  /**
   * AJAX command to dismiss BeautyTip.
   */
  Drupal.sms_vote.delayed_modal_dismiss = function(ajax, response, status) {
    if (response.delay !== undefined) {
      setTimeout(function() {
        Drupal.CTools.Modal.modal_dismiss();
      }, response.delay);
    }
  };
  
  Drupal.ajax.prototype.commands.delayed_modal_dismiss = Drupal.sms_vote.delayed_modal_dismiss;
})(jQuery);
