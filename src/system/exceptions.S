/* cpsr mode constants */
#define und 0b11011
#define iqr 0b10010
#define fir 0b10001
#define abt 0b10111
#define sup 0b10011
#define sys 0b11111

.globl _abort_the_data
.globl _stop_system
.globl _get_dfar
.globl _get_dfsr
.globl _get_ifar
.globl _get_ifsr
.globl _invalidate_tlb

.globl _ivt
.balign 64

_ivt:
	b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used: .word not_used
_irq: .word irq
_fiq: .word fiq


reset:
	/* nichts fortsetzen */
	b _start

undefined_instruction:
	sub	lr, lr, #4
	stmfd	sp!, {r0-r12, lr, pc}

	/* Thread information */
	mrs	r4, sp_usr
	mrs	r5, lr_usr
	mrs	r6, spsr
	stmfd	sp!, {r4-r6}

	mov	r1, lr
	mrs	r2, cpsr
	mrs	r3, spsr

	bl	_get_mode_regs
	mov	r0, sp

	mov	lr, pc
	ldr	pc, =undef_handler

	add	sp, sp, #72 /* ignore the 18 mode specific register */

	ldmfd	sp!, {r4-r6}
	msr	sp_usr, r4
	msr	lr_usr, r5
	msr	spsr_cxsf, r6

	ldmfd	sp!, {r0-r12, lr}
	add	sp, sp, #4 /* ignore pc */

	movs	pc, lr

software_interrupt:
	sub	lr, lr, #0
	stmfd	sp!, {r0-r12, lr, pc}
	mrs	r4, sp_usr
	mrs	r5, lr_usr
	mrs	r6, spsr
	stmfd	sp!, {r4-r6}

	/* Thread information */
	mov	r1, lr
	mrs	r2, cpsr
	mrs	r3, spsr

	bl	_get_mode_regs
	mov	r0, sp

	mov	lr, pc
	ldr	pc, =swi_handler

	add	sp, sp, #72 /* ignore the 18 mode specific register */

	ldmfd	sp!, {r4-r6}
	msr	sp_usr, r4
	msr	lr_usr, r5
	msr	spsr_cxsf, r6

	ldmfd	sp!, {r0-r12, lr}
	add	sp, sp, #4 /* ignore pc */

	movs	pc, lr


prefetch_abort:
	sub	lr, lr, #4
	stmfd	sp!, {r0-r12, lr, pc}
	mrs	r4, sp_usr
	mrs	r5, lr_usr
	mrs	r6, spsr
	stmfd	sp!, {r4-r6}

	/* Thread information */
	mov	r1, lr
	mrs	r2, cpsr
	mrs	r3, spsr

	bl	_get_mode_regs
	mov	r0, sp

	mov	lr, pc
	ldr	pc, =prefetch_handler

	add	sp, sp, #72 /* ignore the 18 mode specific register */
	ldmfd	sp!, {r4-r6}
	msr	sp_usr, r4
	msr	lr_usr, r5
	msr	spsr_cxsf, r6

	ldmfd	sp!, {r0-r12, lr}
	add	sp, sp, #4 /* ignore pc */

	movs	pc, lr

data_abort:
	sub	lr, lr, #8
	stmfd	sp!, {r0-r12, lr, pc}
	mrs	r4, sp_usr
	mrs	r5, lr_usr
	mrs	r6, spsr
	stmfd	sp!, {r4-r6}

	/* Thread information */
	mov	r1, lr
	mrs	r2, cpsr
	mrs	r3, spsr

	bl	_get_mode_regs
	mov	r0, sp

	mov	lr, pc
	ldr	pc, =data_handler

	add	sp, sp, #72 /* ignore the 18 mode specific register */
	ldmfd	sp!, {r4-r6}
	msr	sp_usr, r4
	msr	lr_usr, r5
	msr	spsr_cxsf, r6

	ldmfd	sp!, {r0-r12, lr}
	add	sp, sp, #4 /* ignore pc */

	movs	pc, lr

not_used:
	movs	pc, lr

irq:
	sub	lr, lr, #4

	stmfd	sp!, {r0-r12, lr, pc}

	/* Thread information */
	mrs	r4, sp_usr
	mrs	r5, lr_usr
	mrs	r6, spsr
	stmfd	sp!, {r4-r6}

	mov	r1, lr
	mrs	r2, cpsr
	mrs	r3, spsr

	bl	_get_mode_regs
	mov	r0, sp

	mov	lr, pc
	ldr	pc, =irq_handler

	add	sp, sp, #72 /* ignore the 18 mode specific register */
	ldmfd	sp!, {r4-r6}
	msr	sp_usr, r4
	msr	lr_usr, r5
	msr	spsr_cxsf, r6

	ldmfd	sp!, {r0-r12, lr}
	add	sp, sp, #4 /* ignore pc */
	breakdat:
	movs	pc, lr

fiq:
	stmfd	sp!, {r0-r12, lr, pc}
	sub	sp, sp, #4 /* place holder for alignment*/

	mov	r1, lr
	mrs	r2, cpsr
	mrs	r3, spsr

	bl	_get_mode_regs
	mov	r0, sp

	mov	lr, pc
	ldr	pc, =fiq_handler

	add	sp, sp, #72 /* ignore the 18 mode specific register */
	add	sp, sp, #4 /* ignore place holder */
	ldmfd	sp!, {r0-r12, lr}
	add	sp, sp, #4 /* ignore pc */

	subs	pc, lr, #4


_get_dfar:
	mrc	p15, 0, r0, c6, c0, 0
	mov	pc, lr

_get_dfsr:
	mrc	p15, 0, r0, c5, c0, 0
	mov	pc, lr

_get_ifar:
	mrc p15, 0, r0, c6, c0, 2
	mov pc, lr

_get_ifsr:
	mrc p15, 0, r0, c5, c0, 1
	mov pc, lr

/*save mode specific registers to stack*/
_get_mode_regs:
	and	r4, r2, #0x1F /* take lower 5 bits of cpsr (mode) */

undefined:
	cmp	r4, #und
	beq	IRQ

	mrs	r5, lr_und
	mrs	r6, sp_und
	mrs	r7, spsr_und

	stmfd	sp!, {r4,r5-r7}

IRQ:

	cmp	r4, #iqr
	beq	FIQ

	mrs	r5, lr_irq
	mrs	r6, sp_irq
	mrs	r7, spsr_irq

	stmfd	sp!, {r4, r5-r7}

FIQ:
	cmp	r4, #fir
	beq	abort

	mrs	r5, lr_fiq
	mrs	r6, sp_fiq
	mrs	r7, spsr_fiq

	stmfd	sp!, {r4, r5-r7}

abort:
	cmp	r4, #abt
	beq	supervisor

	mrs	r5, lr_abt
	mrs	r6, sp_abt
	mrs	r7, spsr_abt

	stmfd	sp!, {r4,r5-r7}

supervisor:
	cmp	r4, #sup
	beq	system

	mrs	r5, lr_svc
	mrs	r6, sp_svc
	mrs	r7, spsr_svc

	stmfd	sp!, {r4, r5-r7}

system:
	cmp	r4, #sys
	beq	end

	mrs	r5, lr_usr
	mrs	r6, sp_usr
	stmfd	sp!, {r5,r6}

end:
	mov	pc, lr

_abort_the_data:
	ldr	sp, =0x1
	stmfd	sp!, {r0}
	mov pc, lr /*sollte nie erreicht werden*/

_invalidate_tlb:
	mcr p15, 0, r0, c8, c5, 0
	mcr p15, 0, r0, c8, c6, 2
	mov pc, lr

_stop_system:
	b	_stop_system
