package main;

import main.view.ElevatorControlWindow;

public class Main {

	
	
	
	public static void onError(Throwable th){
		th.printStackTrace();
		System.exit(1);
	}
	
	public static void main(String[] args){
		ElevatorControlWindow.getInstance().display();
	}
}
