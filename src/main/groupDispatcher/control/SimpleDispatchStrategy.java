package main.groupDispatcher.control;

import java.util.Enumeration;
import java.util.concurrent.ConcurrentHashMap;

import main.model.ElevatorData;

public class SimpleDispatchStrategy implements DispatchStrategy{
	
	
	
	
	
	
	public int dispatch(int floor, int direction) {
		
		//find closest elevator
		ConcurrentHashMap<Integer, ElevatorData> elevatorCars = GroupDispatcher.getInstance().getElevatorControllerData();
		Enumeration<Integer> enumeration = elevatorCars.keys();
		
		int leastDistance = Integer.MAX_VALUE;
		int selectedElevatorId = -1;
		
		int leastDistanceOp = Integer.MAX_VALUE;
		int selectedElevatorIdOp = -1;
		
		while(enumeration.hasMoreElements()){
			Integer elevatorId = enumeration.nextElement();
			ElevatorData elevatorData = elevatorCars.get(elevatorId);
			
			int distance =  elevatorData.getPosition() - floor;
			if( elevatorData.getDirection() == direction ){
				if(distance < leastDistance){
					leastDistance = distance;
					selectedElevatorId = elevatorId.intValue();
				} 
			}else{
				if(distance < leastDistanceOp){
					leastDistanceOp = distance;
					selectedElevatorIdOp = elevatorId.intValue();
				}
			}
			
			
		}
		
		if(selectedElevatorId != -1){
			return selectedElevatorId;
		}
		return selectedElevatorIdOp;
	}
}
