#include <exception_handlers.h>
#include <kprint.h>
#include <bit_op.h>
#include <fsr.h>

int _get_dfar();
int _get_dfsr();
int _get_ifar();
int _get_ifsr();

int print_psr(int psr);

int print_registers(unsigned int * sp)
{
	kprintf(">>> Registerschnappschuss (aktueller Modus) <<<\n");

	int saved_sp = (int) sp;
	int i;

	for (i = 0; i<5; i++) {
		kprintf("r%i: %010p	r%i: %010p\n", i,  *sp, i+8, *(sp+8));
		sp++;
	}

	kprintf("r%i: %010p	sp: %010p\n",i++, *sp, saved_sp);
	sp++;
	kprintf("r%i: %010p	lr: %010p\n",i++, *sp, *(sp+7));
	sp++;
	kprintf("r%i: %010p	pc: %010p\n",i, *sp, *(sp+7));

	return 0;
}

void print_mode_specific(unsigned int * sp, char mode, int lr, int spsr)
{
	int saved_sp = (int) sp;

	kprintf("\n>>> Aktuelle modusspezifische Register <<<\n");

	const char* mode_strings[] = {"Supervisor:", "Abort:\t", "FIQ:\t", "IRQ:\t", "Undefined:", "", ""};
	const char modes_char[] = "safiu";

	kprintf("\t\tLR		SP		SPSR\n");
	kprintf("User/System:	%010p	%010p\n",  sp[0], sp[1]);
	sp+=3;
	for (int i = 0; i < 5; i++) {
		if (mode != modes_char[i]) {
			kprintf("%s	%010p	%010p	", mode_strings[i], sp[0], sp[1]);
			print_psr(sp[2]);
			sp+=4;
		} else {
			kprintf("%s	%010p	%010p	", mode_strings[i], lr, saved_sp);
			print_psr(spsr);
		}
	}
}

int print_psr(int psr)
{
	//Compare Flags
	char * psr_str = "NZCV E IFT";
	int psr_regno []= {31, 30, 29, 28, -1, 9, -1, 7, 6, 5, -1};

	for(int i = 0; i < 11; i++){
		if(psr_regno[i]<0){
			kprintf(" ");
			continue;
		}

		if(BITSET(psr, psr_regno[i])){
			kprintf("%c", psr_str[i]);
		} else {
			kprintf("_");
		}
	}

	//Modus
	switch(bitslice(psr, 4, 0)){
		case(USR_MOD):
			kprintf("User\t");
			break;
		case(FIQ_MOD):
			kprintf("FIQ\t");
			break;
		case(IRQ_MOD):
			kprintf("IRQ\t");
			break;
		case(SVC_MOD):
			kprintf("Supervisor");
			break;
		case(ABT_MOD):
			kprintf("Abort\t");
			break;
		case(UND_MOD):
			kprintf("Undefined");
			break;
		case(SYS_MOD):
			kprintf("System\t");
			break;
		default:
			kprintf("No such mode. Unknown behavior\n");
			return -1;
	}

	kprintf("\t(%010p)\n", psr);
	return 0;
}


int handler_output(int ex_type, unsigned int * sp, int lr, int cpsr, int spsr, int prev_mode)
{
	char mode;
	char abort_type = 0;
	kprintf("\n###########################################################################\n");
	switch (ex_type){
		case data_abt:
			kprintf("Data Abort an Adresse %010p\n", lr);
			mode = 'a';
			abort_type = 'd';
			break;

		case und:
			kprintf("Undefined Instruction an Adresse %010p\n", lr);
			mode = 'u';
			break;

		case swi:
			kprintf("Software Interrupt an Adresse %010p\n", lr);
			mode = 's';
			break;

		case pref_abt:
			kprintf("Prefetch Abort an Adresse %010p\n", lr);
			mode = 'a';
			abort_type = 'p';
			break;

		case inter:
			kprintf("Hardware Interrupt nach Abschluss der Instruktion an Adresse %010p\n", lr - INTERRUPT_OFF);
			mode = 'i';
			break;

		default: return -1;
	}

	if (abort_type == 'd'){
		int dfar = _get_dfar();
		int dfsr = _get_dfsr();

		kprintf("Zugriff: %s auf Adresse %010p\n", ((BITSET(dfsr, RW_DFSR)) == 1 ? "schreibend" : "lesend"), dfar);

		fsr_print(dfsr);
	} else if (abort_type == 'p'){
		int ifar = _get_ifar();
		int ifsr = _get_ifsr();

		kprintf("Zugriff: lesend auf Adresse %010p\n", ifar);

		fsr_print(ifsr);
	}

	kprintf("\n");
	print_registers(sp+MODE_SPECIFIC_SIZE+USR_SPECIFIC_SIZE); //modusspezifische Register überspringen

	kprintf("\n");

	kprintf(">>> Aktuelle Statusregister (SPSR des aktuellen Modus) <<<\n");
	kprintf("CPSR: ");
	print_psr(cpsr);

	kprintf("SPSR: ");
	print_psr(spsr);

	print_mode_specific(sp, mode, lr, spsr);

	if ((mode != 'i')) {
		switch (prev_mode) {
			case USR_MOD:
				kprintf("\nThread angehalten.\n\n");
				break;
			default:
				kprintf("\nSystem angehalten.\n\n");
				break;
		}
	}
	return 0;
}
