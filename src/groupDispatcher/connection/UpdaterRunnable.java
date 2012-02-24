package groupDispatcher.connection;

import java.io.IOException;
import java.net.Socket;





public class UpdaterRunnable implements Runnable{

	int clientId;
	
	public UpdaterRunnable(int clientId){
		this.clientId = clientId;
	}
	
	public void run() {
		try {
			
			Socket clientSocket = TCPConnectionManager.getInstance().getClientConnectionSocket(clientId);
			TCPConnectionManager.getInstance().sendData(clientSocket, "Status update needed".getBytes());
			byte[] line = TCPConnectionManager.getInstance().receiveData(clientSocket);
			if(line != null){
				System.out.println("Client said: " + new String(line));
			}
		} catch (IOException e) { onError(e); }
	}
	
	public void onError(Exception e){
		e.printStackTrace(System.err);
		System.exit(1);
	}
	
}