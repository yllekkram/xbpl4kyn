CXX					= g++
CXXFLAGS		=
CXXINC			= -Iinclude
CXXLD				= $(CXX)
CXXLDFLAGS	=
RM					= rm

vpath %.hpp include:test/include

EXE			= ElevatorTestServer UDPTestServer testTCP testUDP testElevatorController
SRCS		= $(wildcard *.cpp)
OBJS		= ${SRCS:.cpp=.opp}

# clear out all the suffixes
.SUFFIXES:
# list only those that we use
.SUFFIXES: .opp .cpp

# define a suffix rule for .cpp -> .opp
.cpp.opp:
	$(CXX) $(CXXINC) -o $@ -c $< $(CXXFLAGS) 

all: $(EXE)

clean:
	-$(RM) -f $(EXE) $(OBJS) $(wildcard *.gch)

ElevatorTestServer: ElevatorCommon.opp ElevatorTestServer.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

testTCP: ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp testTCP.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
UDPTestServer: ElevatorCommon.opp UDPTestServer.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
testUDP: ElevatorCommon.opp ElevatorController.opp UDPView.opp testUDP.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
testElevatorController: ElevatorCommon.hpp ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp UDPView.opp testElevatorController.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

ElevatorCommon.opp: ElevatorCommon.hpp
ElevatorController.opp: ElevatorCommon.hpp
ElevatorControllerView.opp: ElevatorControllerView.hpp
ElevatorTestServer.opp: ElevatorCommon.hpp
FloorRunHeap.opp: FloorRunHeap.hpp
UDPTestServer.opp: ElevatorCommon.hpp
UDPView.opp: ElevatorControllerView.hpp UDPView.hpp
testElevatorController.opp: ElevatorCommon.hpp ElevatorController.hpp UDPView.hpp
testTCP.opp: ElevatorCommon.hpp
testUDP.opp: ElevatorCommon.hpp ElevatorController.hpp UDPView.hpp
