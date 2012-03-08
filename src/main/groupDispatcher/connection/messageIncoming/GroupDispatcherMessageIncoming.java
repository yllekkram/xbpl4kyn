package main.groupDispatcher.connection.messageIncoming;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;

public abstract class GroupDispatcherMessageIncoming extends GroupDispatcherMessage{

	
	public abstract void deserialize(byte[] data);
}
