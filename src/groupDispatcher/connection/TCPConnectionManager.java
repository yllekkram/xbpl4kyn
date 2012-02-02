package groupDispatcher.connection;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Observable;
import java.util.concurrent.ConcurrentHashMap;



public class TCPConnectionManager extends Observable{

	public ConcurrentHashMap<Integer, ClientSocketWrapper> connections;

	private static int connectionCount = 1;
	
	private static TCPConnectionManager instance;
	
	private TCPConnectionManager(){
		connections = new ConcurrentHashMap<Integer, ClientSocketWrapper>();
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
			inData = receiveData(socket);
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
		if(inData == null){ //something went wrong, the socket has been closed
			return;
		}
		int clientId = (int)inData[0] & 0xff;
		System.out.println("onConnectionCreated(" + clientId + ")");

		ClientSocketWrapper clientSocketWrapper = new ClientSocketWrapper(clientId, socket);
		connections.put(Integer.valueOf(clientId), clientSocketWrapper);
		
		//notify the observers about the new connection
		setChanged();
		notifyObservers(clientSocketWrapper);
	}
	
	public Socket getClientConnectionSocket(int clientId){
		Integer key = Integer.valueOf(clientId);
		return connections.get(key).getSocket();
	}
	
	public void sendData(Socket socket, byte[] data) throws IOException{
		PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
		String dataStr = new String(data);
		out.println(dataStr);
		
	}
	
	public byte[] receiveData(Socket socket) throws IOException{
		BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		String line = in.readLine();
		if(line != null){
			return line.getBytes();
		}
		return null;
	}
	
	public void waitForNewConnection(){
		new Thread(new UpdaterConnectionRunnable()).start();
	}
	
	public int getNewClientNumber(){
		return connectionCount++;
	}
}
