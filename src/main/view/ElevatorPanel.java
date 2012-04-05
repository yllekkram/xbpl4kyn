package main.view;

import javax.swing.*;

import main.util.Constants;
import main.util.Log;
import main.view.util.UIUtils;


import java.awt.*;
import java.awt.event.*;


public class ElevatorPanel extends JScrollPane implements ActionListener {
	
	private static final long serialVersionUID = 2562440202529823914L;
	
	private JPanel buttonGrid, delegate;
	private FloorButton[] floorButtons;
	private JButton stopButton, openDoorButton, closeDoorButton;
	private JLabel floorIndicator;
	
	private int carNumber;
	private int floor, direction;
	
    public ElevatorPanel(int carNumber, int initialFloor) {
    	super();
    	
    	setLayout(new ScrollPaneLayout());
    	setPreferredSize(new Dimension(300,400));
    	
    	GridBagLayout gridBagLayout = new GridBagLayout();
    	delegate = new JPanel(gridBagLayout);
        GridBagConstraints gridBagContraints = new GridBagConstraints();
        
    	GridLayout gridLayout = new GridLayout(0, 2);
    	buttonGrid = new JPanel(gridLayout);
    	buttonGrid.setBackground(Color.GRAY);
    	gridLayout.setHgap(UIUtils.PADDING_HORIZONTAL_PX); gridLayout.setVgap(UIUtils.PADDING_VERTICAL_PX);
    	UIUtils.addEmptyBorder(buttonGrid);
    	
    	floorButtons = new FloorButton[Constants.FLOOR_COUNT];
    	
    	//create the floor buttons
    	floorButtons[0] = new FloorButton("B", 0);
    	floorButtons[0].addActionListener( this );
    	buttonGrid.add(floorButtons[0]);
    	for(int i=1; i<Constants.FLOOR_COUNT; i++) {
    		floorButtons[i] = new FloorButton("" + i, i);
    		floorButtons[i].addActionListener( this );
    		buttonGrid.add(floorButtons[i]);
    	}
    	
    	//create the stop button
    	stopButton = new JButton("Stop");
    	stopButton.addActionListener( this );
    	buttonGrid.add(stopButton);
    	
    	//create the "open door" button
    	openDoorButton = new JButton(UIUtils.LEFT_ARROW + UIUtils.RIGHT_ARROW);
    	openDoorButton.addActionListener( this );
    	buttonGrid.add(openDoorButton);
    	
    	//create the "open door" button
    	closeDoorButton = new JButton(UIUtils.RIGHT_ARROW + UIUtils.LEFT_ARROW);
    	closeDoorButton.addActionListener( this );
    	buttonGrid.add(closeDoorButton);
    	
    	//create the floor indicator screen
    	floorIndicator = new JLabel("");
    	setFloorAndDirection(0, Constants.DIRECTION_UP);
    	floorIndicator.setFont(new Font("Serif", Font.PLAIN, 36));
    	JComponent indicatorBox = UIUtils.centralizeComponent(floorIndicator);
    	gridBagContraints.gridwidth = 0;
    	gridBagContraints.ipady = UIUtils.PADDING_VERTICAL_PX;
    	gridBagLayout.setConstraints(indicatorBox, gridBagContraints);
    	delegate.add(indicatorBox);
    	gridBagLayout.setConstraints(buttonGrid, gridBagContraints);
    	delegate.add(buttonGrid);
    	
    	this.carNumber = carNumber;
    	this.setViewportView(delegate);
    }
 
  	public void actionPerformed( ActionEvent event ) {
  		Object source = event.getSource();
  		if (source instanceof FloorButton){
  			FloorButton floorButton = (FloorButton) source;
  			setButtonPressed(floorButton.getFloorNumber(), true);
  			ViewControl.getInstance().onFloorSelection(carNumber, floorButton.getFloorNumber());
  		}else if (source == stopButton){
  			ViewControl.getInstance().onStopRequest(carNumber);
  		}else if (source == openDoorButton){
  			ViewControl.getInstance().onOpenDoorRequest(carNumber);
  		}else if (source == closeDoorButton){
  			ViewControl.getInstance().onCloseDoorRequest(carNumber);
  		}else{
  			Log.log("Unknown event");
  		}
  	}
  	
  	public void setFloor(int floor){
  		this.setFloorAndDirection(floor, this.direction);
  	}
  	
  	public void setDirection(int direction){
  		this.setFloorAndDirection(this.floor, direction);
  	}
  	
  	public void setFloorAndDirection(final int floor, int direction){
  		this.floor = floor;
  		this.direction = direction;
  		
  		final String directionArrowFinal = UIUtils.getDirectionArrow(direction);
  		SwingUtilities.invokeLater(new Runnable(){
  			public void run(){
  				floorIndicator.setText( floor + "  " + directionArrowFinal);
  			}
  		});
  		
  	}
  	
  	public void setButtonPressed(final int floorNumber, final boolean set){
  		SwingUtilities.invokeLater(new Runnable(){
  			public void run(){
  				floorButtons[floorNumber].setEnabled(!set);
  			}
  		});
  	}

}