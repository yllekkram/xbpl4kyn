package test;


import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class MessageTest extends TestCase{

	public void testMessages(){
		assertEquals(2, 2);
	}
	
	public static Test suite(){
		return new TestSuite(MessageTest.class);
	}
}
