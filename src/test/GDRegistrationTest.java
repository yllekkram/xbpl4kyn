package test;

import static org.junit.Assert.*;

import java.util.concurrent.ConcurrentHashMap;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;
import main.groupDispatcher.control.GroupDispatcher;
import main.model.ElevatorData;
import main.util.Constants;
import main.util.Log;

import org.junit.*;

import test.util.TCPTransmitter;


import junit.framework.TestSuite;

public class GDRegistrationTest extends TestSuite{

	TCPTransmitter tcpTransmitter;
	GroupDispatcher gd;
	
	@Before
	public void create(){
		Log.setEnabled(false);
		gd = GroupDispatcher.getInstance();
		gd.startUp(true, false);
		gd.setStatusUpdateRequestsEnabled(false);
		tcpTransmitter = new TCPTransmitter();
	}
	
	@After
	public void destroy(){
		GroupDispatcher.getInstance().destroy();
		tcpTransmitter.closeSockets();
	}
		
	@Test
	public void testGUIRegistration(){
		int elevatorId = 2;
		
		tcpTransmitter.send(new byte[]{GroupDispatcherMessage.REGISTRATION_REQUEST, (byte) elevatorId, Constants.MESSAGE_DELIMITER});
		
		assertEquals(tcpTransmitter.receive()[0], (byte) GroupDispatcherMessage.REGISTRATION_ACKNOWLEDGMENT);
		
		
		//make sure the elevator controller is added to the list
		ConcurrentHashMap<Integer, ElevatorData> ed = gd.getElevatorControllerData();
		
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) { e.printStackTrace(); }
		
		assertEquals( ed.size(), 1);
		assertNotNull( ed.get(Integer.valueOf(elevatorId)) );
	}
}
