#include <native/task.h>
#include <signal.h>
#include <sys/mman.h>

#include "ElevatorController.hpp"
#include "Exception.hpp"
#include "UDPView.hpp"

void run(void*);
void jog(void*);
void catch_signal(int);

#define NUM_TASKS 1
RT_TASK task[NUM_TASKS];

ElevatorController ec;
UDPView uv;

int main(int argc, char* argv[]) {
	mlockall(MCL_CURRENT|MCL_FUTURE);

	if (argc != 2) {
		std::cerr << "USAGE: testThreadedElevatorController <gd_ip>" << std::endl;
		exit(1);
	}

	// for (int i = 0; i < NUM_TASKS; i++) {
	// 	rt_task_create(&task[i], NULL, 0, 99, T_JOINABLE);
	// }

	ec.connectToGD(argv[1], 5000);
	
	uv.init("192.168.251.1", "5000");
	try {
		ec.addView(&uv);
	}
	catch (Exception e) {}

	uv.run();

	// rt_task_start(&task[0], &run, &ec);

	return 0;
}

void run(void* cookie) {
	while (true) {
		printf("%d ", ((ElevatorController*)cookie)->getID());
	}
}

void catch_signal(int sig) {
	for (int i = 0; i < NUM_TASKS; i++) {
		rt_task_delete(&task[i]);
	}
}
