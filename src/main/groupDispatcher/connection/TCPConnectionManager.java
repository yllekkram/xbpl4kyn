package main.groupDispatcher.connection;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Arrays;
import java.util.Observable;
import java.util.concurrent.ConcurrentHashMap;

import main.Main;
import main.groupDispatcher.connection.message.GroupDispatcherMessageParser;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageIncoming.RegistrationRequestMessage;
import main.util.Constants;



public class TCPConnectionManager extends Observable{

	public ConcurrentHashMap<Integer, TCPClientSocketWrapper> connections;

	private static int connectionCount = 1;
	
	private static TCPConnectionManager instance;
	
	private TCPConnectionManager(){
		connections = new ConcurrentHashMap<Integer, TCPClientSocketWrapper>();
	}
	
	public synchronized static TCPConnectionManager getInstance(){
		if(instance == null){
			instance = new TCPConnectionManager();
		}
		return instance;
	}
	
	public void initialize(){
		waitForNewConnection();
	}
	
	public void onConnectionCreated(Socket socket){
		waitForNewConnection();
		
		byte[] inData = null;
		try {
			inData = receiveData(socket, 0);
		} catch (IOException e) {
			Main.onError(e);
		}
		GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.getInstance().parseMessage(inData);
		System.out.println("Client said: " + Arrays.toString(inData));
		if(message instanceof RegistrationRequestMessage){
			int clientId = ((RegistrationRequestMessage) message).getElevatorControllerId();
			System.out.println("onConnectionCreated(" + clientId + ")");
			
			TCPClientSocketWrapper clientSocketWrapper = new TCPClientSocketWrapper(clientId, socket);
			connections.put(Integer.valueOf(clientId), clientSocketWrapper);
			
			//notify the observers about the new connection
			setChanged();
			notifyObservers(clientSocketWrapper);

		}else{
			System.out.println("TCPConnectionManager.onConnectionCreated() - Unexpected message");
		}
	}
	
	public Socket getClientConnectionSocket(int clientId){
		Integer key = Integer.valueOf(clientId);
		return connections.get(key).getSocket();
	}
	
	public void sendData(int clientId, byte[] data){
		sendData(getClientConnectionSocket(clientId), data);
	}
	
	public void sendData(Socket socket, byte[] data){
		try {
			socket.getOutputStream().write(data);
		} catch (IOException e) {
			System.out.println("Failed to send the data: " + Arrays.toString(data));
			e.printStackTrace();
		}
	}
	
	public byte[] receiveData(Socket socket, int timeout) throws IOException, SocketTimeoutException{
		socket.setSoTimeout(timeout);
		InputStream in = socket.getInputStream();
		
		byte[] data = new byte[Constants.MAX_MESSAGE_LENGTH];
		try {
			byte currentB = (byte) in.read();
			for(int currentIndex = 0; currentIndex < data.length && currentB != Constants.MESSAGE_DELIMITER; currentIndex++){
				data[currentIndex] = currentB;
				currentB = (byte) in.read();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		return data;
	}
	
	public void waitForNewConnection(){
		new Thread(new TCPConnectionCreationRunnable()).start();
	}
	
	public int getNewClientNumber(){
		return connectionCount++;
	}
}
