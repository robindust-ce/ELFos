#include <kprint.h>
#include <thread_management.h>
#include <tcb_queue.h>
#include <uart.h>
#include <MMU_init.h>
#include <linker_variables.h>
#include <context.h>

#define USR_CPSR 0x00000110

#define WAITC_TIMEOUT 40
#define PROC_SPACE_SIZE (1 << 18)

tcb all_threads[THREAD_CNT];

tcb idle_thread;

typedef struct usr_proc_space {
	unsigned int stack[PROC_SPACE_SIZE];
} usr_proc_space;

extern void exit();

extern unsigned int stack_size;

usr_proc_space __attribute__((section (".userstacks"))) user_stacks[PROCESS_COUNT+1]; //+1 for Idleing

int threads_per_proc[PROCESS_COUNT];

void __attribute__((section (".usertext"))) idle ()
{
	while(1) {
		asm volatile ("WFI ");
	}
}

void set_runstate(enum status state)
{
	if (get_running() >= 0) {
		tcb * thread = &all_threads[get_running()];
		thread->state = state;
	}
}

void __attribute__((section (".usertext"))) thread_terminate()
{
	exit();
	while(1) {
	}
}

void tcb_init ()
{
	for (int i = 0; i < 32; i++) {
		all_threads[i].id = i;
		all_threads[i].process_id = -1;
		save_to_queue(TERMQ, i);
		all_threads[i].state = finished;
		all_threads[i].sleep_time = 0;
	}

	for (int j = 0; j < 13; j++) {
		idle_thread.usr_regs[j] = 0;
	}
	idle_thread.id = IDLE_ID;
	idle_thread.process_id = -1;
	idle_thread.cpsr = USR_CPSR;
	idle_thread.sp = (userdata_start+MB_OFFSET);
	idle_thread.lr = (unsigned int) &thread_terminate;
	idle_thread.pc = (unsigned int) &idle;
	set_running(idle_thread.id);
	idle_thread.state = ready;
	idle_thread.sleep_time = 0;

	for (int k = 0; k < PROCESS_COUNT; k++) {
		save_to_queue(PROCESSQ, k);
	}

}

void stack_to_tcb (unsigned int * sp, int id)
{

	all_threads[id].sp = *sp ;
	sp++;
	all_threads[id].lr = *sp;
	sp++;
	all_threads[id].cpsr = *sp;
	sp++;

	for (int i = 0; i < 13; i++) {
		all_threads[id].usr_regs[i] = *sp;
		sp++;
	}

	all_threads[id].pc = (*sp);
}


void stack_copy (char ** sp, const void * args, unsigned int args_size)
{
	char * bytes = (char *) args;
	unsigned int pad = (args_size % 8) == 0 ? 0 : (8 - (args_size % 8));

	(*sp) -= args_size;

	for (unsigned int i = 0; i < args_size; i++) {
		*sp[i] = bytes[i];
	}
	(*sp) -= pad;
}

int thread_create(void (*func)(void *), const void * args, unsigned int args_size, char mode)
{
	int proc_id;
	if (mode == 'c') {
		proc_id = all_threads[get_running()].process_id;

		if (threads_per_proc[proc_id] >= 4) return -3;

	} else if (mode == 'f') {
		proc_id = get_from_queue(PROCESSQ);
		if (proc_id < 0) return -4;
		copy_data_section(proc_id);

	} else {
		return -3;
	}

	int g;
	if ((g = get_from_queue(TERMQ)) < 0){
		return -1;
	}
	tcb * new_thread = &all_threads[g];

	new_thread->pc = (unsigned int) func;

	new_thread->sp = (userdata_start+MB_OFFSET) - threads_per_proc[proc_id]*stack_size;
	new_thread->process_id = proc_id;
	threads_per_proc[proc_id]++;

	for (unsigned int i = 0; i < 13; i++) {
		new_thread->usr_regs[i] = 0;
	}

	new_thread->cpsr = USR_CPSR;

	new_thread->lr = (unsigned int) &thread_terminate;

	new_thread->usr_regs[0] = new_thread->sp - args_size;

	//in running queue packen
	if (save_to_queue(RUNQ, new_thread->id) != 0) {
		return -2;
	}

	//write args to usr stack
	stack_copy((char **) &(new_thread->sp), args, args_size);

	return 0;
}

void tcb_to_stack (unsigned int * sp, int id)
{
	tcb * thread = id == -1 ? &idle_thread : &all_threads[id];

	*sp = thread->sp;
	sp++;
	*sp = thread->lr;
	sp++;
	*sp = thread->cpsr;
	sp++;

	for (int i = 0; i < 13; i++) {
		*sp = thread->usr_regs[i];
		sp++;
	}

	*sp = thread->pc;
}

void change_thread (unsigned int * sp, enum status state)
{
	set_runstate(state);

	tcb * old_t = &idle_thread;
	//old thread wegpacken
	if (get_running() != IDLE_ID) {
		old_t = &all_threads[get_running()];

		switch (old_t->state) {
		case ready:
			save_to_queue(RUNQ, old_t->id);
			break;
		case finished:
			if ((--threads_per_proc[old_t->process_id]) <= 0) {
				save_to_queue(PROCESSQ, old_t->process_id);
			}
			save_to_queue(TERMQ, old_t->id);
			break;
		case sleeping:
			save_to_queue(SLEEPQ, old_t->id);
			break;
		case waitingc:
			old_t->sleep_time = WAITC_TIMEOUT;
			save_to_queue(WAITCQ, old_t->id);
			break;
		default:
			return;
		}
		stack_to_tcb(sp, old_t->id);
	}

	int new_id = get_from_queue(RUNQ);

	tcb * new_t = new_id == IDLE_ID ? &idle_thread : &all_threads[new_id];

	if (old_t->process_id != new_t->process_id) {
		change_context(new_t->process_id);
	}

	set_running(new_t->id);
	set_runstate(running);
	tcb_to_stack(sp, new_t->id);
}

void schedule_sleep(unsigned int * sp, int t)
{
	(&all_threads[get_running()])->sleep_time = t;
	change_thread(sp, sleeping);
	return;
}

void check_sleepq()
{
	int queue_len = get_queue_length(SLEEPQ);

	int current_id;
	for (int i = 0; i < queue_len; i++) {
		if ((current_id = get_from_queue(SLEEPQ)) == -1) {
			kprintf("No elements in sleepq\n");
			return;
		}
		int t = ((&all_threads[current_id])->sleep_time)--;
		if (t <= 0) {
			save_to_queue(RUNQ, current_id);
		} else {
			save_to_queue(SLEEPQ, current_id);
		}
	}
	return;
}


void check_waitcq()
{
	int temp;
	int queue_len = get_queue_length(WAITCQ);

	for (int i = 0; i < queue_len; i++) {
		tcb * new_tcb = &all_threads[get_from_queue(WAITCQ)];
		new_tcb->sleep_time--;
		if ((temp = get_from_buffer()) != -1) {
			change_context(new_tcb->process_id);
			*((char *) new_tcb->usr_regs[0]) = temp;
			if (get_running() != -1) {
				change_context((&all_threads[get_running()])->process_id);
			}
			new_tcb->usr_regs[0] = 1;
			save_to_queue(RUNQ, new_tcb->id);
		} else if (new_tcb->sleep_time <= 0) { //Timeout, waited too long
			new_tcb->usr_regs[0] = 0;
			save_to_queue(RUNQ, new_tcb->id);
		} else {
			save_to_queue(WAITCQ, new_tcb->id);
		}
	}
	return;
}

void schedule_timer (unsigned int * sp)
{
	check_waitcq();
	check_sleepq();
	change_thread(sp, ready);
}



