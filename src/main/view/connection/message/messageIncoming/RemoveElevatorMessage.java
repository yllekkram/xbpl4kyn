package main.view.connection.message.messageIncoming;

public class RemoveElevatorMessage extends ViewMessageIncoming{

	
	private int elevatorId;
	
	public RemoveElevatorMessage(byte[] data){
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data){
		this.elevatorId = data[1];
	}
	
	public int getElevatorId(){
		return this.elevatorId;
	}
}
