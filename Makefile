CC					= gcc
CFLAGS			=
LD					=	$(CC)
LDFLAGS			=
CPP					= g++
CPPFLAGS		=
CPPLD				= $(CPP)
CPPLDFLAGS	=
RM					= rm

EXE			= ElevatorController ElevatorTestServer
SRCS		= ElevatorTestServer.c
OBJS		= ${SRCS:.c=.o}

# clear out all the suffixes
.SUFFIXES:
# list only those that we use
.SUFFIXES: .o .c

# define a suffix rule for .c -> .o
.c.o:
	$(CC) $(CLFAGS) -c $<

all: $(EXE)

clean:
	-$(RM) -f $(EXE) $(OBJS) *.opp

ElevatorTestServer: ElevatorTestServer.o
	$(CC) $(CFLAGS) -o $@ ElevatorTestServer.o

ElevatorController: ElevatorController.opp
	$(CPP) $(CPPFLAGS) -o $@ ElevatorController.opp

ElevatorController.opp: ElevatorController.cpp ElevatorController.hpp
	$(CPP) $(CPPFLAGS) -o $@ -c ElevatorController.cpp
