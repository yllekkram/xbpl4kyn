CPP					= g++
CPPFLAGS		=
CPPLD				= $(CPP)
CPPLDFLAGS	=
RM					= rm

EXE			= ElevatorTestServer UDPTestServer testTCP testUDP testElevatorController main
SRCS		= ElevatorTestServer.cpp testTCP.cpp testUDP.cpp testElevatorController.cpp ElevatorCommon.cpp ElevatorController.cpp UDPView.cpp UDPTestServer.cpp FloorRunHeap.cpp ElevatorSimulator.cpp main.cpp
OBJS		= ${SRCS:.cpp=.opp}

# clear out all the suffixes
.SUFFIXES:
# list only those that we use
.SUFFIXES: .opp .cpp

# define a suffix rule for .cpp -> .opp
# .cpp.opp:
# 	$(CPP) $(CPPFLAGS) -c $<

all: $(EXE)

clean:
	-$(RM) -f $(EXE) $(OBJS) $(wildcard *.gch)

ElevatorTestServer.opp: ElevatorCommon.hpp ElevatorTestServer.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorTestServer.cpp

testTCP.opp: ElevatorCommon.hpp testTCP.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ testTCP.cpp

ElevatorCommon.opp: ElevatorCommon.hpp ElevatorCommon.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorCommon.cpp

ElevatorTestServer: ElevatorCommon.opp ElevatorTestServer.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorTestServer.opp ElevatorCommon.opp

testTCP: ElevatorCommon.hpp ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp testTCP.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorController.opp ElevatorCommon.opp testTCP.opp

ElevatorController.opp: ElevatorCommon.hpp ElevatorController.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorController.cpp

ElevatorControllerView.opp: ElevatorControllerView.hpp ElevatorControllerView.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorControllerView.cpp
	
UDPView.opp: ElevatorControllerView.hpp UDPView.hpp UDPView.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ UDPView.cpp
	
UDPTestServer: ElevatorCommon.opp UDPTestServer.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorCommon.opp UDPTestServer.opp
	
UDPTestServer.opp: ElevatorCommon.hpp UDPTestServer.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ UDPTestServer.cpp
	
testUDP: ElevatorCommon.opp ElevatorCommon.opp UDPView.opp testUDP.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorCommon.opp ElevatorController.opp UDPView.opp testUDP.opp
	
testUDP.opp: ElevatorCommon.hpp ElevatorController.hpp UDPView.hpp testUDP.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ testUDP.cpp
	
testElevatorController: ElevatorCommon.hpp ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp UDPView.opp testElevatorController.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorCommon.hpp ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp UDPView.opp testElevatorController.opp
	
testElevatorController.opp: ElevatorCommon.hpp ElevatorController.hpp UDPView.hpp testElevatorController.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ testElevatorController.cpp
	
FloorRunHeap.opp: FloorRunHeap.hpp FloorRunHeap.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ FloorRunHeap.cpp

ElevatorSimulator.opp: ElevatorSimulator.hpp ElevatorSimulator.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorSimulator.cpp

main.opp: main.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ main.cpp

main: main.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ main.opp
