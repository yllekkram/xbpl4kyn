package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;
import main.util.Constants;


public class RegistrationAcknowledgmentMessage extends GroupDispatcherMessageOutgoing{
	
	public RegistrationAcknowledgmentMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{GroupDispatcherMessage.REGISTRATION_ACKNOWLEDGMENT, Constants.MESSAGE_DELIMITER};
	}
}

