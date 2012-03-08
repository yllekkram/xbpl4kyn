package main.view.connection;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.concurrent.ConcurrentHashMap;

import main.util.Constants;


public class UDPConnectionManager {
	
	private DatagramSocket sendSocket, receiveSocketFromGD, receiveSocketFromEC;
	
	private ConcurrentHashMap<Integer, UDPElevatorAddress> elevatorAddressList;
	
	private static UDPConnectionManager instance;
	
	private UDPConnectionManager(){
		elevatorAddressList = new ConcurrentHashMap<Integer, UDPElevatorAddress>();
	}
	
	public synchronized static UDPConnectionManager getInstance(){
		if(instance == null){
			instance = new UDPConnectionManager();
		}
		return instance;
	}
	
	public void initialize(){
		try {
	         sendSocket = new DatagramSocket();
	         receiveSocketFromGD = new DatagramSocket(Constants.GD_TO_GUI_UDP_PORT);
	         receiveSocketFromEC = new DatagramSocket(Constants.EC_TO_GUI_UDP_PORT);
	         receiveFromGD();
	         receiveFromEC();
	      } catch (SocketException se) {
	         onError(se);
	      }
	}
	
	public void sendDataToElevator(int elevatorId, byte[] data){
		UDPElevatorAddress elevatorAddress = elevatorAddressList.get(Integer.valueOf(elevatorId));
		if(elevatorAddress == null){
			System.out.println("The IP address and port for this elevator were not found. The ElevatorController was not registered.");
		}else{
			sendData(elevatorAddress.getIp(), elevatorAddress.getPort(), data);
		}
	}
	
	public void sendData(int port, byte[] data){
		try {
			sendData(InetAddress.getLocalHost().getHostAddress(), port, data);
		} catch (UnknownHostException e) {
			onError(e);
		}
	}
	
	public void sendData(String ip, int port, byte[] data){
		try {
			DatagramPacket sendPacket = new DatagramPacket(data, data.length, InetAddress.getByName(ip), port);
			sendSocket.send(sendPacket);
		} catch (Exception e) {
			onError(e);
		}
	}

	/**
	 * Continually receive DatagramPackets from the group dispatcher
	 */
	public void receiveFromGD(){
		new Thread( new Runnable(){
			public void run(){
				//continually loop, waiting for messages until something goes wrong
				while(true){
					try {
						DatagramPacket inPacket = new DatagramPacket(new byte[Constants.MAX_MESSAGE_LENGTH], Constants.MAX_MESSAGE_LENGTH);
						receiveSocketFromGD.receive(inPacket);
						
						//create a new thread to handle the received message
						new Thread(new GDRequestHandlerRunnable(inPacket)).start();
					} catch (IOException e) {
						onError(e);
					}
				}
			}
		}).start();	
	}
	
	
	/**
	 * Continually receive DatagramPackets from the elevator controllers
	 */
	public void receiveFromEC(){
		new Thread( new Runnable(){
			public void run(){
				//continually loop, waiting for messages until something goes wrong
				while(true){
					try {
						DatagramPacket inPacket = new DatagramPacket(new byte[Constants.MAX_MESSAGE_LENGTH], Constants.MAX_MESSAGE_LENGTH);
						receiveSocketFromEC.receive(inPacket);
						
						//create a new thread to handle the received message
						new Thread(new ECRequestHandlerRunnable(inPacket)).start();
					} catch (IOException e) {
						onError(e);
					}
				}
			}
		}).start();	
	}
	
	public void onError(Exception e){
		e.printStackTrace();
        System.exit(1);
	}
	
	public void addElevatorAddress(int elevatorId, String ip, int port){
		this.elevatorAddressList.put(Integer.valueOf(elevatorId), new UDPElevatorAddress(elevatorId, ip, port));
	}
}
