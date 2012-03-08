package main.view.connection.message.messageOutgoing;

import main.view.connection.message.ViewMessage;

public class RegistrationAcknowledgementMessage extends ViewMessageOutgoing {

	public RegistrationAcknowledgementMessage(){
		//no-op
	}
	
	public byte[] serialize() {
		return new byte[]{ViewMessage.REGISTRATION_ACKNOWLEDGEMENT};
	}
	
	

}
