package test.util;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

import main.util.Constants;

public class UDPTransmitter {

	private DatagramSocket sendSocket, receiveSocket;

	public UDPTransmitter(){
		this.initialize();
	}

	public void initialize(){
		try {
			sendSocket = new DatagramSocket();
			receiveSocket = new DatagramSocket(Constants.GD_TO_GUI_UDP_PORT);
			receiveFromGD();
		} catch (SocketException se) {
			se.printStackTrace();
		}
	}

	public void sendData(int port, byte[] data){
		try {
			DatagramPacket sendPacket = new DatagramPacket(data, data.length, InetAddress.getLocalHost(), port);
			sendSocket.send(sendPacket);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void receiveFromGD(){
		try {
			DatagramPacket inPacket = new DatagramPacket(new byte[Constants.MAX_MESSAGE_LENGTH], Constants.MAX_MESSAGE_LENGTH);
			receiveSocket.receive(inPacket);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void closeSockets(){
		try{
			sendSocket.close();
			receiveSocket.close();
		}catch(Exception e){
			
		}
	}
}
