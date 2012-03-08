package main.groupDispatcher.connection.messageIncoming;

import main.model.Destination;


public class ECStatusMessage extends GroupDispatcherMessageIncoming {

	int position;
	int direction;
	int speed;
	Destination destinations[];
	
	public ECStatusMessage(byte[] data){
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data){
		this.position = (int) data[2];
		this.direction = (int) data[3];
		this.speed = (int) data[4];
		
		int index = 5;
		int destinationCount = (int) data[5];
		destinations = new Destination[destinationCount];
		for(int i = 0; i<destinationCount; i++){
			destinations[i] = new Destination( (int) data[index++], (int) data[index++] );
		}
	}
	
	public int getPosition(){
		return this.position;
	}
	
	public int getDirection(){
		return this.direction;
	}
	
	public int getSpeed(){
		return this.speed;
	}
	
	public Destination[] getDestinations(){
		return this.destinations;
	}
}
