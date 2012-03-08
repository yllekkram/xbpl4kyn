package main.view.connection.message.messageIncoming;


public class FloorReachedMessage extends ViewMessageIncoming {

	int elevatorControllerId;
	int floor;
	
	public FloorReachedMessage(byte[] data){
		this.deserialize(data);
	}
	
	
	public void deserialize(byte[] data){
		this.elevatorControllerId = data[1];
		this.floor = data[2];
	}
	
	public int getElevatorControllerId(){
		return this.elevatorControllerId;
	}
	
	public int getFloor(){
		return this.floor;
	}

}
