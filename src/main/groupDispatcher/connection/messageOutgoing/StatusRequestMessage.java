package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;
import main.util.Constants;


public class StatusRequestMessage extends GroupDispatcherMessageOutgoing{

	public StatusRequestMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{
				GroupDispatcherMessage.STATUS_REQUEST,
				Constants.MESSAGE_DELIMITER
				};
	}
}
