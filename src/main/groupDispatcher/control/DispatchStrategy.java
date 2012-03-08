package main.groupDispatcher.control;

public interface DispatchStrategy {

	
	/**
	 * Selects an elevator to dispatch the given hall call to
	 * 
	 * @param floor Origin of the hall call
	 * @param direction Hall call direction
	 * @return Returns the elevator ID for which the hall call request should be sent 
	 */
	public int dispatch(int floor, int direction);
}
