package main.view.connection;


import java.net.DatagramPacket;
import java.util.Arrays;

import main.Main;
import main.exception.UnexpectedEndOfMessageException;
import main.view.ViewControl;
import main.view.connection.message.ViewMessageParser;
import main.view.connection.message.messageIncoming.HallCallRequestAcknowledgmentMessage;
import main.view.connection.message.messageIncoming.HallCallServicedMessage;
import main.view.connection.message.messageIncoming.RemoveElevatorMessage;
import main.view.connection.message.messageIncoming.ViewMessageIncoming;

public class GDRequestHandlerRunnable implements Runnable{

	private DatagramPacket inPacket;
	
	public GDRequestHandlerRunnable(DatagramPacket inPacket){
		this.inPacket = inPacket;
	}

	
	public void run() {
		System.out.println("GUI - message received from GD: " + Arrays.toString(inPacket.getData()));
		
		try {
			//parse the message
			ViewMessageIncoming message = ViewMessageParser.getInstance().parseMessage(inPacket.getData());
			
			//handle the message
			if( message instanceof RemoveElevatorMessage){
				RemoveElevatorMessage message2 = (RemoveElevatorMessage) message;
				ViewControl.getInstance().onElevatorError( message2.getElevatorId() );
			}else if(message instanceof HallCallServicedMessage){
				HallCallServicedMessage message2 = (HallCallServicedMessage) message;
				ViewControl.getInstance().onHallCallServiced(message2.getFloor(), message2.getDirection());
			}else if(message instanceof HallCallRequestAcknowledgmentMessage){
				System.out.println("GDRequestHandlerRunnable - HallCall request acknowledgment received");
			}else{
				System.out.println("GDRequestHandlerRunnable - Unexpected message received");
			}
		} catch (UnexpectedEndOfMessageException e) {
			Main.onError(e);
			
		}
		
		
	}
}
