package main.groupDispatcher.connection;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import main.Main;
import main.util.Constants;
import main.util.Log;


public class TCPConnectionCreationRunnable implements Runnable{
	
	private static ServerSocket serverSocket = null;
	Socket clientSocket;
	
	public TCPConnectionCreationRunnable(){
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
			Log.log("waiting for a TCP connection from an ElevatorController..");
			clientSocket = serverSocket.accept();
			TCPConnectionManager.getInstance().onConnectionCreated(clientSocket);
		} catch (Exception e) { onError(e); }
	}
	
	public void onError(Exception e){
		try {
			serverSocket.close();
			clientSocket.close();
		} catch (Exception e1) {}
		Main.onFatalError(e);
	}
}	