package view;

import java.util.concurrent.ConcurrentHashMap;

import exception.IllegalDirectionException;
import util.Constants;
import view.connection.UDPConnectionManager;
import view.connection.UDPElevatorAddress;
import groupDispatcher.control.GroupDispatcher;


public class ElevatorMonitor {

	private static ElevatorMonitor instance; 
	
	public static final int HALLCALL_DIRECTION_UP = 1;
	public static final int HALLCALL_DIRECTION_DOWN = 2;
	
	private ConcurrentHashMap<Integer, UDPElevatorAddress> elevatorAddressList;
	
	public synchronized static ElevatorMonitor getInstance() {
		if(instance == null){
			instance = new ElevatorMonitor();
		}
		return instance;
	}
	
	private ElevatorMonitor(){
		super();
		elevatorAddressList = new ConcurrentHashMap<Integer, UDPElevatorAddress>();
	}
	
	/**
	 * Handles the sequence of events that occur when the user starts the system
	 */
	public void onStart(){
		System.out.println("Starting");
		GroupDispatcher.getInstance().startUp();
		ElevatorControlWindow.getInstance().onStart();
		UDPConnectionManager.getInstance().initialize();
	}
	
	/**
	 * Handles the sequence of events that occur when the user terminates the system
	 */
	public void onExit(){
		System.out.println("Exiting");
		System.exit(0);
	}
	
	public void onElevatorRegister(int elevatorId, String ip, int port){
		elevatorAddressList.put(Integer.valueOf(elevatorId), new UDPElevatorAddress(elevatorId, ip, port));
		ElevatorControlWindow.getInstance().addCar(elevatorId, 0);
	}

	
	public void onStopRequest(int carNumber){
		System.out.println("ElevatorMonitor - User clicked on stopButton in elevator " + carNumber);
	}
	
	public void onOpenDoorRequest(int carNumber){
		System.out.println("ElevatorMonitor - User clicked on openDoorButton in elevator " + carNumber);
	}
	
	public void onCloseDoorRequest(int carNumber){
		System.out.println("ElevatorMonitor - User clicked on closeDoorButton in elevator " + carNumber);
	}
	
	public void onHallCall(int floorNumber, int directionRequested){
		String directionRequestedStr;
		if(directionRequested == HALLCALL_DIRECTION_UP){
			directionRequestedStr = "UP";
		} else if(directionRequested == HALLCALL_DIRECTION_DOWN){
			directionRequestedStr = "DOWN";
		} else {
			throw new IllegalDirectionException("directionRequested is invalid. A valid direction is either HALLCALL_DIRECTION_UP or HALLCALL_DIRECTION_DOWN");
		}
		String message = "User on floor " + floorNumber + " pressed the " + directionRequestedStr + " button";
		
		System.out.println("ElevatorMonitor - " + message);
		
		//send a message to the groupDispatcher
		//TODO: use the defined message protocol
		UDPConnectionManager.getInstance().sendData(Constants.GUI_TO_GD_UDP_PORT, message.getBytes());
		
	}
	
	public void onFloorSelection(int carNumber, int floorNumber){
		String message = "User selected floor " + floorNumber + " in elevator " + carNumber;
		System.out.println("ElevatorMonitor - " + message);
		
		//send a message to the appropriate elevator
		//TODO: use the defined message protocol
		UDPElevatorAddress elevatorAddress = elevatorAddressList.get(Integer.valueOf(carNumber));
		if(elevatorAddress == null){
			System.out.println("The IP address and port for this elevator were not found. The ElevatorController was not registered.");
		}else{
			UDPConnectionManager.getInstance().sendData(elevatorAddress.getIp(), elevatorAddress.getPort(), message.getBytes());
		}
	}
}
