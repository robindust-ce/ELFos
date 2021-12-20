#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <syscall_const.h>

/* EXIT()
 * Description: The exit() function causes normal thread termination. It uses a SVC call using the immediate EXIT_SVC (refer to syscall_const.h)
 * Parameters: The exit() function has no parameters
 * Return value: The exit() function has no return value
*/

void exit ();


/* Description: The sleep() function pauses the calling thread for at least "t" full timer interrupts. The waiting time therefore depends on the configurations of the clock. It uses a SVC call using the immediate SLEEP_SVC (refer to syscall_const.h)
 * Parameters: The parameter "t" specifies the number of full timer interrupts the thread will sleep. If the SVC is called directly "t" must be placed in Register r0.
 * Return value: The sleep() function has no return value
 */

void sleep(unsigned int t);


/* WRITE()
 * Description: The write() function prints one character using uart. It uses a SVC call using the immediate WRITE_SVC (refer to syscall_const.h)
 * Parameters: The parameter "c" will be printed via uart. If the SVC is called directly "t" must be placed in Register r0.
 * Return value: The write() function has no return value
 */

void write(char c);


/* CLONE()
 * Description:	The clone() function creates a new thread and hands it to the scheduler. It uses a SVC call using the immediate CLONE_SVC (refer to syscall_const.h)
 * Parameters: 	The parameter "func" is a function pointer pointing to the starting point of the thread. If the SVC is called directly "func" must be placed in Register r0.
 *		The parameter "args" is a pointer to the memory in which the arguments for "func" are placed. If the SVC is called directly "args" must be placed in Register r1.
 *		The parameter "args_size" should be initialized to the size of the arguments in "args". If the SVC is called directly "args_size" must be placed in Register r2.
 *		Disclaimer: At this point the clone function only supports functions of type void (*func) (char *), therefore args must contain a char and args_size must be equal to one. The OS does only support a maximum of 4 Threads per address space.
 * Return value:	On success the clone() function returns 0, otherwise -1 is returned (e.g. when memory adresses outside of the user stack are handed to clone())
 */

int clone(void (*func)(void *), const void * args, unsigned int args_size);


/* FORK()
 * Description: just like clone but creates process that runs in a different address space than calling process. Copies data of calling process
 * Parameters: 	The parameter "func" is a function pointer pointing to the starting point of the thread. If the SVC is called directly "func" must be placed in Register r0.
 *		The parameter "args" is a pointer to the memory in which the arguments for "func" are placed. If the SVC is called directly "args" must be placed in Register r1.
 *		The parameter "args_size" should be initialized to the size of the arguments in "args". If the SVC is called directly "args_size" must be placed in Register r2.
 *		Disclaimer: At this point the fork function only supports functions of type void (*func) (char *), therefore args must contain a char and args_size must be equal to one. The OS also only supports a maximum of 8 different processes/address spaces
 * Return value:	On success the fork() function returns 0, otherwise -1 is returned (e.g. when memory adresses outside of the user stack are handed to fork())
 */

int fork(void (*func)(void *), const void * args, unsigned int args_size);


/* READ()
 * Description:	The read() function reads one character from uart and stores it at the provided memory address. It uses a SVC call using the immediate READ_SVC (refer to syscall_const.h). If there is currently no character to read, the function will wait for a char to arrive. If there is no character available for a certain amount of time, the function will return 0 to prevent deadlocks.
* Parameters:	The parameter "buf" is a pointer to where the character that was read should be stored. If the SVC is called directly "buf" must be placed in Register r1.
* Return value:	On success, the read() function returns the number of read chars. If read() returns 0 there were no chars available within the timeout period. If read() fails it returns -1 (e.g. if "buf" is outside of user memory bounds)
*/

int read(char * buf);


#endif//_SYSCALL_H_
