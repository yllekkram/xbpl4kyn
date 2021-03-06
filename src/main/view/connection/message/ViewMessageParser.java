package main.view.connection.message;


import main.exception.UnexpectedEndOfMessageException;
import main.view.connection.message.messageIncoming.DirectionChangeMessage;
import main.view.connection.message.messageIncoming.FloorReachedMessage;
import main.view.connection.message.messageIncoming.HallCallRequestAcknowledgmentMessage;
import main.view.connection.message.messageIncoming.HallCallServicedMessage;
import main.view.connection.message.messageIncoming.RegistrationRequestMessage;
import main.view.connection.message.messageIncoming.RemoveElevatorMessage;
import main.view.connection.message.messageIncoming.ViewMessageIncoming;

public class ViewMessageParser {

	public static ViewMessageIncoming parseMessage(byte[] data) throws UnexpectedEndOfMessageException{
		int messageType = (int) data[0];
		ViewMessageIncoming message;
		
		switch(messageType){
		case ViewMessage.DIRECTION_CHANGE:
			message = new DirectionChangeMessage(data);
			break;
		case ViewMessage.FLOOR_REACHED:
			message = new FloorReachedMessage(data);
			break;
		case ViewMessage.HALLCALL_REQUEST_ACHNOWLEDGMENT:
			message = new HallCallRequestAcknowledgmentMessage(data);
			break;
		case ViewMessage.HALLCALL_SERVICED:
			message = new HallCallServicedMessage(data);
			break;
		case ViewMessage.REGISTRATION_REQUEST:
			message = new RegistrationRequestMessage(data);
			break;
		case ViewMessage.REMOVE_ELEVATOR:
			message = new RemoveElevatorMessage(data);
			break;
		default:
			return null;
		}
		return message; 
	}

}
