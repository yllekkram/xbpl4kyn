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
	
	public byte[] serialize(){
		return new byte[]{(byte) floor, (byte) direction};
	}
	
	public boolean equals(Object other){
		if(other instanceof Destination){
			Destination otherD = (Destination) other;
			if(otherD.getDirection() == this.getDirection() && otherD.getFloor() == this.getFloor()){
				return true;
			}
		}
		return false;
	}
}
