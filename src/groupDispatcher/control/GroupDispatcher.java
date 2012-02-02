package groupDispatcher.control;

import groupDispatcher.connection.ClientSocketWrapper;
import groupDispatcher.connection.TCPConnectionManager;
import groupDispatcher.connection.UDPConnectionManager;
import groupDispatcher.connection.UpdaterRunnable;

import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import util.Constants;

import model.ElevatorControllerData;


public class GroupDispatcher implements Observer{

	private ExecutorService executor;
	private ConcurrentHashMap<Integer, Runnable> updaterRunnables;
	private ConcurrentHashMap<Integer, ElevatorControllerData> elevatorCars;
	
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
		elevatorCars = new ConcurrentHashMap<Integer, ElevatorControllerData>();
		TCPConnectionManager.getInstance().addObserver(this);
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
		timer.scheduleAtFixedRate(task, 0, Constants.REQUEST_INTERVAL);
		
		
	}
	
	public void onConnectionCreated(ClientSocketWrapper clientSocket){		
		//store elevator models
		ElevatorControllerData elevatorControllerData = new ElevatorControllerData();
		elevatorCars.put(Integer.valueOf(clientSocket.getClientId()), elevatorControllerData); //thread-safe
		
		//store elevator updater runnables
		UpdaterRunnable runnable = new UpdaterRunnable(clientSocket.getClientId());
		updaterRunnables.put(Integer.valueOf(clientSocket.getClientId()), runnable); //thread-safe
	}
	
	public void hallCall(int floor, int direction){
		
	}
	
	
	public static void main(String[] args){
		GroupDispatcher.getInstance().startUp();
	}

	public void update(Observable observable, Object arg1) {
		if(observable instanceof TCPConnectionManager){
			if(arg1 instanceof ClientSocketWrapper){
				onConnectionCreated((ClientSocketWrapper) arg1);
			}
		}
	}
}