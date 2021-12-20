#include <kprint.h>
#include <bit_op.h>

#define RESET 			0b0000
#define ALIGNMENT 		0b0001
#define DEBUG 			0b0010
#define ACC_SEC 			0b0011
#define CACHE 			0b0100
#define TRANS_SEC 		0b0101
#define ACC_PG			0b0110
#define TRANS_PG			0b0111
#define PREC_EXT_ABT 		0b1000
#define DOM_SEC			0b1001
//no function				0b1010
#define DOM_PG			0b1011
#define TRANS_EXT_ABT1	0b1100
#define PERM_SEC			0b1101
#define TRANS_EXT_ABT2	0b1110
#define PERM_PG			0b1111
#define IMPREC_EXT_ABT	0b0110


#define IEA 10 //imprecise external abort Status[4]

int fsr_print (int fsr)
{
	kprintf("Fehler: ");

	if ((BITSET(fsr, IEA)) && bitslice(fsr, 3, 0) == IMPREC_EXT_ABT){
		kprintf("Imprecise External Abort\n\n");
		return 0;
	}

	switch (bitslice(fsr, 3, 0)){
		case (RESET):
			kprintf("No function, reset value\n\n");
			return 0;
		case (ALIGNMENT):
			kprintf("Alignment fault\n\n");
			return 0;
		case (DEBUG):
			kprintf("Debug event fault\n\n");
			return 0;
		case (ACC_SEC):
			kprintf("Access Flag fault on Section\n\n");
			return 0;
		case (CACHE):
			kprintf("Cache maintenance operation fault[b]\n\n");
			return 0;
		case (TRANS_SEC):
			kprintf("Translation fault on Section\n\n");
			return 0;
		case (ACC_PG):
			kprintf("Access Flag fault on Page\n\n");
			return 0;
		case (TRANS_PG):
			kprintf("Translation fault on Page\n\n");
			return 0;
		case (PREC_EXT_ABT):
			kprintf("Precise External Abort\n\n");
			return 0;
		case (DOM_SEC):
			kprintf("Domain fault on Section\n\n");
			return 0;
		case (DOM_PG):
			kprintf("Domain fault on Page\n\n");
			return 0;
		case (TRANS_EXT_ABT1):
			kprintf("External abort on translation, first level\n\n");
			return 0;
		case (PERM_SEC):
			kprintf("Permission fault on Section\n\n");
			return 0;
		case (TRANS_EXT_ABT2):
			kprintf("External abort on translation, second level\n\n");
			return 0;
		case (PERM_PG):
			kprintf("Permission fault on Page\n\n");
			return 0;
		default:
			kprintf("Unknown fault source due to unknown status code %c%c\n\n", bitslice(fsr, 10, 10), bitslice(fsr, 3, 0));
			return -1;
		}
}


