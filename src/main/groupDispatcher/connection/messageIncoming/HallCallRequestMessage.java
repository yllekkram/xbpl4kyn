package main.groupDispatcher.connection.messageIncoming;

import java.util.Arrays;

import main.exception.UnexpectedEndOfMessageException;
import main.model.Destination;

public class HallCallRequestMessage extends GroupDispatcherMessageIncoming{
	
	Destination destination;
	
	public HallCallRequestMessage(byte[] data) throws UnexpectedEndOfMessageException{
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data) throws UnexpectedEndOfMessageException{
		try{
			int floorNumber = data[1];
			int direction = data[2];
			destination = new Destination(floorNumber, direction);
		}catch(ArrayIndexOutOfBoundsException e){
			throw new UnexpectedEndOfMessageException("Message received is " + Arrays.toString(data));
		}
	}
	
	public int getFloorNumber(){
		return this.destination.getFloor();
	}
	
	public int getDirection(){
		return this.destination.getDirection();
	}
	
}
