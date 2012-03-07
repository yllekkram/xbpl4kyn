CXX					= g++
CXXFLAGS		= -Wall
CXXINC			= -Iinclude -Itest/include
CXXLD				= $(CXX)
CXXLDFLAGS	=
RM					= rm

vpath %.hpp include:test/include

EXE			= ElevatorTestServer UDPTestServer testTCP testUDP testElevatorController main ElevatorSimulatorTest
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

main: main.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

tests: test/test.opp test/testElevatorController.opp test/testHeap.opp test/testMessage.opp Message.opp Heap.opp ElevatorCommon.opp
	$(CXX) $(CXXINC) -o test/test $^ test/lib/libUnitTest++.a $(CXXFLAGS)

clean:
	-$(RM) -f $(EXE) $(OBJS) $(wildcard *.gch) $(wildcard test/*.opp) test/test

ElevatorSimulatorTest: ElevatorSimulator.opp ElevatorSimulatorTest.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

ElevatorTestServer: ElevatorCommon.opp ElevatorTestServer.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

testTCP: ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp Message.opp testTCP.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
UDPTestServer: ElevatorCommon.opp UDPTestServer.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
testUDP: ElevatorCommon.opp ElevatorController.opp Message.opp UDPView.opp testUDP.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
testElevatorController: ElevatorCommon.hpp ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp Message.opp UDPView.opp testElevatorController.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

ElevatorCommon.opp: ElevatorCommon.hpp
ElevatorController.opp: ElevatorCommon.hpp Heap.hpp Message.hpp
ElevatorControllerView.opp: ElevatorControllerView.hpp
ElevatorTestServer.opp: ElevatorCommon.hpp
Heap.opp: Heap.hpp
Message.opp: Message.hpp ElevatorCommon.hpp
UDPTestServer.opp: ElevatorCommon.hpp
UDPView.opp: ElevatorControllerView.hpp UDPView.hpp
testElevatorController.opp: ElevatorCommon.hpp ElevatorController.hpp Message.hpp UDPView.hpp
testTCP.opp: ElevatorCommon.hpp
testUDP.opp: ElevatorCommon.hpp ElevatorController.hpp Message.hpp UDPView.hpp

test.opp: UnitTest++.h
testElevatorController.opp: ElevatorController.hpp ElevatorController.cpp
testHeap.opp: Heap.hpp
testMessage.opp: Message.hpp MessageFixtures.hpp

ElevatorSimulator.opp: ElevatorSimulator.hpp

ElevatorSimulatorTest.opp: ElevatorSimulator.hpp

