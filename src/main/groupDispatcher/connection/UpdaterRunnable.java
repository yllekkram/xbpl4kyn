package main.groupDispatcher.connection;


import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.Arrays;

import main.Main;
import main.exception.UnexpectedEndOfMessageException;
import main.groupDispatcher.connection.message.GroupDispatcherMessageParser;
import main.groupDispatcher.connection.messageIncoming.ECStatusMessage;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageOutgoing.StatusRequestMessage;
import main.groupDispatcher.control.GroupDispatcher;
import main.util.Constants;
import main.util.Log;


public class UpdaterRunnable implements Runnable{

	int clientId;
	
	public UpdaterRunnable(int clientId){
		this.clientId = clientId;
	}
	
	public void run() {
		try {
			Log.log("Requesting status update from client " + clientId); 
			Socket clientSocket = TCPConnectionManager.getInstance().getClientConnectionSocket(clientId);
			TCPConnectionManager.getInstance().sendData(clientSocket, new StatusRequestMessage().serialize());
			byte[] line = TCPConnectionManager.getInstance().receiveData(clientSocket, Constants.STATUS_UPDATE_REQUEST_TIMEOUT);
			if(line != null){
				Log.log("Client " + clientId  + " said: " + Arrays.toString(line));
				GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.getInstance().parseMessage(line);
				if(message instanceof ECStatusMessage){
					ECStatusMessage status = (ECStatusMessage) message;
					GroupDispatcher.getInstance().updateECStatus( clientId, status );
				}else{
					Log.log("UpdaterRunnable - Unexpected message type from client " + clientId);
				}
				
			}else{
				Log.log("UpdaterRunnable received null data from client " + clientId);
			}
		} catch(UnexpectedEndOfMessageException e){
			Main.onError(e);
			GroupDispatcher.getInstance().removeElevator(clientId);
		} catch (SocketException e){
			Log.log("Received a SocketException in UpdaterRunnable for client " +  clientId + ". The elevator will be removed.");
			GroupDispatcher.getInstance().removeElevator(clientId);
		} catch (SocketTimeoutException e) {
			Log.log("Received a SocketTimeoutException in UpdaterRunnable for client " +  clientId + ". The elevator will be removed.");
			GroupDispatcher.getInstance().removeElevator(clientId);
		} catch (IOException e) { 
			Main.onFatalError(e); 
		}
	}
}