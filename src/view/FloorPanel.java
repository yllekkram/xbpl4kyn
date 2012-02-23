package view;

import javax.swing.*;
import javax.swing.border.Border;

import util.Constants;
import view.util.UIUtils;

import java.awt.*;
import java.awt.event.*;

public class FloorPanel extends JPanel implements ActionListener { 

	private static final long serialVersionUID = 8967073701202397640L;
	
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
     	
 
     	if(directions==Constants.DIRECTION_UP || directions==Constants.DIRECTION_BOTH){
     		//up button
         	up = new JButton( UIUtils.UP_ARROW );
         	add( up );
         	up.addActionListener( this );
     	}
     	
     	if(directions==Constants.DIRECTION_DOWN || directions==Constants.DIRECTION_BOTH){
     		//down button
         	down = new JButton ( UIUtils.DOWN_ARROW );
         	add( down );
         	down.addActionListener( this );
     	}
     	
     	this.floorNumber = floorNumber;
     	this.title = title;
 	}
  
 
  	public void actionPerformed( ActionEvent e ) {
  		if (e.getSource().equals( up )) {
  			ElevatorMonitor.getInstance().onHallCall(floorNumber, Constants.DIRECTION_UP);
  		} else {
  			ElevatorMonitor.getInstance().onHallCall(floorNumber, Constants.DIRECTION_DOWN);
  		}
  	}
}

