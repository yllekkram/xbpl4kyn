package main.groupDispatcher.connection.messageOutgoing;

import main.model.Destination;
import main.util.Constants;

public class HallCallServicedMessage extends GroupDispatcherMessageOutgoing{
	
	Destination destination;
	
	public HallCallServicedMessage(int floor, int direction){
		this.destination = new Destination(floor, direction);
	}
	
	public byte[] serialize() {
		return new byte[]{
				GroupDispatcherMessageOutgoing.HALLCALL_SERVICED, 
				(byte) this.destination.getFloor(), 
				(byte) this.destination.getDirection(), 
				Constants.MESSAGE_DELIMITER
				};
	}
}
