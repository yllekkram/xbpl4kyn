package main.view.connection.message.messageIncoming;

import java.util.Arrays;

import main.exception.UnexpectedEndOfMessageException;
import main.model.Destination;


public class HallCallServicedMessage extends ViewMessageIncoming{
	
	Destination destination;
	
	public HallCallServicedMessage(byte[] data) throws UnexpectedEndOfMessageException{
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data) throws UnexpectedEndOfMessageException{
		try{
			int floor = data[1];
			int direction = data[2];
			this.destination = new Destination(floor, direction);
		}catch(ArrayIndexOutOfBoundsException e){
			throw new UnexpectedEndOfMessageException("Message received is " + Arrays.toString(data));
		}
	}
	
	public int getFloor(){
		return this.destination.getFloor();
	}
	
	public int getDirection(){
		return this.destination.getDirection();
	}
}
