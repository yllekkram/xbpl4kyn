package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;

public class HallCallRequestAcknowledgementMessage extends GroupDispatcherMessageOutgoing{
	
	public HallCallRequestAcknowledgementMessage(){
	}
	
	public byte[] serialize() {
		return new byte[]{GroupDispatcherMessage.HALLCALL_REQUEST_ACHNOWLEDGEMENT};
	}
}
