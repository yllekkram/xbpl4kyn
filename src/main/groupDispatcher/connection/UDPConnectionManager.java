package main.groupDispatcher.connection;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import main.Main;
import main.util.Constants;


public class UDPConnectionManager {
	
	private DatagramSocket sendSocket, receiveSocket;
	
	private static UDPConnectionManager instance;
	private static boolean isAlive;
	
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
	         receiveSocket = new DatagramSocket(Constants.GUI_TO_GD_UDP_PORT);
	         isAlive = true;
	         receive();
	      } catch (SocketException e) {
	    	  Main.onFatalError(e);
	      }
	}
	
	public void sendData(int port, byte[] data){
		try {
			sendData(InetAddress.getLocalHost().getHostAddress(), port, data);
		} catch (UnknownHostException e) {
			Main.onFatalError(e);
		}
	}
	
	public void sendData(String ip, int port, byte[] data){
		try {
			DatagramPacket sendPacket = new DatagramPacket(data, data.length, InetAddress.getByName(ip), port);
			sendSocket.send(sendPacket);
		} catch (Exception e) {
			Main.onFatalError(e);
		}
	}

	public void receive(){
		new Thread( new Runnable(){
			public void run(){
				//continually loop, waiting for messages until something goes wrong
				while(true){
					try {
						if(!isAlive){
							break;
						}
						DatagramPacket inPacket = new DatagramPacket(new byte[Constants.MAX_MESSAGE_LENGTH], Constants.MAX_MESSAGE_LENGTH);
						receiveSocket.receive(inPacket);
						
						//create a new thread to handle the received message
						new Thread(new UDPReceiveHandlerRunnable(inPacket)).start();
					} catch (IOException e) {
						Main.onFatalError(e);
					}
				}
			}
		}).start();
		
	}
	
	public void destroy(){
		isAlive = false;
		if(sendSocket != null)
			sendSocket.close();
		if(receiveSocket != null)
			receiveSocket.close();
		instance = null;
	}
}
