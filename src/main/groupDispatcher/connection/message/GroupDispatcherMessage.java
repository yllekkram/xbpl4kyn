package main.groupDispatcher.connection.message;

public abstract class GroupDispatcherMessage {

	//To EC
	public static final int REGISTRATION_ACKNOWLEDGMENT = 1;
	public static final int STATUS_REQUEST = 2;
	public static final int HALLCALL_ASSIGNMENT = 3;
	
	//From EC
	public static final int REGISTRATION_REQUEST = 4;
	public static final int EC_STATUS = 5;

	//From GUI
	public static final int HALLCALL_REQUEST = 7;
	
	//To GUI
	public static final int HALLCALL_REQUEST_ACHNOWLEDGMENT = 8;
	public static final int HALLCALL_SERVICED = 9;
	public static final int REMOVE_ELEVATOR = 18;
}
