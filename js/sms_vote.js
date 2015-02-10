(function ($) {

Drupal.behaviors.SMSVote = {
  attach: function(context, settings) {
    // Hide the update button, when we have js the entire form should work with
    // AJAX.
    $('#sms-vote-voting-cart input[name="update"]').hide();
  }
};

}(jQuery));