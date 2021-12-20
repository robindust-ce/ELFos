#include <syscall.h>
#include <print.h>

char PRINT_CHAR;
volatile unsigned int GLOB_COUNTER = 100;

void counter_loop(void * tid)
{
	unsigned int loc_counter = 0;
	char id = *((char *) tid);
	while(GLOB_COUNTER < 200){

		printf("%c:%u (%c:%u)\n", PRINT_CHAR, GLOB_COUNTER, id, loc_counter);

		loc_counter++;
		GLOB_COUNTER++;
		sleep(1);
	}
	exit();
	return;
}

void create_threads()
{
	char one = '1';
	char two = '2';
	char three = '3';

	if (clone(counter_loop, &two, sizeof(char)) != 0) printf("Fehler beim clonen\n");

	if (clone(counter_loop, &three, sizeof(char)) != 0) printf("Fehler beim clonen\n");

	counter_loop(&one);
}

int main()
{
	int chars_read;
	while(1) {
		while ((chars_read = read(&PRINT_CHAR)) == 0 ){
		}
		if(PRINT_CHAR == '\n') continue;
		if (fork(create_threads, &PRINT_CHAR, sizeof(char)) != 0) printf("Couldn't fork\n");
	}
}


