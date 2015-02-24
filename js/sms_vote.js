(function ($) {

Drupal.behaviors.SMSVote = {
  attach: function(context, settings) {
    // Hide the update button, when we have js the entire form should work with
    // AJAX.
    $('#sms-vote-voting-cart input[name="update"]').hide();
  }
};

Drupal.behaviors.SMSVoteChangePhone = {
  attach: function(context, settings) {
    $('#change-phone', context).bind('click', function(e) {
      $('#current-phone').remove();
      $('#phone-wrapper').fadeIn();
      return false;
    });
  }
}

}(jQuery));