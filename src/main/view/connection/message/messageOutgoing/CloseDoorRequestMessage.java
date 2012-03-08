package main.view.connection.message.messageOutgoing;

import main.view.connection.message.ViewMessage;


public class CloseDoorRequestMessage extends ViewMessageOutgoing {

	public CloseDoorRequestMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{ViewMessage.CLOSE_DOOR_REQUEST};
	}
}
