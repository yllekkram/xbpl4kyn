package main.view.connection.message.messageIncoming;


public class DirectionChangeMessage extends ViewMessageIncoming {

	int elevatorControllerId;
	int newDirection;
	
	public DirectionChangeMessage(byte[] data){
		this.deserialize(data);
	}
	
	
	public void deserialize(byte[] data){
		this.elevatorControllerId = data[1];
		this.newDirection = data[2];
	}
	
	public int getElevatorControllerId(){
		return this.elevatorControllerId;
	}
	
	public int getNewDirection(){
		return this.newDirection;
	}

}
