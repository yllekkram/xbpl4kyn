package view.connection;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import util.Constants;

public class UDPConnectionManager {
	
	private DatagramSocket sendSocket, receiveSocketFromGD, receiveSocketFromEC;
	
	private static UDPConnectionManager instance;
	
	private UDPConnectionManager(){
		
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
}
