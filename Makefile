CPP					= g++
CPPFLAGS		=
CPPLD				= $(CPP)
CPPLDFLAGS	=
RM					= rm

EXE			= testTCP ElevatorTestServer
SRCS		= ElevatorTestServer.cpp testTCP.cpp ElevatorCommon.cpp ElevatorController.cpp UDPView.cpp
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
	-$(RM) -f $(EXE) $(OBJS)

ElevatorTestServer.opp: ElevatorCommon.hpp ElevatorTestServer.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorTestServer.cpp

testTCP.opp: ElevatorCommon.hpp testTCP.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ testTCP.cpp

ElevatorCommon.opp: ElevatorCommon.hpp ElevatorCommon.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorCommon.cpp

ElevatorTestServer: ElevatorCommon.opp ElevatorTestServer.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorTestServer.opp ElevatorCommon.opp

testTCP: ElevatorCommon.hpp ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp UDPView.hpp UDPView.opp testTCP.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorController.opp UDPView.opp ElevatorCommon.opp testTCP.opp

ElevatorController.opp: ElevatorCommon.hpp ElevatorController.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorController.cpp

ElevatorControllerView.opp: ElevatorControllerView.hpp ElevatorControllerView.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorControllerView.cpp
	
UDPView.opp: ElevatorControllerView.hpp UDPView.hpp UDPView.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ UDPView.cpp
