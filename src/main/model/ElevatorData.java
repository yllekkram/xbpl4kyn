package main.model;

import java.util.Arrays;
import java.util.Vector;


public class ElevatorData {

	private int direction;
	private int speed;
	private int position;
	private Vector<Destination> destinations;
	
	public ElevatorData(){
		destinations = new Vector<Destination>();
	}
	
	public void setDirection(int direction){
		this.direction = direction;
	}
	
	public int getDirection(){
		return direction;
	}
	
	public void setSpeed(int speed){
		this.speed = speed;
	}
	
	public int getSpeed(){
		return speed;
	}

	public void setPosition(int position) {
		this.position = position;
	}
	
	public int getPosition() {
		return position;
	}
	
	public void setDestinations(Destination destinations[]){
		this.destinations = new Vector<Destination>(Arrays.asList(destinations));
	}
	
	public Destination[] getDestinations(){
		Destination destinations[] = new Destination[this.destinations.size()];
		return this.destinations.toArray(destinations);
	}
	
	public void addDestination(Destination destination){
		this.destinations.add(destination);
	}
}
