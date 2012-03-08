package main.groupDispatcher.connection.messageIncoming;

import main.model.Destination;

public class HallCallRequestMessage extends GroupDispatcherMessageIncoming{
	
	Destination destination;
	
	public HallCallRequestMessage(byte[] data){
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data){
		int floorNumber = data[1];
		int direction = data[2];
		destination = new Destination(floorNumber, direction);
	}
	
	public int getFloorNumber(){
		return this.destination.getFloor();
	}
	
	public int getDirection(){
		return this.destination.getDirection();
	}
	
}
