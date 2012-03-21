package main.view.connection.message.messageIncoming;

import java.util.Arrays;

import main.exception.UnexpectedEndOfMessageException;

public class RegistrationRequestMessage extends ViewMessageIncoming{

	int elevatorControllerId;
	
	public RegistrationRequestMessage(byte[] data) throws UnexpectedEndOfMessageException{
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data) throws UnexpectedEndOfMessageException {
		try{
			this.elevatorControllerId = data[1];
		}catch(ArrayIndexOutOfBoundsException e){
			throw new UnexpectedEndOfMessageException("Message received is " + Arrays.toString(data));
		}
	}
	
	
	public int getElevatorControllerId() {
		return this.elevatorControllerId;
	}
	

}
