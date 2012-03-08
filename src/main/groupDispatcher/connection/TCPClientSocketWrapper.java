package main.groupDispatcher.connection;

import java.net.Socket;

public class TCPClientSocketWrapper{

	int clientId;
	Socket socket;
	
	public TCPClientSocketWrapper(int clientId, Socket socket){
		this.clientId = clientId;
		this.socket = socket;
	}
	
	public int getClientId(){
		return this.clientId;
	}
	
	public Socket getSocket(){
		return this.socket;
	}
}
