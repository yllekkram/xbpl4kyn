package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;
import main.util.Constants;


public class HallCallAssignmentMessage extends GroupDispatcherMessageOutgoing{
	
	int floorNumber;
	int direction;
	
	public HallCallAssignmentMessage(int floorNumber, int direction){
		this.floorNumber = floorNumber;
		this.direction = direction;
	}
	
	public byte[] serialize() {
		return new byte[]{
				GroupDispatcherMessage.HALLCALL_ASSIGNMENT,
				(byte) this.floorNumber,
				(byte) this.direction,
				Constants.MESSAGE_DELIMITER
				};
	}
}
