package main.groupDispatcher.connection;


import java.net.DatagramPacket;
import java.util.Arrays;

import main.Main;
import main.exception.UnexpectedEndOfMessageException;
import main.groupDispatcher.connection.message.GroupDispatcherMessageParser;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageIncoming.HallCallRequestMessage;
import main.groupDispatcher.control.GroupDispatcher;


public class UDPReceiveHandlerRunnable implements Runnable{

	private DatagramPacket inPacket;
	
	public UDPReceiveHandlerRunnable(DatagramPacket inPacket){
		this.inPacket = inPacket;
	}

	
	public void run() {
		System.out.println("Group dispatcher - message received from GUI:" + Arrays.toString(inPacket.getData()));
		
		//parse the received message and act appropriately
		GroupDispatcherMessageIncoming message;
		try {
			message = GroupDispatcherMessageParser.getInstance().parseMessage(inPacket.getData());
			if(message instanceof HallCallRequestMessage){
				HallCallRequestMessage hallCall = (HallCallRequestMessage) message;
				GroupDispatcher.getInstance().onHallCall(hallCall);
			}else{
				//ignore
				System.out.println("UDPReceiveHandlerRunnable - unexpected message type");
			}
		} catch (UnexpectedEndOfMessageException e) {
			Main.onError(e);
		}
	}
}
