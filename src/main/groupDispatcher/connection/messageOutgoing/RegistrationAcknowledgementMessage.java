package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;


public class RegistrationAcknowledgementMessage extends GroupDispatcherMessageOutgoing{
	
	public RegistrationAcknowledgementMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{GroupDispatcherMessage.REGISTRATION_ACKNOWLEDGEMENT};
	}
}

