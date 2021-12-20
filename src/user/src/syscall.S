#include <syscall_const.h>

.globl sleep
.globl write
.globl clone
.globl read
.globl exit
.globl fork

sleep:
	svc SLEEP_SVC
	mov pc, lr

write:
	svc WRITE_SVC
	mov pc, lr

clone:
	svc CLONE_SVC
	mov pc, lr

read:
	svc READ_SVC
	mov pc, lr

exit:
	svc EXIT_SVC
	mov pc, lr

fork:
	svc FORK_SVC
	mov pc, lr
