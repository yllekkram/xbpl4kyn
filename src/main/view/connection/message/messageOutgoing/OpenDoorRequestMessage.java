package main.view.connection.message.messageOutgoing;

import main.view.connection.message.ViewMessage;


public class OpenDoorRequestMessage extends ViewMessageOutgoing {

	public OpenDoorRequestMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{
				ViewMessage.OPEN_DOOR_REQUEST
				};
	}
}
