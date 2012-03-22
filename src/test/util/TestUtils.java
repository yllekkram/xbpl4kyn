package test.util;

public class TestUtils {

	

	public static byte[] append(byte[] head, byte[] tail){
		int totalLength = head.length + tail.length;
		byte[] all = new byte[totalLength];
		
		System.arraycopy(head, 0, all, 0, head.length);
		System.arraycopy(tail, 0, all, head.length, tail.length);
		
		return all;
	}
	
	public static byte[] append(byte[] head, int[] tail){
		int totalLength = head.length + tail.length;
		byte[] all = new byte[totalLength];
		
		System.arraycopy(head, 0, all, 0, head.length);
		int currentAllIndex = head.length;
		for(int i=0; i<tail.length; i++){
			all[currentAllIndex] = (byte) tail[i];
			currentAllIndex++;
		}
		
		return all;
	}
	
	
	public static byte booleanToByte(boolean bool){
		if(bool){
			return 1;
		}
		return 0;
	}
	
	
	
}
