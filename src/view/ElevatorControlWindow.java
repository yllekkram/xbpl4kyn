package view;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

import util.*;
import view.util.UIUtils;

public class ElevatorControlWindow extends JFrame implements ActionListener{

	private static final long serialVersionUID = 3168789651745366452L;

	private JMenuItem startMenuItem, exitMenuItem, connectMenuItem;
	private JButton startButton;
	private JPanel mainPanel;
	private JTabbedPane elevatorTabs;
	private JLabel noElevatorsWarningLabel;
	private boolean elevatorsConnected = false;
	
	private static ElevatorControlWindow instance;
	
	private int elevatorNumber = 1;
	
	public synchronized static ElevatorControlWindow getInstance(){
		if(instance == null){
			instance = new ElevatorControlWindow();
		}
		return instance;
	}
	
	
	private ElevatorControlWindow(){
		super("Group 4 - Elevator Control System");
		setSize(1000, 600);
				
		//initialize panels
		mainPanel = new JPanel();
		mainPanel.setLayout(new BorderLayout());
		mainPanel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
	    setContentPane(mainPanel);
	    elevatorTabs = new JTabbedPane(JTabbedPane.TOP, JTabbedPane.WRAP_TAB_LAYOUT);
		elevatorTabs.setTabLayoutPolicy(JTabbedPane.WRAP_TAB_LAYOUT);
		// Set up the File menu
	    {
	    	//menu
	    	JMenu fileMenu = new JMenu("File");
	    	//start
	    	startMenuItem = new JMenuItem ("Start");
	    	startMenuItem.addActionListener(this);
	    	//exit
	    	exitMenuItem = new JMenuItem("Exit");
	    	exitMenuItem.addActionListener(this);

	    	//connect
	    	connectMenuItem = new JMenuItem ("Connect");
	    	connectMenuItem.addActionListener(this);
	    	
	    	//build
	    	fileMenu.add( startMenuItem );
	    	fileMenu.add( exitMenuItem );
	    	fileMenu.add( connectMenuItem );
	    	JMenuBar menubar = new JMenuBar();
	    	menubar.add( fileMenu );
	    	this.setJMenuBar( menubar );
	    }
	    
	    //startPanel
	    startButton = new JButton("Start");
	    startButton.addActionListener(this);
	    
	    mainPanel.add(UIUtils.centralizeComponent(startButton), BorderLayout.CENTER);
	   	
	    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    setLocationRelativeTo(null);
		setVisible(true);
	}
	

	public void onStart(){
		SwingUtilities.invokeLater(new Runnable(){
			public void run(){
				//remove start button
				startButton.removeActionListener(ElevatorControlWindow.this);
				mainPanel.remove(startButton.getParent());
				
				//remove start menu item
				startMenuItem.removeActionListener(ElevatorControlWindow.this);
				startMenuItem.getParent().remove(startMenuItem);
				
				noElevatorsWarningLabel = new JLabel("No elevators connected");
				mainPanel.add(UIUtils.centralizeComponent(noElevatorsWarningLabel), BorderLayout.EAST);
				addFloorPanels();
				mainPanel.validate();
				ElevatorControlWindow.this.repaint();
			}
		});
		
		
	}
	
	
	public void addCar(final int title, final int initialFloor){
		SwingUtilities.invokeLater(new Runnable(){
			public void run(){
				if( !elevatorsConnected ){
					mainPanel.remove(noElevatorsWarningLabel.getParent());
					mainPanel.add(elevatorTabs, BorderLayout.EAST);
					elevatorsConnected = true;
				}
				elevatorTabs.add("" + title, new CarPanel(title, initialFloor));
				mainPanel.validate();
				ElevatorControlWindow.this.repaint();
			}
		});
	}
	
	private void addFloorPanels(){
		JScrollPane scroll = new JScrollPane();
		scroll.setLayout(new ScrollPaneLayout());
    	scroll.setPreferredSize(new Dimension(200,400));
		mainPanel.add(scroll, BorderLayout.WEST);
		
		JPanel floorPanels = new JPanel(new GridLayout(0, 1));
		scroll.setViewportView(floorPanels);
		
		
		//top floor
		floorPanels.add(new FloorPanel(Constants.FLOOR_COUNT, "Penthouse", FloorPanel.DIRECTION_DOWN));
		
		//middle floors
		for(int i=Constants.FLOOR_COUNT-1; i>=2; --i){
			floorPanels.add(new FloorPanel(i, "Floor "+i, FloorPanel.DIRECTION_BOTH));
		}
		
		//bottom floor
		floorPanels.add(new FloorPanel(1, "Basement", FloorPanel.DIRECTION_UP));
	}
	
	
	public void actionPerformed(ActionEvent event) {
		Object source = event.getSource();
		
		if( source == startMenuItem || source == startButton){
			ElevatorMonitor.getInstance().onStart();
		}else if( source == exitMenuItem ){
			ElevatorMonitor.getInstance().onExit();
		}else if( source == connectMenuItem ){
			addCar(elevatorNumber++, 0);
		}
	}
	
	
	
	
	public static void main(String[] args){
		ElevatorControlWindow.getInstance();
	}
}
