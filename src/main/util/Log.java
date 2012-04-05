package main.util;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

public class Log {

	private static boolean logEnabled = true;
	
	/**
	 * Logs the given string to the standard output stream
	 * @param string
	 */
	public static synchronized void log(String string){
		if(logEnabled){
			System.out.println(string);			
		}
	}
	
	/**
	 * Enables or disables the logger
	 * @param enabled
	 */
	public static void setEnabled(boolean enabled){
		logEnabled = enabled;
	}
	
	
	
	private static Object fileMonitor = new Object();
	private static BufferedWriter out;
	private static String currentFileName;
	/**
	 * Appends the given string to the specified file
	 * @param fileName
	 * @param string
	 */
	public static void logStats(String fileName, String string){
		synchronized(fileMonitor){			
			try {
				if(out == null || !currentFileName.equals(fileName)){
					currentFileName = fileName;
					FileWriter fstream = new FileWriter(fileName);
					out = new BufferedWriter(fstream);
				}
				out.write(string);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Closes file output streams used to log stats
	 */
	public static void closeStatsLogger(){
		synchronized(fileMonitor){			
			currentFileName = null;
			try{
				if(out != null){					
					out.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
