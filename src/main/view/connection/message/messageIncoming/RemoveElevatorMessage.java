package main.view.connection.message.messageIncoming;

import java.util.Arrays;

import main.exception.UnexpectedEndOfMessageException;

public class RemoveElevatorMessage extends ViewMessageIncoming{

	
	private int elevatorId;
	
	public RemoveElevatorMessage(byte[] data) throws UnexpectedEndOfMessageException{
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data) throws UnexpectedEndOfMessageException{
		try{
			this.elevatorId = data[1];
		}catch(ArrayIndexOutOfBoundsException e){
			throw new UnexpectedEndOfMessageException("Message received is " + Arrays.toString(data));
		}
	}
	
	public int getElevatorId(){
		return this.elevatorId;
	}
}
