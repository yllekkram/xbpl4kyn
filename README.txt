README

GroupDispatcher and GUI:
	Import the directory containing the Java code, as an Existing Java Project into Eclipse.
	- To run the GroupDispatcher and GUI:
		Find main/Main.java and run it as a Java Application
		When the GUI appears, click start
	- To run the unit tests:
		Find test/TestAll.java and run it as a JUnit test suite

To compile the ElevatorController, run the following commands:
	# remove any leftover files from previous builds
	cd elevator_controller
	make clean
	# compile
	make
	
To run the ElevatorController, first start the GroupDispatcher and GUI from within Eclipse. Next, from the working directory, run
	./main <group_dispatcher_ip> <group_dispatcher_port> <gui_ip> <gui_port>
	
<group_dispatcher_port> is set to 5000
<gui_port> is set to 5003
For example, ./testElevatorController 127.0.0.1 5000 127.0.0.1 5003

