#include <tcb_queue.h>
#include <kprint.h>

volatile tcb_q queues[5];
int running;

void tcbq_init() {

	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < 32; i++) {
			queues[j].buffer[i] = -1;
		}
		queues[j].write_head = 0;
		queues[j].read_head = 0;
		queues[j].length = 0;
	}
}

int get_running()
{
	return running;
}

void set_running(int id)
{
	running = id;
}

int save_to_queue(int q, int id)
{
	if (q >= Q_CNT || q < 0) {
		return -2;
	}

	if(queues[q].length >= THREAD_CNT){
		return -1; //full
	}

	queues[q].buffer[queues[q].write_head] = id;
	queues[q].write_head++;
	queues[q].length++;
	queues[q].write_head %= THREAD_CNT;

	return 0;
}

int get_from_queue(int q)
{
	if (q >= Q_CNT || q < 0) {
		return -2;
	}

	if (queues[q].length <= 0){
		return -1;
	} else {
		int out = queues[q].buffer[queues[q].read_head];
		queues[q].read_head++;
		queues[q].read_head %= THREAD_CNT;
		queues[q].length--;
		return out;
	}
}

int get_queue_length (int q)
{
	if (q >= Q_CNT || q < 0) {
		return -2;
	}

	return queues[q].length;
}
