package test.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Arrays;

import main.util.Constants;

public class TCPTransmitter {

	Socket socket;

	private InputStream in;
	private OutputStream out;

	public TCPTransmitter(){
		initialize();
	}

	private void initialize(){
		try {
			socket = new Socket("127.0.0.1", Constants.GD_TCP_PORT);
			out = socket.getOutputStream();
			in = socket.getInputStream();
		} catch (IOException e) {
			e.printStackTrace();
		}    	 
	}

	public void send(byte[] data){
		try {
			out.write(data);
		} catch (IOException e) {
			System.out.println("Failed to send the data: " + Arrays.toString(data));
			e.printStackTrace();
		}
	}
	
	public byte[] receive(){
		byte[] data = new byte[Constants.MAX_MESSAGE_LENGTH];
		try {
			byte currentB = (byte) in.read();
			for(int currentIndex = 0; currentIndex < data.length && currentB != Constants.MESSAGE_DELIMITER; currentIndex++){
				data[currentIndex] = currentB;
				currentB = (byte) in.read();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		return data;
	}

	public void closeSockets(){
		try {
			out.close();
			in.close();
			socket.close();   
		} catch (IOException e) { 
			e.printStackTrace();
		}
	}
}
