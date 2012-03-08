package main.view.connection.message;

public abstract class ViewMessage {

	//To GD
	public static final int HALLCALL_REQUEST = 7;

	//From GD
	public static final int HALLCALL_REQUEST_ACHNOWLEDGEMENT = 8;
	public static final int HALLCALL_SERVICED = 9;
	public static final int REMOVE_ELEVATOR = 18;
	
	//To EC
	public static final int REGISTRATION_ACKNOWLEDGEMENT = 10; 
	public static final int FLOOR_SELECTION = 11;
	public static final int OPEN_DOOR_REQUEST = 12;
	public static final int CLOSE_DOOR_REQUEST = 13;
	public static final int STOP_REQUEST = 14;
	
	//From EC
	public static final int REGISTRATION_REQUEST = 15;
	public static final int FLOOR_REACHED = 16;
	public static final int DIRECTION_CHANGE = 17;
	
}
