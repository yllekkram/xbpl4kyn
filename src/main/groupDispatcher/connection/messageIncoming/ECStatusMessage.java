package main.groupDispatcher.connection.messageIncoming;

import java.util.Arrays;

import main.exception.UnexpectedEndOfMessageException;
import main.model.Destination;


public class ECStatusMessage extends GroupDispatcherMessageIncoming {

	int position;
	int direction;
	boolean isMoving;
	Destination hallcalls[];
	int floorSelections[];
	
	public ECStatusMessage(byte[] data) throws UnexpectedEndOfMessageException{
		this.deserialize(data);
	}
	
	public void deserialize(byte[] data) throws UnexpectedEndOfMessageException{
		try{
			this.position = (int) data[2];

			this.direction = (int) data[3];
			this.isMoving = data[4] != 0;

			int index = 5;
			int hallcallCount = (int) data[index++];
			hallcalls = new Destination[hallcallCount];
			for(int i = 0; i<hallcallCount; i++){
				hallcalls[i] = new Destination( (int) data[index++], (int) data[index++] );
			}

			int floorSelectionCount = (int) data[index++];
			floorSelections = new int[floorSelectionCount];
			for(int i = 0; i<floorSelectionCount; i++){
				floorSelections[i] = (int) data[index++];
			}
		}catch(ArrayIndexOutOfBoundsException e){
			throw new UnexpectedEndOfMessageException("Message received is " + Arrays.toString(data));
		}
	}
	
	public int getPosition(){
		return this.position;
	}
	
	public int getDirection(){
		return this.direction;
	}
	
	public boolean isMoving(){
		return this.isMoving;
	}
	
	public Destination[] getHallCalls(){
		return this.hallcalls;
	}
	
	public int[] getFloorSelections(){
		return this.floorSelections;
	}
}
