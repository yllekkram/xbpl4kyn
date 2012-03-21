package main.view.connection.message.messageIncoming;

import java.util.Arrays;

import main.exception.UnexpectedEndOfMessageException;


public class DirectionChangeMessage extends ViewMessageIncoming {

	int elevatorControllerId;
	int newDirection;
	
	public DirectionChangeMessage(byte[] data) throws UnexpectedEndOfMessageException{
		this.deserialize(data);
	}
	
	
	public void deserialize(byte[] data) throws UnexpectedEndOfMessageException{
		try{
		this.elevatorControllerId = data[1];
		this.newDirection = data[2];
		}catch(ArrayIndexOutOfBoundsException e){
			throw new UnexpectedEndOfMessageException("Message received is " + Arrays.toString(data));
		}
	}
	
	public int getElevatorControllerId(){
		return this.elevatorControllerId;
	}
	
	public int getNewDirection(){
		return this.newDirection;
	}

}
