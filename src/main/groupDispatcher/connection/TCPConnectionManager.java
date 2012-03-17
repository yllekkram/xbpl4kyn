package main.groupDispatcher.connection;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Observable;
import java.util.concurrent.ConcurrentHashMap;

import main.Main;
import main.groupDispatcher.connection.message.GroupDispatcherMessageParser;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageIncoming.RegistrationRequestMessage;



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
		PrintWriter out;
		try {
			out = new PrintWriter(socket.getOutputStream(), true);
			String dataStr = new String(data);
			out.println(dataStr);
		} catch (IOException e) {
			Main.onError(e);
		}
		
	}
	
	public byte[] receiveData(Socket socket, int timeout) throws IOException, SocketTimeoutException{
		socket.setSoTimeout(timeout);
		BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		String line = in.readLine();
		if(line != null){
			return line.getBytes();
		}
		return null;
	}
	
	public void waitForNewConnection(){
		new Thread(new TCPConnectionCreationRunnable()).start();
	}
	
	public int getNewClientNumber(){
		return connectionCount++;
	}
}
