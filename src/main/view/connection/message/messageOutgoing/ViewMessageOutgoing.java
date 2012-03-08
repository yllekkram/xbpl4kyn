package main.view.connection.message.messageOutgoing;

import main.view.connection.message.ViewMessage;

public abstract class ViewMessageOutgoing extends ViewMessage {

	
	public abstract byte[] serialize();
}
