package main.util;

public class Log {

	private static boolean logEnabled = true;
	
	public static void log(String string){
		if(logEnabled){
			System.out.println(string);			
		}
	}
	
	public static void setEnabled(boolean enabled){
		logEnabled = enabled;
	}
}
