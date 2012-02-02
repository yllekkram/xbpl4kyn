package groupDispatcher.connection;

import java.net.DatagramPacket;

public class UDPReceiveHandlerRunnable implements Runnable{

	private DatagramPacket inPacket;
	
	public UDPReceiveHandlerRunnable(DatagramPacket inPacket){
		this.inPacket = inPacket;
	}

	
	public void run() {
		String inMessage = new String(inPacket.getData());
		
		System.out.println("Group dispatcher - message received from GUI: \"" + inMessage + "\"");
		
		//reply to sender
		UDPConnectionManager.getInstance().sendData(inPacket.getPort(), "Message received, thanks".getBytes());
		
		//TODO: parse the received message and act appropriately
	}
}
