package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;
import main.util.Constants;

public class HallCallRequestAcknowledgmentMessage extends GroupDispatcherMessageOutgoing{
	
	public HallCallRequestAcknowledgmentMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{
				GroupDispatcherMessage.HALLCALL_REQUEST_ACHNOWLEDGMENT,
				Constants.MESSAGE_DELIMITER
				};
	}
}
