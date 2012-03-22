package main.groupDispatcher.connection.message;

import main.exception.UnexpectedEndOfMessageException;
import main.groupDispatcher.connection.messageIncoming.ECStatusMessage;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageIncoming.HallCallRequestMessage;
import main.groupDispatcher.connection.messageIncoming.RegistrationRequestMessage;

public class GroupDispatcherMessageParser {
	
	public static GroupDispatcherMessageIncoming parseMessage(byte[] data) throws UnexpectedEndOfMessageException{
		int messageType = (int) data[0];
		GroupDispatcherMessageIncoming message;
		
		switch(messageType){
			case GroupDispatcherMessage.EC_STATUS:
				message = new ECStatusMessage(data);
				break;
			case GroupDispatcherMessage.HALLCALL_REQUEST:
				message = new HallCallRequestMessage(data);
				break;
			case GroupDispatcherMessage.REGISTRATION_REQUEST:
				message = new RegistrationRequestMessage(data);
				break;
			default:
				return null;
			}
		return message; 
	}

}
