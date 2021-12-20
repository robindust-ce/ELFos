#include <syscall_const1.h>
#include <thread_management.h>
#include <exception_handlers.h>
#include <kprint.h>
#include <uart.h>
#include <localtimer.h>
#include <tcb_queue.h>

extern unsigned int userspace_start;
extern unsigned int userstacks_end;

void sleep_svc (unsigned int * sp)
{
	if ( (int) sp[R0_CURRENT_STACK] > 0) {
		schedule_sleep(sp+MODE_SPECIFIC_SIZE, (int) sp[R0_CURRENT_STACK]);
		reset_timer();
	} else {
		kprintf("Not a valid sleeping time\n");
	}
}

void read_svc (unsigned int * sp)
{
	if (sp[R0_CURRENT_STACK] > userstacks_end || sp[R0_CURRENT_STACK] < userspace_start) {
		sp[R0_CURRENT_STACK] = -1;
		return;
	}

	int temp;
	if ((temp = get_from_buffer()) == -1) {
		change_thread(sp+MODE_SPECIFIC_SIZE, waitingc);
	} else {
		*((char *) sp[R0_CURRENT_STACK]) = temp;
		sp[R0_CURRENT_STACK] = 1;
	}
}

void clone_svc (unsigned int * sp)
{
	if (sp[R1_CURRENT_STACK] > userstacks_end || sp[R1_CURRENT_STACK] < userspace_start) {
		sp[R0_CURRENT_STACK] = -1;
		return;
	}
	int err = thread_create((void (*)(void *)) sp[R0_CURRENT_STACK], (void *) sp[R1_CURRENT_STACK], sp[R2_CURRENT_STACK], 'c');

	if (err != 0) {
		sp[R0_CURRENT_STACK] = -1;
		return;
	}

	if (get_running() == IDLE_ID) {
		change_thread(sp+MODE_SPECIFIC_SIZE, finished);
		reset_timer();
	}
	sp[R0_CURRENT_STACK] = 0;
}

void fork_svc (unsigned int * sp)
{
	if (sp[R1_CURRENT_STACK] > userstacks_end || sp[R1_CURRENT_STACK] < userspace_start) {
		sp[R0_CURRENT_STACK] = -1;
		return;
	}
	int err = thread_create((void (*)(void *)) sp[R0_CURRENT_STACK], (void *) sp[R1_CURRENT_STACK], sp[R2_CURRENT_STACK], 'f');

	if (err != 0) {
		sp[R0_CURRENT_STACK] = -1;
		return;
	}

	if (get_running() == IDLE_ID) {
		change_thread(sp+MODE_SPECIFIC_SIZE, finished);
		reset_timer();
	}
	sp[R0_CURRENT_STACK] = 0;
}

void exit_svc (unsigned int * sp)
{
	change_thread(sp+MODE_SPECIFIC_SIZE, finished);
	reset_timer();
}

void write_svc (unsigned int * sp)
{
	kprintf("%c", sp[R0_CURRENT_STACK]);
}

int usr_svc (unsigned int * sp, int svc_code)
{
	switch (svc_code) {
		case SLEEP_SVC:
			sleep_svc(sp);
			break;
		case READ_SVC:
			read_svc(sp);
			break;
		case WRITE_SVC:
			write_svc(sp);
			break;
		case CLONE_SVC:
			clone_svc(sp);
			break;
		case FORK_SVC:
			fork_svc(sp);
			break;
		case EXIT_SVC:
			exit_svc(sp);
			break;
		default:
			return -1;
	}
	return 0;
}

