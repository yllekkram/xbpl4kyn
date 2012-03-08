package main.groupDispatcher.connection.messageIncoming;


public class RegistrationRequestMessage extends GroupDispatcherMessageIncoming{

	int elevatorControllerId;
	
	public RegistrationRequestMessage(byte[] data){
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data){
		this.elevatorControllerId = (int) data[1];
	}
	
	public int getElevatorControllerId(){
		return this.elevatorControllerId;
	}
}
