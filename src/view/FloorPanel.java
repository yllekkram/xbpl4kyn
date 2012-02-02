package view;

import javax.swing.*;
import javax.swing.border.Border;

import java.awt.*;
import java.awt.event.*;

public class FloorPanel extends JPanel implements ActionListener { 

	private static final long serialVersionUID = 8967073701202397640L;
	
	private static final String UP_ARROW = "\u25B2";
	private static final String DOWN_ARROW = "\u25BC";
	
	public static final int DIRECTION_UP = 0;
	public static final int DIRECTION_DOWN = 1;
	public static final int DIRECTION_BOTH = 2;
	
	JButton up;
	JButton down;
	int floorNumber;
	String title;
	
    public FloorPanel(int floorNumber, String title, int directions ) {
    	super();
    	
     	setBackground(Color.GRAY);
     	setLayout(new GridLayout(1, 2));

     	
     	Border border = BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.BLACK), title);
     	setBorder(border);
     	
 
     	if(directions==DIRECTION_UP || directions==DIRECTION_BOTH){
     		//up button
         	up = new JButton(UP_ARROW);
         	add( up );
         	up.addActionListener( this );
     	}
     	
     	if(directions==DIRECTION_DOWN || directions==DIRECTION_BOTH){
     		//down button
         	down = new JButton (DOWN_ARROW);
         	add( down );
         	down.addActionListener( this );
     	}
     	
     	this.floorNumber = floorNumber;
     	this.title = title;
 	}
  
 
  	public void actionPerformed( ActionEvent e ) {
  		if (e.getSource().equals( up )) {
  			ElevatorMonitor.getInstance().onHallCall(floorNumber, ElevatorMonitor.HALLCALL_DIRECTION_UP);
  		} else {
  			ElevatorMonitor.getInstance().onHallCall(floorNumber, ElevatorMonitor.HALLCALL_DIRECTION_DOWN);
  		}
  	}
}

