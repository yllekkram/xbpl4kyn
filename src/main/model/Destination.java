package main.model;

public class Destination{
	int floor;
	int direction;
	
	public Destination(){
	}
	
	public Destination(int floor, int direction){
		this.floor = floor;
		this.direction = direction;
	}
	
	public int getFloor(){
		return this.floor;
	}
	
	public int getDirection(){
		return this.direction;
	}
}
