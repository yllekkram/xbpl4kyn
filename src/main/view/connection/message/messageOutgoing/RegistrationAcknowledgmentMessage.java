package main.view.connection.message.messageOutgoing;

import main.view.connection.message.ViewMessage;

public class RegistrationAcknowledgmentMessage extends ViewMessageOutgoing {

	public RegistrationAcknowledgmentMessage(){
		//no-op
	}
	
	public byte[] serialize() {
		return new byte[]{ViewMessage.REGISTRATION_ACKNOWLEDGMENT};
	}
	
	

}
