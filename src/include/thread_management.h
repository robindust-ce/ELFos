#ifndef THREAD_MANAGEMENT_H
#define THREAD_MANAGEMENT_H

enum status {
	running = 1,
	sleeping,
	waitingc,
	ready,
	finished
};

typedef struct tcb {
	unsigned int usr_regs[13];
	unsigned int sp;
	unsigned int lr;
	unsigned int pc;
	unsigned int cpsr;
	enum status state;
	int id;
	int process_id;
	int sleep_time;
} tcb;

int thread_create(void (*func)(void *), const void * args, unsigned int args_size, char mode);
void tcb_to_stack (unsigned int * sp, int id);
void test_func(void * c);
void tcb_init();
void change_thread (unsigned int * sp, enum status state);
void set_runstate(enum status state);
void schedule_timer (unsigned int * sp);
void schedule_sleep (unsigned int * sp, int t);
void check_waitcq();

#endif

