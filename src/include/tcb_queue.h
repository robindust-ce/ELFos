#ifndef TCB_QUEUE_H
#define TCB_QUEUE_H

#define THREAD_CNT 32
#define Q_CNT 5

#define TERMQ 0
#define RUNQ 1
#define SLEEPQ 2
#define WAITCQ 3
#define PROCESSQ 4

#define IDLE_ID -1

typedef struct tcb_q {
	volatile int buffer[THREAD_CNT];
	volatile int write_head;
	volatile int read_head;
	volatile int length;
}tcb_q;

int get_from_queue(int q);
int save_to_queue(int q, int id);
int get_running();
void set_running(int id);
int get_queue_length(int q);
tcb_q * get_queue(int q);
void tcbq_init();

#endif

