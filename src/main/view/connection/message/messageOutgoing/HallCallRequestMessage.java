package main.view.connection.message.messageOutgoing;

import main.model.Destination;
import main.view.connection.message.ViewMessage;

public class HallCallRequestMessage extends ViewMessageOutgoing{
	
	Destination destination;
	
	public HallCallRequestMessage(int floorNumber, int direction){
		this.destination = new Destination(floorNumber, direction);
	}
	
	public byte[] serialize() {
		return new byte[]{ViewMessage.HALLCALL_REQUEST, (byte) this.destination.getFloor(), (byte) this.destination.getDirection()};
	}
}
