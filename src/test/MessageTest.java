package test;

import static org.junit.Assert.*;

import main.exception.UnexpectedEndOfMessageException;
import main.groupDispatcher.connection.message.GroupDispatcherMessage;
import main.groupDispatcher.connection.message.GroupDispatcherMessageParser;
import main.groupDispatcher.connection.messageIncoming.ECStatusMessage;
import main.groupDispatcher.connection.messageIncoming.GroupDispatcherMessageIncoming;
import main.groupDispatcher.connection.messageIncoming.HallCallRequestMessage;
import main.groupDispatcher.connection.messageIncoming.RegistrationRequestMessage;
import main.groupDispatcher.connection.messageOutgoing.HallCallAssignmentMessage;
import main.groupDispatcher.connection.messageOutgoing.HallCallRequestAcknowledgmentMessage;
import main.groupDispatcher.connection.messageOutgoing.HallCallServicedMessage;
import main.groupDispatcher.connection.messageOutgoing.RegistrationAcknowledgmentMessage;
import main.groupDispatcher.connection.messageOutgoing.RemoveElevatorMessage;
import main.groupDispatcher.connection.messageOutgoing.StatusRequestMessage;
import main.model.Destination;
import main.util.Constants;

import org.junit.Test;

import test.util.TestUtils;

public class MessageTest {

	@Test
	public void testStatusRequestMessage() {
		StatusRequestMessage message = new StatusRequestMessage();
		byte[] messageBytes = message.serialize();
		
		byte[] expected = new byte[]{GroupDispatcherMessage.STATUS_REQUEST, Constants.MESSAGE_DELIMITER};

		assertArrayEquals(expected, messageBytes);
	}
	
	@Test
	public void testRemoveElevatorMessage() {
		int elevatorId = 10;
		RemoveElevatorMessage message = new RemoveElevatorMessage(elevatorId);
		byte[] messageBytes = message.serialize();
		
		byte[] expected = new byte[]{GroupDispatcherMessage.REMOVE_ELEVATOR, (byte) elevatorId, Constants.MESSAGE_DELIMITER};

		assertArrayEquals(expected, messageBytes);
	}
	
	@Test
	public void testRegistrationAcknowledgmentMessage() {
		RegistrationAcknowledgmentMessage message = new RegistrationAcknowledgmentMessage();
		byte[] messageBytes = message.serialize();
		
		byte[] expected = new byte[]{GroupDispatcherMessage.REGISTRATION_ACKNOWLEDGMENT, Constants.MESSAGE_DELIMITER};

		assertArrayEquals(expected, messageBytes);
	}

	@Test
	public void testHallCallServicedMessage() {
		int floor = 5;
		int direction = Constants.DIRECTION_DOWN;
		HallCallServicedMessage message = new HallCallServicedMessage(floor, direction);
		byte[] messageBytes = message.serialize();
		
		byte[] expected = new byte[]{GroupDispatcherMessage.HALLCALL_SERVICED, (byte) floor, (byte) direction, Constants.MESSAGE_DELIMITER};

		assertArrayEquals(expected, messageBytes);
	}

	@Test
	public void testHallCallRequestAcknowledgmentMessage() {
		HallCallRequestAcknowledgmentMessage message = new HallCallRequestAcknowledgmentMessage();
		byte[] messageBytes = message.serialize();
		
		byte[] expected = new byte[]{GroupDispatcherMessage.HALLCALL_REQUEST_ACHNOWLEDGMENT, Constants.MESSAGE_DELIMITER};

		assertArrayEquals(expected, messageBytes);
	}

	@Test
	public void testHallCallAssignmentMessage() {
		int floor = 5;
		int direction = Constants.DIRECTION_DOWN;
		HallCallAssignmentMessage message = new HallCallAssignmentMessage(floor, direction);
		byte[] messageBytes = message.serialize();
		
		byte[] expected = new byte[]{GroupDispatcherMessage.HALLCALL_ASSIGNMENT, (byte) floor, (byte) direction, Constants.MESSAGE_DELIMITER};

		assertArrayEquals(expected, messageBytes);
	}

	@Test
	public void testRegistrationRequestMessage() {
		//invalid data test
		byte[] testBytes = new byte[]{GroupDispatcherMessage.REGISTRATION_REQUEST};
		try {
			@SuppressWarnings("unused")
			GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.parseMessage(testBytes);
			fail("Expected an UnexpectedEndOfMessageException");
		} catch (UnexpectedEndOfMessageException e) {
			//ignore
		}
		
		//valid data test
		int elevatorId = 3;
		testBytes = new byte[]{GroupDispatcherMessage.REGISTRATION_REQUEST, (byte) elevatorId};
		try {
			GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.parseMessage(testBytes);
			assert(message instanceof RegistrationRequestMessage);
			RegistrationRequestMessage message2 = (RegistrationRequestMessage) message;
			assertEquals(elevatorId, message2.getElevatorControllerId());
		} catch (UnexpectedEndOfMessageException e) {
			fail("Caught an UnexpectedEndOfMessageException");
		}
	} 

	@Test
	public void testHallCallRequestMessage() {
		//invalid data test
		int floor = 4;
		int direction = Constants.DIRECTION_UP;
		byte[] testBytes = new byte[]{GroupDispatcherMessage.HALLCALL_REQUEST, (byte) floor};
		try {
			@SuppressWarnings("unused")
			GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.parseMessage(testBytes);
			fail("Expected an UnexpectedEndOfMessageException");
		} catch (UnexpectedEndOfMessageException e) {
			//ignore
		}

		//valid data test
		testBytes = new byte[]{GroupDispatcherMessage.HALLCALL_REQUEST, (byte) floor, (byte) direction, (byte) floor};
		try {
			GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.parseMessage(testBytes);
			assert(message instanceof HallCallRequestMessage);
			HallCallRequestMessage message2 = (HallCallRequestMessage) message;
			assertEquals(floor, message2.getFloorNumber());
			assertEquals(direction, message2.getDirection());
		} catch (UnexpectedEndOfMessageException e) {
			fail("Caught an UnexpectedEndOfMessageException");
		}
	}

	
	@Test
	public void testECStatusMessage() {
		int position = 2;
		int direction = Constants.DIRECTION_DOWN;
		boolean isMoving = false;
		Destination[] assignedHallCalls = new Destination[2];
		assignedHallCalls[0] = new Destination(1,  Constants.DIRECTION_UP);
		assignedHallCalls[1] = new Destination(12,  Constants.DIRECTION_DOWN);
		int[] floorSelections = new int[]{3, 6, 4, 2, 1};
		
		//invalid data test
		byte[] testBytes = new byte[]{GroupDispatcherMessage.EC_STATUS, (byte) position};
		try {
			@SuppressWarnings("unused")
			GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.parseMessage(testBytes);
			fail("Expected an UnexpectedEndOfMessageException");
		} catch (UnexpectedEndOfMessageException e) {
			//ignore
		}

		//valid data test
		testBytes = new byte[]{GroupDispatcherMessage.EC_STATUS, 1, (byte) position, (byte) direction, TestUtils.booleanToByte(isMoving), (byte) assignedHallCalls.length};
		for(int i=0; i<assignedHallCalls.length; i++){
			testBytes = TestUtils.append(testBytes, assignedHallCalls[i].serialize());
		}
		testBytes = TestUtils.append(testBytes, new byte[]{(byte) floorSelections.length});
		testBytes = TestUtils.append(testBytes, floorSelections);
		try {
			GroupDispatcherMessageIncoming message = GroupDispatcherMessageParser.parseMessage(testBytes);
			assert(message instanceof ECStatusMessage);
			ECStatusMessage message2 = (ECStatusMessage) message;
			assertEquals(position, message2.getPosition());
			assertEquals(direction, message2.getDirection());
			assertEquals(isMoving, message2.isMoving());
			assertArrayEquals(assignedHallCalls, message2.getHallCalls());
			assertArrayEquals(floorSelections, message2.getFloorSelections());
		} catch (UnexpectedEndOfMessageException e) {
			fail("Caught an UnexpectedEndOfMessageException");
		}
	}
}
