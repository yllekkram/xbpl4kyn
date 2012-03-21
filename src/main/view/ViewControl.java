package main.view;


import javax.swing.SwingUtilities;

import main.Main;
import main.groupDispatcher.control.GroupDispatcher;
import main.util.Constants;
import main.util.Log;
import main.view.connection.UDPConnectionManager;
import main.view.connection.message.messageOutgoing.CloseDoorRequestMessage;
import main.view.connection.message.messageOutgoing.FloorSelectionMessage;
import main.view.connection.message.messageOutgoing.HallCallRequestMessage;
import main.view.connection.message.messageOutgoing.OpenDoorRequestMessage;
import main.view.connection.message.messageOutgoing.StopRequestMessage;
import main.view.util.UIUtils;



public class ViewControl {

	private static ViewControl instance; 
	
	public synchronized static ViewControl getInstance() {
		if(instance == null){
			instance = new ViewControl();
		}
		return instance;
	}
	
	private ViewControl(){
		super();
	}
	
	/**
	 * Handles the sequence of events that occur when the user starts the system
	 */
	public void onStart(){
		Log.log("Starting");
		GroupDispatcher.getInstance().startUp();
		ElevatorControlWindow.getInstance().onStart();
		UDPConnectionManager.getInstance().initialize();
	}
	
	/**
	 * Handles the sequence of events that occur when the user terminates the system
	 */
	public void onExit(){
		Log.log("Exiting");
		System.exit(0);
	}
	
	public void onElevatorRegister(int elevatorId, String ip, int port){
		UDPConnectionManager.getInstance().addElevatorAddress(elevatorId, ip, port);
		ElevatorControlWindow.getInstance().addCar(elevatorId, 0);
	}

	public void onElevatorError(int elevatorId){
		//disable the elevator panel
		final ElevatorPanel elevatorPanel = ElevatorControlWindow.getInstance().getElevatorPanel(elevatorId);
		if(elevatorPanel == null){
			Main.onFatalError(new IllegalStateException("GUI - Could not find the elevator panel"));
		}
		SwingUtilities.invokeLater(new Runnable(){
			public void run(){
				elevatorPanel.getParent().remove(elevatorPanel);	
			}
		});
	}
	
	public void onHallCallServiced(int floor, final int direction){
		//enable the button
		final FloorPanel floorPanel = ElevatorControlWindow.getInstance().getFloorPanel(floor);
		if(floorPanel == null){
			Main.onFatalError(new IllegalStateException("GUI - Could not find the floor panel"));
		}
		SwingUtilities.invokeLater(new Runnable(){
			public void run(){
				floorPanel.enableButton(direction);
			}
		});
	}
	
	public void onStopRequest(int carNumber){
		Log.log("ViewControl - User clicked on stopButton in elevator " + carNumber);
		UDPConnectionManager.getInstance().sendDataToElevator(carNumber, new StopRequestMessage().serialize());
	}
	
	public void onOpenDoorRequest(int carNumber){
		Log.log("ViewControl - User clicked on openDoorButton in elevator " + carNumber);
		UDPConnectionManager.getInstance().sendDataToElevator(carNumber, new OpenDoorRequestMessage().serialize());
	}
	
	public void onCloseDoorRequest(int carNumber){
		Log.log("ViewControl - User clicked on closeDoorButton in elevator " + carNumber);
		UDPConnectionManager.getInstance().sendDataToElevator(carNumber, new CloseDoorRequestMessage().serialize());
	}
	
	public void onFloorReached(int elevatorId, int floor){
		//enable the button
		ElevatorPanel elevatorPanel = ElevatorControlWindow.getInstance().getElevatorPanel(elevatorId);
		if(elevatorPanel == null){
			Main.onFatalError(new IllegalStateException("GUI - Could not find the elevator panel"));
		}
		elevatorPanel.setButtonPressed(floor, false);
		elevatorPanel.setFloor(floor);
	}

	public void onElevatorDirectionChange(int elevatorId, int newDirection){
		//enable the button
		ElevatorPanel elevatorPanel = ElevatorControlWindow.getInstance().getElevatorPanel(elevatorId);
		if(elevatorPanel == null){
			Main.onFatalError(new IllegalStateException("GUI - Could not find the elevator panel"));
		}
		elevatorPanel.setDirection(newDirection);
	}

	public void onHallCall(int floorNumber, int directionRequested){
		String directionRequestedStr = UIUtils.getDirectionString(directionRequested);
		Log.log("ElevatorMonitor - User on floor " + floorNumber + " pressed the " + directionRequestedStr + " button");
		
		//send a message to the groupDispatcher
		HallCallRequestMessage hallCallMessage = new HallCallRequestMessage(floorNumber, directionRequested);
		UDPConnectionManager.getInstance().sendData(Constants.GUI_TO_GD_UDP_PORT, hallCallMessage.serialize());
		
	}
	
	public void onFloorSelection(int carNumber, int floorNumber){
		Log.log("ElevatorMonitor - User selected floor " + floorNumber + " in elevator " + carNumber);
		
		//send a message to the appropriate elevator
		UDPConnectionManager.getInstance().sendDataToElevator(carNumber, new FloorSelectionMessage(floorNumber).serialize());
	}
}
