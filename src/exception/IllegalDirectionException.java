package exception;

public class IllegalDirectionException extends IllegalArgumentException{

	private static final long serialVersionUID = 4171006166659993474L;

	public IllegalDirectionException(){
		super();
	}
	
	public IllegalDirectionException(String message){
		super(message);
	}
}
