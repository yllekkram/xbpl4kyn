package main.view.connection.message.messageOutgoing;

import main.view.connection.message.ViewMessage;


public class StopRequestMessage extends ViewMessageOutgoing {

	public StopRequestMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{ ViewMessage.STOP_REQUEST };
	}
}
