package view;

import javax.swing.JButton;

public class FloorButton extends JButton {
	
	private static final long serialVersionUID = -8291536227768390085L;
	private int floorNumber;
	
	public FloorButton(String label, int floorNumber) {
		super(label);
		this.floorNumber = floorNumber;
	}
	
	public int getFloorNumber() {
		return this.floorNumber;
	}
}