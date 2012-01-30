all: ElevatorController


clean:
	-rm -rf *.o
	-rm -rf ElevatorController

ElevatorController.o: ElevatorController.c
	gcc -c -o ElevatorController.o ElevatorController.c

ElevatorController: ElevatorController.o
	gcc -o ElevatorController ElevatorController.o
