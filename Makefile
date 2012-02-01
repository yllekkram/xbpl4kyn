CPP					= g++
CPPFLAGS		=
CPPLD				= $(CPP)
CPPLDFLAGS	=
RM					= rm

EXE			= main ElevatorTestServer
SRCS		= ElevatorTestServer.cpp main.cpp ElevatorCommon.cpp ElevatorController.cpp
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

main.opp: ElevatorCommon.hpp main.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ main.cpp

ElevatorCommon.opp: ElevatorCommon.hpp ElevatorCommon.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorCommon.cpp

ElevatorTestServer: ElevatorCommon.opp ElevatorTestServer.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorTestServer.opp ElevatorCommon.opp

main: ElevatorCommon.hpp ElevatorCommon.opp ElevatorController.hpp ElevatorController.opp main.opp
	$(CPPLD) $(CPPLDFLAGS) -o $@ ElevatorController.opp main.opp ElevatorCommon.opp

ElevatorController.opp: ElevatorCommon.hpp ElevatorController.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ ElevatorController.cpp
