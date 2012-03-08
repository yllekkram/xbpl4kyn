README

To launch the GroupDispatcher and GUI, import the GroupDispatcher (located at the root of code.zip) project into Eclipse. Find and open Main.java and run it as a 
Java application. When the UI appears, click start.

To compile the ElevatorController, run the following commands:
	# remove any leftover files from previous builds
	cd elevator_controller
	make clean
	# compile
	make
	
To run the ElevatorController, first start the GroupDispatcher and GUI from within Eclipse. Next, from the working directory, run
	./main <group_dispatcher_ip> <group_dispatcher_port> <gui_ip> <gui_port>
	
<group_dispatcher_port> is set to 5000
<gui_ip> is set to 5003
For example, ./testElevatorController 127.0.0.1 5000 127.0.0.1 5003

