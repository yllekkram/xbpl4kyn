package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;


public class HallCallAssignmentMessage extends GroupDispatcherMessageOutgoing{
	
	int floorNumber;
	int direction;
	
	public HallCallAssignmentMessage(int floorNumber, int direction){
		this.floorNumber = floorNumber;
		this.direction = floorNumber;
	}
	
	public byte[] serialize() {
		return new byte[]{GroupDispatcherMessage.HALLCALL_ASSIGNMENT, (byte) this.floorNumber, (byte) this.direction};
	}
}
