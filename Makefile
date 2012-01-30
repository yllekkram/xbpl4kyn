CC			= gcc
CFLAGS	=
LD			=	$(CC)
LDFLAGS	=
RM			= rm

EXE			= ElevatorController ElevatorTestServer
SRCS 		= ElevatorController.c ElevatorTestServer.c
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
	-$(RM) -f $(EXE) $(OBJS)

ElevatorController: ElevatorController.o
	gcc -o $@ ElevatorController.o

ElevatorTestServer: ElevatorTestServer.o
	gcc -o $@ ElevatorTestServer.o
