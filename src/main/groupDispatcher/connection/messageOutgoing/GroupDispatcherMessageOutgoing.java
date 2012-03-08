package main.groupDispatcher.connection.messageOutgoing;

import main.groupDispatcher.connection.message.GroupDispatcherMessage;

public abstract class GroupDispatcherMessageOutgoing extends GroupDispatcherMessage{

	
	public abstract byte[] serialize();
}
