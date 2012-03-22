package test;

import static org.junit.Assert.*;

import java.util.Random;


import main.groupDispatcher.connection.message.GroupDispatcherMessage;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageOutgoing.GroupDispatcherMessageOutgoing;
import main.groupDispatcher.control.GroupDispatcher;

import main.util.Constants;
import main.util.Log;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import test.util.TCPTransmitter;

public class UpdaterRunnableTest {

	TCPTransmitter[] tcpTransmitters;
	GroupDispatcher gd;
	private static final int CLIENT_COUNT = 15;
	private static final int REQUEST_COUNT = 20;
	
	
	@Before
	public void create(){
		Log.setEnabled(false);
		gd = GroupDispatcher.getInstance();
		gd.startUp(true, false);
		tcpTransmitters = new TCPTransmitter[CLIENT_COUNT];
		for(int i=0; i<CLIENT_COUNT; i++){
			tcpTransmitters[i] = new TCPTransmitter();
		}
	}
	
	@After
	public void destroy(){
		for(int i=0; i<CLIENT_COUNT; i++){
			tcpTransmitters[i].closeSockets();
		}
		GroupDispatcher.getInstance().destroy();
	}
	
	@Test
	public void test() {
		Thread[] threads = new Thread[CLIENT_COUNT];
		for(int i=0; i<CLIENT_COUNT; i++){
			final int index = i;
			threads[i] = new Thread( new Runnable(){
				public void run(){
					int elevatorId = index + 1;
					
					//register
					sleep();
					tcpTransmitters[index].send(new byte[]{GroupDispatcherMessage.REGISTRATION_REQUEST, (byte) elevatorId, Constants.MESSAGE_DELIMITER});
					sleep();
					tcpTransmitters[index].receive();
					
					int count = 1;
					long lastRequestTime = System.currentTimeMillis();
					
					//loop and measure time between requests for status update
					while(count <= REQUEST_COUNT){
						byte[] requestBytes = tcpTransmitters[index].receive();
						long thisRequestTime = System.currentTimeMillis();
						assertEquals(GroupDispatcherMessageOutgoing.STATUS_REQUEST, requestBytes[0]);
						
						//respond to the status update request
						tcpTransmitters[index].send(new byte[]{
								GroupDispatcherMessageIncoming.EC_STATUS,
								(byte) 3, //position
								(byte) 2, //direction
								(byte) 0, //isMoving
								(byte) 0, //hallCalls
								(byte) 0,  //floorSelections
								(byte) Constants.MESSAGE_DELIMITER
						});
						long difference = thisRequestTime - lastRequestTime;
						long upperLimit = Constants.STATUS_UPDATE_REQUEST_INTERVAL + 25;
						long lowerLimit = Constants.STATUS_UPDATE_REQUEST_INTERVAL - 25;
						if(count != 1){
							//skip the first one
							assertTrue(difference <= upperLimit && difference >= lowerLimit);
						}
						lastRequestTime = thisRequestTime;
						count++;
					}
				}});
			threads[i].start();
		}
		
		//join the threads
		for(int i=0; i<CLIENT_COUNT; i++){
			try {
				threads[i].join();
			} catch (InterruptedException e) { e.printStackTrace(); }
		}
	}

	private void sleep(){
		try {
			Random random = new Random();
			Thread.sleep((long) (random.nextFloat() * 100 + 50)); //give some expected delay
		} catch (InterruptedException e) { e.printStackTrace(); }
	}


}
