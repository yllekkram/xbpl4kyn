package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;

public class RemoveElevatorMessage extends GroupDispatcherMessageOutgoing{
	
	int elevatorId;
	
	public RemoveElevatorMessage(int elevatorId){
		this.elevatorId = elevatorId;
	}

	public byte[] serialize(){
		return new byte[]{ GroupDispatcherMessage.REMOVE_ELEVATOR, (byte) elevatorId };
	}
}
