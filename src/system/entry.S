#define svc_stack_offset 0x000000
#define fiq_stack_offset 0x100000
#define abt_stack_offset 0x200000
#define und_stack_offset 0x300000
#define irq_stack_offset 0x400000

#define usr_stack_offset 0x000000

.section .init

.global _start
_start:
	/* Erkennen ob Hypervisor Mode aktiv */
	mrs r0, cpsr
	and r0, r0, #0x1F
	mov r1, #0x1A	//Modebits = 0x1A = Hypervisor Mode
	cmp r0, r1	//Vergleichen und Statusbits setzen für bedingte Sprünge

	/* Wenn Hypemode erkannt: abschalten (sprung nach _exitHyper)
	 * wenn nicht erkannt: dann weitermachen (weiterlaufen nach _bsprak)
	 */
	beq _exitHyper

	/* Qemu startet immer alle 4 Kerne
	 * Wir lassen alle anderen Kerne endless loopen
	 */
_loopCores:
	/* Id des Cpu Cores Abfragen */
	mrc p15, 0, r0, c0, c0, 5
	/* Falls nicht core 0 -> Endless loop */
	tst r0, #3
	bne .Lend

_bsprak:
	/* Wir brauchen einen validen Stack für Qemu.
	 * Auf dem Pi wurde dieser zwar bereits durch U-Boot
	 * gesetzt, schadet aber auch nicht
	 */
	bl get_kernelstack_address

	/* Stackpointer aller Modi initialisieren */
	sub r1, r0, #svc_stack_offset
	mov sp, r1 /* initial 0x20000 */

/* 	mov r1, #abt_stack */
	sub r1, r0, #abt_stack_offset
	msr SP_abt, r1

/* 	mov r1, #und_stack */
	sub r1, r0, #und_stack_offset
	msr SP_und, r1

/* 	mov r1, #irq_stack */
	sub r1, r0, #irq_stack_offset
	msr SP_irq, r1

/* 	mov r1, #fiq_stack */
	sub r1, r0, #fiq_stack_offset
	msr SP_fiq, r1

	bl get_userstack_address
	sub r1, r0, #usr_stack_offset
/* 	mov r1, #usr_stack */
	msr SP_usr, r1

	/* Einstellen der IVT-Basisadresse*/
	ldr r0, =_ivt
	mcr p15, 0, r0, c12, c0, 0

	/* set I and F Flag to 0 */
	mrs r2, cpsr
	and r2, r2, #0xFFFFFF3F
	msr cpsr_cxsf, r2

/* 	b _setupMMU */

	starten:
	/* Zu c Code springen */
	bl start_kernel

.Lend:
	WFI
	b .Lend

_exitHyper:

	/* Rücksprungadresse ins Hypervisor Rücksprungregister schreiben.
	 * Dies ist das Label _bsprak für die normale Programmausfürung.
	 */
	ldr lr, =_loopCores
	msr ELR_hyp, lr

	/* Der Hypervisor mode ist auch "nur" eine etwas kompliziertere Exception.
	 * Also springen wir aus dieser zurück.
	 * (Rücksprunge in Aufgabe 2 sind anders zu lösen!).
	 * Dazu zunächst das CPSR auslesen und die Modebits zum Supervisor ändern.
	 * Danach dies in das SPSR für den Rücksprung schreiben.
	 */

	mrs r0, cpsr
	bic r0, r0, #0x1F
	orr r0, r0, #0x13	//Supervisormode
	msr spsr_hyp, r0
	eret

.globl _setupMMU
_setupMMU:
	/* Einstellungen für MMU */
	/*L1 tabelle platzieren (TTBR0)*/
/* 	ldr r0, =L1tab */
	breaker1:
	mrc p15, 0, r1, c2, c0, 0
	and r1, r1, #0x7F /*unteren 7 bit behalten*/
	orr r1, r1, r0
	mcr p15, 0, r1, c2, c0, 0

	/*Rechte auf client mode (DACR)*/
	mrc p15, 0, r1, c3, c0, 0
	and r1, r1, #0xFFFFFFFC
	orr r1, r1, #0b1
	mcr p15, 0, r1, c3, c0, 0

	/* short umsetzung aktivieren (TTBCR)*/
	mrc p15, 0, r1, c2, c0, 2
	and r1, r1, #0x7FFFFFFF /*oberstes Bit auf 0 setzten*/
	mcr p15, 0, r1, c2, c0, 2

	breaker2:
	/* none cacheable (SCTLR) + MMU on*/
	mrc p15, 0, r1, c1, c0, 0
	and r1, r1, #0xFFFFFFFB /*bit 2 auf 0 setzen*/
	orr r1, r1, #0b1 /* bit 0 auf 1 setzen*/
	mcr p15, 0, r1, c1, c0, 0

	mov pc, lr
