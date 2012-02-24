package view.connection;

import groupDispatcher.connection.UDPConnectionManager;

import java.net.DatagramPacket;

public class GDRequestHandlerRunnable implements Runnable{

	private DatagramPacket inPacket;
	
	public GDRequestHandlerRunnable(DatagramPacket inPacket){
		this.inPacket = inPacket;
	}

	
	public void run() {
		String inMessage = new String(inPacket.getData());
		System.out.println("GUI - message received: \"" + inMessage + "\"");
		
		//reply to sender
		UDPConnectionManager.getInstance().sendData(inPacket.getPort(), "Message received, thanks".getBytes());
		
		//TODO: parse the received message and act appropriately
	}
}
