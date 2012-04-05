package test.stress;

import main.util.Constants;
import main.view.ViewControl;

public class MorningRush {

	
	
	
	public static void run(){
		System.out.println("starting morning rush..");
		
		ViewControl viewControl = ViewControl.getInstance();
		viewControl.onHallCall(5, Constants.DIRECTION_DOWN);
	}
}
