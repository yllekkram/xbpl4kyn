package main.exception;

public class UnexpectedEndOfMessageException extends Exception {

	private static final long serialVersionUID = -1307890122502650258L;

	
	public UnexpectedEndOfMessageException(){
		super();
	}
	
	public UnexpectedEndOfMessageException(String message){
		super(message);
	}
}
