package main.view.connection.message.messageIncoming;

public class RegistrationRequestMessage extends ViewMessageIncoming{

	int elevatorControllerId;
	
	public RegistrationRequestMessage(byte[] data){
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data) {
		this.elevatorControllerId = data[1];
	}
	
	
	public int getElevatorControllerId() {
		return this.elevatorControllerId;
	}
	

}
