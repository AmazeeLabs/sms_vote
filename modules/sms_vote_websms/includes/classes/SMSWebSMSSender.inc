<?php

/**
 * @file
 *  SMS Sender class that uses the Web SMS service (websms.ch).
 */

/**
 * SMS Sender class that uses the Web SMS service.
 */
class SMSWebSMSSender extends SMSDefaultSender {

  protected $username;
  protected $password;
  protected $gateway_url;

  /**
   * Constructor of the class.
   */
  public function __construct() {
    // @todo: It would be nice that these parameters to be received in the
    // constructor. But the way how the class is instantiated by the factory
    // makes a bit more complex to send the parameters.
    $this->username = variable_get('sms_vote_websms_username', '');
    $this->password = variable_get('sms_vote_websms_password', '');
    $this->gateway_url = variable_get('sms_vote_websms_gateway_url', 'https://api.websms.com/');
  }

  /**
   * @see SMSSenderInterface::send()
   */
	public function send($phone, $text) {
	  drupal_set_message('Phone: ' . $phone . '; Text: ' . $text);
	  return;

    // If the first character of the phone is '+', remove it. The service does
    // not accept this format.
    if (substr($phone, 0, 1) == '+') {
      $phone = substr($phone, 1);
    }

    $recipientAddressList = array($phone);
    $maxSmsPerMessage = 1;
    $test = variable_get('sms_vote_websms_testmode', TRUE) ? TRUE : FALSE;
    $debug = variable_get('sms_vote_websms_debug', FALSE);

	  try {
      $smsClient = new WebSmsCom_Client($this->username, $this->password, $this->gateway_url);
	    if (!empty($debug)) {
	      $smsClient->setVerbose(true);
	    }
	    // Make sure we are sending a string.
      $text = (string) $text;
      $message  = new WebSmsCom_TextMessage($recipientAddressList, $text);

      // force sendAsFlashSms to be false
      $message->setSendAsFlashSms(FALSE);

      // Send the message and log the resposne if in debug mode.
      $Response = $smsClient->send($message, $maxSmsPerMessage, $test);
      if (!empty($debug)) {
        ob_start;
        echo '<pre>';
        print_r(array(
               "Status          : ".$Response->getStatusCode(),
               "StatusMessage   : ".$Response->getStatusMessage(),
               "TransferId      : ".$Response->getTransferId(),
               "ClientMessageId : ".(($Response->getClientMessageId()) ?
                                    $Response->getClientMessageId() : '<NOT SET>'),
        ));
        $output = ob_get_clean();
        watchdog('sms_vote', 'SMS sent to ' . $phone . ': ' . $output, NULL);
      }
    } catch (Exception $e) {
      watchdog_exception('sms_vote_websms', $e);
    }
  }
}
