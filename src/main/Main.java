package main;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import main.view.ElevatorControlWindow;

public class Main {

	
	
	
	public static void onError(Throwable th){
		th.printStackTrace();
	}
	
	public static void onFatalError(Throwable th){
		onError(th);
		System.exit(1);
	}
	
	
	public static void main(String[] args){
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InstantiationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (UnsupportedLookAndFeelException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		ElevatorControlWindow.getInstance().display();
	}
}
