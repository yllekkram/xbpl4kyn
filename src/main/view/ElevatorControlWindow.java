package main.view;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.concurrent.ConcurrentHashMap;

import javax.swing.*;

import test.stress.MorningRush;

import main.util.*;
import main.view.util.UIUtils;


public class ElevatorControlWindow extends JFrame implements ActionListener{

	private static final long serialVersionUID = 3168789651745366452L;

	private ConcurrentHashMap<Integer, ElevatorPanel> elevatorPanels;
	private ConcurrentHashMap<Integer, FloorPanel> floorPanels;
	
	private JMenuItem startMenuItem, exitMenuItem;
	private JMenuItem morningRushTestMenuItem;
	private JButton startButton;
	private JPanel mainPanel;
	private JTabbedPane elevatorTabs;
	private JLabel noElevatorsWarningLabel;
	private boolean elevatorsConnected = false;
	
	private static ElevatorControlWindow instance;
	
	public synchronized static ElevatorControlWindow getInstance(){
		if(instance == null){
			instance = new ElevatorControlWindow();
		}
		return instance;
	}
	
	
	private ElevatorControlWindow(){
		super("Group 4 - Elevator Control System");
		setSize(650, 850);
				
		//initialize panels
		mainPanel = new JPanel();
		mainPanel.setLayout(new BorderLayout());
		mainPanel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
	    setContentPane(mainPanel);
	    elevatorTabs = new JTabbedPane(JTabbedPane.TOP, JTabbedPane.WRAP_TAB_LAYOUT);
		elevatorTabs.setTabLayoutPolicy(JTabbedPane.WRAP_TAB_LAYOUT);
		elevatorPanels = new ConcurrentHashMap<Integer, ElevatorPanel>();
		floorPanels = new ConcurrentHashMap<Integer, FloorPanel>();
		
		//set menu bar
    	JMenuBar menubar = new JMenuBar();
    	this.setJMenuBar( menubar );
    	
		// Set up the File menu
	    {
	    	//menu
	    	JMenu fileMenu = new JMenu("File");
	    	
	    	//start
	    	startMenuItem = new JMenuItem ("Start");
	    	startMenuItem.addActionListener(this);
	    	fileMenu.add( startMenuItem );
	    	
	    	//exit
	    	exitMenuItem = new JMenuItem("Exit");
	    	exitMenuItem.addActionListener(this);
	    	fileMenu.add( exitMenuItem );
	    	
	    	menubar.add( fileMenu );
	    }
	    
	    // Set up the stress test menu
	    {
	    	//menu
	    	JMenu stressTestMenu = new JMenu("Stress Tests");
	    	
	    	//morning rush
	    	morningRushTestMenuItem = new JMenuItem ("Morning Rush");
	    	morningRushTestMenuItem.addActionListener(this);
	    	stressTestMenu.add( morningRushTestMenuItem );
	    	
	    	menubar.add( stressTestMenu );
	    }
	    
	    //startPanel
	    startButton = new JButton("Start");
	    startButton.addActionListener(this);
	    
	    mainPanel.add(UIUtils.centralizeComponent(startButton), BorderLayout.CENTER);
	   	
	    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    setLocationRelativeTo(null);
	}
	
	public void display(){
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
	
	
	public void addElevatorPanel(final int title, final int initialFloor){
		SwingUtilities.invokeLater(new Runnable(){
			public void run(){
				if( !elevatorsConnected ){
					mainPanel.remove(noElevatorsWarningLabel.getParent());
					mainPanel.add(elevatorTabs, BorderLayout.EAST);
					elevatorsConnected = true;
				}
				ElevatorPanel carPanel = new ElevatorPanel(title, initialFloor);
				elevatorPanels.put(Integer.valueOf(title), carPanel);
				elevatorTabs.add("" + title, carPanel);
				mainPanel.validate();
				ElevatorControlWindow.this.repaint();
			}
		});
	}
	
	private void addFloorPanels(){
		JScrollPane scroll = new JScrollPane();
		scroll.setLayout(new ScrollPaneLayout());
    	scroll.setPreferredSize(new Dimension(260,400));
		mainPanel.add(scroll, BorderLayout.WEST);
		
		JPanel floorPanelContainer = new JPanel(new GridLayout(0, 1));
    	UIUtils.addEmptyBorder(floorPanelContainer);

		scroll.setViewportView(floorPanelContainer);
		scroll.getVerticalScrollBar().setUnitIncrement(16);
		
		
		//top floor
		FloorPanel floorPanel = new FloorPanel(Constants.FLOOR_COUNT, "Penthouse", Constants.DIRECTION_DOWN);
		floorPanels.put(Integer.valueOf(Constants.FLOOR_COUNT), floorPanel);
		floorPanelContainer.add(floorPanel);
		
		//middle floors
		for(int i=Constants.FLOOR_COUNT-1; i>=2; --i){
			floorPanel = new FloorPanel(i, "Floor "+i, Constants.DIRECTION_BOTH);
			floorPanels.put(Integer.valueOf(i), floorPanel);
			floorPanelContainer.add(floorPanel);
		}
		
		//bottom floor
		floorPanel = new FloorPanel(1, "Basement", Constants.DIRECTION_UP);
		floorPanels.put(Integer.valueOf(1), floorPanel);
		floorPanelContainer.add(floorPanel);
	}
	
	
	public void actionPerformed(ActionEvent event) {
		Object source = event.getSource();

		if( source == startMenuItem || source == startButton){
			ViewControl.getInstance().onStart();
		}else if( source == exitMenuItem ){
			ViewControl.getInstance().onExit();
		}
		
		else if( source == morningRushTestMenuItem ){
			MorningRush.run();
		}
	}
	
	public ElevatorPanel getElevatorPanel(int elevatorId){
		return this.elevatorPanels.get(Integer.valueOf(elevatorId));
	}
	
	public FloorPanel getFloorPanel(int floor){
		return this.floorPanels.get(Integer.valueOf(floor));
	}
}
