package view;

import javax.swing.*;
import javax.swing.border.EmptyBorder;

import util.Constants;
import view.util.UIUtils;

import java.awt.*;
import java.awt.event.*;


public class CarPanel extends JScrollPane implements ActionListener {
	
	private static final long serialVersionUID = 2562440202529823914L;
	
	private JPanel buttonGrid;
	private JPanel delegate;
	private FloorButton[] floorButtons;
	private JButton stopButton;
	private JButton openDoorButton;
	private JButton closeDoorButton;
	private JLabel floorIndicator;
	
	private int carNumber;
	
    public CarPanel(int carNumber, int initialFloor) {
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
    	buttonGrid.setBorder(new EmptyBorder(UIUtils.PADDING_HORIZONTAL_PX, UIUtils.PADDING_HORIZONTAL_PX, UIUtils.PADDING_HORIZONTAL_PX, UIUtils.PADDING_HORIZONTAL_PX));
    	
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
  			ElevatorMonitor.getInstance().onFloorSelection(carNumber, floorButton.getFloorNumber());
  		}else if (source == stopButton){
  			ElevatorMonitor.getInstance().onStopRequest(carNumber);
  		}else if (source == openDoorButton){
  			ElevatorMonitor.getInstance().onOpenDoorRequest(carNumber);
  		}else if (source == closeDoorButton){
  			ElevatorMonitor.getInstance().onCloseDoorRequest(carNumber);
  		}else{
  			System.out.println("Unknown event");
  		}
  	}
  	
  	public void setFloorAndDirection(final int floorNumber, int direction){
  		final String directionArrowFinal = UIUtils.getDirectionArrow(direction);
  		SwingUtilities.invokeLater(new Runnable(){
  			public void run(){
  				floorIndicator.setText( floorNumber + "  " + directionArrowFinal);
  				setButtonPressed(floorNumber, false);
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