package main.view.connection.message.messageIncoming;


public class HallCallRequestAcknowledgmentMessage extends ViewMessageIncoming{
	
	public HallCallRequestAcknowledgmentMessage(byte[] data){
		this.deserialize(data);
	}

	public void deserialize(byte[] data){
		//no-op
	}
}
