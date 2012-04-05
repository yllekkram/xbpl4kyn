package main.view;

import javax.swing.*;
import javax.swing.border.Border;

import main.util.Constants;
import main.util.Log;
import main.view.util.UIUtils;


import java.awt.*;
import java.awt.event.*;

public class FloorPanel extends JPanel implements ActionListener { 

	private static final long serialVersionUID = 8967073701202397640L;
	
	private JButton up;
	private JButton down;
	private int floorNumber;
	
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
 	}
  
 
  	public void actionPerformed( ActionEvent e ) {
  		if (e.getSource().equals( up )) {
  			ViewControl.getInstance().onHallCall(floorNumber, Constants.DIRECTION_UP);
  		} else if(e.getSource().equals( down )){
  			ViewControl.getInstance().onHallCall(floorNumber, Constants.DIRECTION_DOWN);
  		}
  	}
  	
  	public void enableButton(int direction, boolean set){
  		if(direction == Constants.DIRECTION_UP){
  			up.setEnabled(set);
  		}else if(direction == Constants.DIRECTION_DOWN){
  			down.setEnabled(set);
  		}else{
  			Log.log("FloorPanel.enableButton() - Unexpected direction " + direction);
  		}
  	}
}

