package test;

import static org.junit.Assert.*;

import main.groupDispatcher.control.GroupDispatcher;
import main.util.Log;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import test.util.TCPTransmitter;

public class UpdaterRunnableTest {

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
		tcpTransmitter.closeSockets();
		GroupDispatcher.getInstance().destroy();
	}
	
	@Test
	public void test() {
		
	}

}
