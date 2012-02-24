package view.connection;

public class UDPElevatorAddress {

	private int elevatorId;
	private String ip;
	private int port;

	
	public UDPElevatorAddress(int elevatorId, String ip, int port){
		this.elevatorId = elevatorId;
		this.ip = ip;
		this.port = port;
	}
	
	
	public int getElevatorId() {
		return elevatorId;
	}

	public String getIp() {
		return ip;
	}
	
	public int getPort() {
		return port;
	}

}
