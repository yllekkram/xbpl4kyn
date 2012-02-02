package view.connection;

import view.connection.UDPConnectionManager;

import java.net.DatagramPacket;

import view.ElevatorMonitor;

public class ECRequestHandlerRunnable implements Runnable{

	private DatagramPacket inPacket;
	
	public ECRequestHandlerRunnable(DatagramPacket inPacket){
		this.inPacket = inPacket;
	}

	
	public void run() {
		byte[] inBytes = inPacket.getData();
		String inMessage = new String(inBytes);
		System.out.println("GUI - message received: \"" + inMessage + "\"");
		
		//reply to sender
		UDPConnectionManager.getInstance().sendData(inPacket.getPort(), "Message received, thanks".getBytes());
				
		//add the elevator to the list of elevators
		int elevatorId = (int)inMessage.getBytes()[0] & 0xff;
		ElevatorMonitor.getInstance().onElevatorRegister(elevatorId, inPacket.getAddress().getHostAddress(), inPacket.getPort());
		
		//TODO: parse the received message and act appropriately
	}
}
