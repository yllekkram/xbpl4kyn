package groupDispatcher.connection;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import util.Constants;


/**
 * Instantiates new connections
 * @author jpmikhail
 *
 */
public class UpdaterConnectionRunnable implements Runnable{
	
	private static ServerSocket serverSocket = null;
	Socket clientSocket;
	
	public UpdaterConnectionRunnable(){
		if(serverSocket == null){
			try {
				serverSocket = new ServerSocket(Constants.GD_TCP_PORT);
			} catch (IOException e) {
				onError(e);
			}
		}
		
	}

	public void run(){
		try {
			System.out.println("waiting for a TCP connection from an ElevatorController..");
			clientSocket = serverSocket.accept();
			TCPConnectionManager.getInstance().onConnectionCreated(clientSocket);
		} catch (Exception e) { onError(e); }
	}
	
	public void onError(Exception e){
		try {
			serverSocket.close();
			clientSocket.close();
		} catch (Exception e1) {}
		e.printStackTrace(System.err);
		System.exit(1);
	}
}	