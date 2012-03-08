package main.groupDispatcher.control;


import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


import main.Main;
import main.groupDispatcher.connection.TCPClientSocketWrapper;
import main.groupDispatcher.connection.TCPConnectionManager;
import main.groupDispatcher.connection.UDPConnectionManager;
import main.groupDispatcher.connection.UpdaterRunnable;
import main.groupDispatcher.connection.messageIncoming.ECStatusMessage;
import main.groupDispatcher.connection.messageIncoming.HallCallRequestMessage;
import main.groupDispatcher.connection.messageOutgoing.HallCallAssignmentMessage;
import main.groupDispatcher.connection.messageOutgoing.HallCallRequestAcknowledgementMessage;
import main.groupDispatcher.connection.messageOutgoing.RegistrationAcknowledgementMessage;
import main.groupDispatcher.connection.messageOutgoing.RemoveElevatorMessage;
import main.model.Destination;
import main.model.ElevatorData;
import main.util.Constants;


public class GroupDispatcher implements Observer{

	private ExecutorService executor;
	private ConcurrentHashMap<Integer, Runnable> updaterRunnables;
	private ConcurrentHashMap<Integer, ElevatorData> elevatorCars;
	
	private DispatchStrategy dispatchStrategy;
	
	private static GroupDispatcher instance;

	public synchronized static GroupDispatcher getInstance(){
		if(instance == null){
			instance = new GroupDispatcher();
		}
		return instance;
	}
	
	private GroupDispatcher(){
		executor = Executors.newFixedThreadPool(Constants.ELEVATOR_COUNT);
		updaterRunnables = new ConcurrentHashMap<Integer, Runnable>();
		elevatorCars = new ConcurrentHashMap<Integer, ElevatorData>();
		TCPConnectionManager.getInstance().addObserver(this);
		
		dispatchStrategy = new SimpleDispatchStrategy();
	}
	
	
	public void startUp(){
		TCPConnectionManager.getInstance().initialize();
		UDPConnectionManager.getInstance().initialize();
		
		//execute
		this.run();
	}
	
	
	public void run(){
		TimerTask task = new TimerTask(){
			public void run(){
				Iterator<Runnable> iterator = updaterRunnables.values().iterator();
				while(iterator.hasNext()){
					Runnable runnable = iterator.next();
					executor.execute(runnable);
				}
			}
		};
		Timer timer = new Timer();
		timer.scheduleAtFixedRate(task, 0, Constants.STATUS_UPDATE_REQUEST_INTERVAL);
	}
	
	public void onConnectionCreated(TCPClientSocketWrapper clientSocket){
		System.out.println("GroupDispatcher recevied a connection request from an ElevatorController,  sending acknowledgement..");
		
		//respond with an acknowledgement message
		TCPConnectionManager.getInstance().sendData(clientSocket.getClientId(), new RegistrationAcknowledgementMessage().serialize());
		
		//store elevator models
		ElevatorData elevatorControllerData = new ElevatorData();
		elevatorCars.put(Integer.valueOf(clientSocket.getClientId()), elevatorControllerData); //thread-safe
		
		//store elevator updater runnables
		UpdaterRunnable runnable = new UpdaterRunnable(clientSocket.getClientId());
		updaterRunnables.put(Integer.valueOf(clientSocket.getClientId()), runnable); //thread-safe
	}
	
	/**
	 * Assigns a given destination to an elevator according to the chosen dispatch strategy and returns the elevator id of the chosen elevator
	 * @param hallCall
	 */
	public void onHallCall(HallCallRequestMessage hallCall){
		this.onHallCall(hallCall.getFloorNumber(), hallCall.getDirection());
	}
	
	public void onHallCall(int floor, int direction){
		int selectedElevator = GroupDispatcher.getInstance().getDispatchStrategy().dispatch(floor, direction);
		if(selectedElevator == -1){
			System.out.println("No elevators connected. Hall call discarded.");
			return;
		}
		TCPConnectionManager.getInstance().sendData(selectedElevator, new HallCallAssignmentMessage(floor, direction).serialize());
		UDPConnectionManager.getInstance().sendData(Constants.GD_TO_GUI_UDP_PORT, new HallCallRequestAcknowledgementMessage().serialize());
	}
	
	public DispatchStrategy getDispatchStrategy(){
		return dispatchStrategy;
	}
	
	public ConcurrentHashMap<Integer, ElevatorData> getElevatorControllerData(){
		return elevatorCars;
	}
	
	public void updateECStatus(int elevatorId, ECStatusMessage status){
		ElevatorData elevator = elevatorCars.get(Integer.valueOf(elevatorId));
		elevator.setDirection(status.getDirection());
		elevator.setPosition(status.getPosition());
		elevator.setSpeed(status.getSpeed());
		elevator.setDestinations(status.getDestinations());
	}

	public void removeElevator(int elevatorId){
		updaterRunnables.remove(Integer.valueOf(elevatorId));
		
		//reassign hall calls
		ElevatorData elevatorData = elevatorCars.remove(Integer.valueOf(elevatorId));
		if(elevatorData == null){
			Main.onError(new IllegalStateException("Attempting to remove an invalid elevator"));
		}
		
		Destination[] destinations = elevatorData.getDestinations();
		
		if(destinations != null){
			for(int i = 0; i<destinations.length; i++){
				this.onHallCall(destinations[i].getFloor(), destinations[i].getDirection());
			}
		}
		//handle UI change
		UDPConnectionManager.getInstance().sendData(Constants.GD_TO_GUI_UDP_PORT, new RemoveElevatorMessage(elevatorId).serialize());
		
		System.out.println("Elevator " + elevatorId + " successfully removed.");
	}
	
	
	public void update(Observable observable, Object arg1) {
		if(observable instanceof TCPConnectionManager){
			if(arg1 instanceof TCPClientSocketWrapper){
				onConnectionCreated((TCPClientSocketWrapper) arg1);
			}
		}
	}
}