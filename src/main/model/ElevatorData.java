package main.model;

import java.util.Arrays;
import java.util.Vector;


public class ElevatorData {

	private int direction;
	private boolean isMoving;
	private int position;
	private Vector<Destination> assignedHallCalls;
	private int[] floorSelections;
	
	
	public ElevatorData(){
		this.assignedHallCalls = new Vector<Destination>();
	}
	
	public void setDirection(int direction){
		this.direction = direction;
	}
	
	public int getDirection(){
		return direction;
	}
	
	public void setIsMoving(boolean isMoving){
		this.isMoving = isMoving;
	}
	
	public boolean isMoving(){
		return this.isMoving;
	}

	public void setPosition(int position) {
		this.position = position;
	}
	
	public int getPosition() {
		return this.position;
	}
	
	public void setAssignedHallCalls(Destination assignedHallCalls[]){
		this.assignedHallCalls = new Vector<Destination>(Arrays.asList(assignedHallCalls));
	}
	
	public Destination[] getAssignedHallCalls(){
		Destination destinations[] = new Destination[this.assignedHallCalls.size()];
		return this.assignedHallCalls.toArray(destinations);
	}
	
	public void addAssignedHallCall(Destination destination){
		this.assignedHallCalls.add(destination);
	}
	
	public int[] getFloorSelections(){
		return this.floorSelections;
	}
}
