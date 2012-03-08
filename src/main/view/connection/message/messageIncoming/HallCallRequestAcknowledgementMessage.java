package main.view.connection.message.messageIncoming;


public class HallCallRequestAcknowledgementMessage extends ViewMessageIncoming{
	
	public HallCallRequestAcknowledgementMessage(byte[] data){
		this.deserialize(data);
	}

	public void deserialize(byte[] data){
		//no-op
	}
}
