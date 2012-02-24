package groupDispatcher.connection;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import util.Constants;

public class UDPConnectionManager {
	
	private DatagramSocket sendSocket, receiveSocket;
	
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
	         receiveSocket = new DatagramSocket(Constants.GUI_TO_GD_UDP_PORT);
	         receive();
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

	public void receive(){
		new Thread( new Runnable(){
			public void run(){
				//continually loop, waiting for messages until something goes wrong
				while(true){
					try {
						DatagramPacket inPacket = new DatagramPacket(new byte[Constants.MAX_MESSAGE_LENGTH], Constants.MAX_MESSAGE_LENGTH);
						receiveSocket.receive(inPacket);
						
						//create a new thread to handle the received message
						new Thread(new UDPReceiveHandlerRunnable(inPacket)).start();
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
