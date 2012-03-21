package main.view.connection.message.messageIncoming;

import main.exception.UnexpectedEndOfMessageException;
import main.view.connection.message.ViewMessage;

public abstract class ViewMessageIncoming extends ViewMessage {

	
	public abstract void deserialize(byte[] data) throws UnexpectedEndOfMessageException;
}
