package main;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import main.util.Log;
import main.view.ElevatorControlWindow;

public class Main {

	public static void onError(Throwable th){
		th.printStackTrace();
	}
	
	public static void onFatalError(Throwable th){
		onError(th);
		exit(1);
	}
	
	public static void exit(int status){
		Log.closeStatsLogger();
		System.exit(status);
	}
	
	
	public static void main(String[] args){
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception e) {
			e.printStackTrace();
		}
		ElevatorControlWindow.getInstance().display();
	}
}
