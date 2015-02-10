(function ($) {

Drupal.behaviors.SMSVote = {
  attach: function(context, settings) {
    // Hide the update button, when we have js the entire form should work with
    // AJAX.
    $('#sms-vote-voting-cart input[name="update"]').hide();

    // everytime a vote is added, everytime we refresh pulsate and remind user that hes got votes
    $('.sms-vote-voting-cart').once(function(){
      $(this).effect('pulsate', { times: 1 }, 'normal');
    });

    // remove the the success message after 3 seconds
    $(document, context).bind('SMSVote_success', function() {
      $('.messages.messages--status').once('vote-success', function(){
        if ($('html').hasClass('mobile')) {
          var $this = $(this);

          // show first
          $this.addClass('fixed').fadeIn('normal');

          // gone in 3 seconds
          setTimeout(function () {
            $this.fadeOut('normal', function () {
              $this.remove();
            });
          }, 6000);
        } else {
          var $this = $(this);

          // move it to the sidebar
          $('.sms-cart-form-wrapper').append($this);

          // gone in 3 seconds
          setTimeout(function () {
            $this.fadeOut('normal', function () {
              $this.remove();
            });
          }, 6000);
        }
      });
    });
  }
};

}(jQuery));