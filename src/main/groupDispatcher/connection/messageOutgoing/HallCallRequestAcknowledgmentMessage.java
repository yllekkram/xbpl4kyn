package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;

public class HallCallRequestAcknowledgmentMessage extends GroupDispatcherMessageOutgoing{
	
	public HallCallRequestAcknowledgmentMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{GroupDispatcherMessage.HALLCALL_REQUEST_ACHNOWLEDGMENT};
	}
}
