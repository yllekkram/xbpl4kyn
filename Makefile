CC			= gcc
CFLAGS	=
LD			=	$(CC)
LDFLAGS	=
RM			= rm

EXE			= ElevatorController ElevatorTestServer ElevatorControllerCPP
SRCS 		= ElevatorController.c ElevatorTestServer.c
OBJS		= ${SRCS:.c=.o}

# clear out all the suffixes
.SUFFIXES:
# list only those that we use
.SUFFIXES: .o .c .cpp

# define a suffix rule for .c -> .o
.c.o:
	$(CC) $(CLFAGS) -c $<

all: $(EXE)

clean:
	-$(RM) -f $(EXE) $(OBJS)

ElevatorController: ElevatorController.o
	$(CC) -o $@ ElevatorController.o

ElevatorTestServer: ElevatorTestServer.o
	$(CC) -o $@ ElevatorTestServer.o

ElevatorControllerCPP: ElevatorController.opp
	g++ -o $@ ElevatorController.o

ElevatorController.opp: ElevatorController.cpp ElevatorController.h
	g++ -c -o $@ ElevatorController.cpp
