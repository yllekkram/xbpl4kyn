package main.groupDispatcher.connection.messageIncoming;

import java.util.Arrays;

import main.exception.UnexpectedEndOfMessageException;


public class RegistrationRequestMessage extends GroupDispatcherMessageIncoming{

	int elevatorControllerId;
	
	public RegistrationRequestMessage(byte[] data) throws UnexpectedEndOfMessageException{
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data) throws UnexpectedEndOfMessageException{
		try{
			this.elevatorControllerId = (int) data[1];			
		}catch(ArrayIndexOutOfBoundsException e){
			throw new UnexpectedEndOfMessageException("Message received is " + Arrays.toString(data));
		}
	}
	
	public int getElevatorControllerId(){
		return this.elevatorControllerId;
	}
}
