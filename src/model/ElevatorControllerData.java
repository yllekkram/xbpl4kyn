package model;


public class ElevatorControllerData {

	private int direction;
	private int speed;
	
	
	public ElevatorControllerData(){
	}
	
	public void setDirection(int direction){
		this.direction = direction;
	}
	
	public void setSpeed(int speed){
		this.speed = speed;
	}
	
	public int getDirection(){
		return direction;
	}
	
	public int getSpeed(){
		return speed;
	}
}
