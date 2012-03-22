package main.view.connection.message.messageOutgoing;

import main.view.connection.message.ViewMessage;


public class FloorSelectionMessage extends ViewMessageOutgoing {

	int floor;
	
	public FloorSelectionMessage(int floor){
		this.floor = floor;
	}
	
	public byte[] serialize() {
		return new byte[]{
				ViewMessage.FLOOR_SELECTION,
				(byte) floor
				};
	}
}
