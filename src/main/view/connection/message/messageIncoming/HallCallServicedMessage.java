package main.view.connection.message.messageIncoming;

import main.model.Destination;


public class HallCallServicedMessage extends ViewMessageIncoming{
	
	Destination destination;
	
	public HallCallServicedMessage(byte[] data){
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data){
		int floor = data[1];
		int direction = data[2];
		this.destination = new Destination(floor, direction);
	}
	
	public int getFloor(){
		return this.destination.getFloor();
	}
	
	public int getDirection(){
		return this.destination.getDirection();
	}
}
