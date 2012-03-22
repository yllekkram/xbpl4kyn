package main.groupDispatcher.connection;


import java.net.DatagramPacket;
import java.util.Arrays;

import main.Main;
import main.exception.UnexpectedEndOfMessageException;
import main.groupDispatcher.connection.message.GroupDispatcherMessageParser;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageIncoming.HallCallRequestMessage;
import main.groupDispatcher.control.GroupDispatcher;
import main.util.Log;


public class UDPReceiveHandlerRunnable implements Runnable{

	private DatagramPacket inPacket;
	
	public UDPReceiveHandlerRunnable(DatagramPacket inPacket){
		this.inPacket = inPacket;
	}

	
	public void run() {
		Log.log("Group dispatcher - message received from GUI:" + Arrays.toString(inPacket.getData()));
		
		//parse the received message and act appropriately
		GroupDispatcherMessageIncoming message;
		try {
			message = GroupDispatcherMessageParser.parseMessage(inPacket.getData());
			if(message instanceof HallCallRequestMessage){
				HallCallRequestMessage hallCall = (HallCallRequestMessage) message;
				GroupDispatcher.getInstance().onHallCall(hallCall);
			}else{
				//ignore
				Log.log("UDPReceiveHandlerRunnable - unexpected message type");
			}
		} catch (UnexpectedEndOfMessageException e) {
			Main.onError(e);
		}
	}
}
