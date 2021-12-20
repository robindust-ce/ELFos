#include <kprint.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define ELF "ELF"
#define OS "OS"

// #define UNUSED(x) (void) x
// int a = 1;
// int b;

/* Disclaimer:
 * Diese Datei ist nur zum Spaß gedacht. Wir dachten vlt heitert es während dem Korrigieren auf. Zumindest hat es uns beim Bearbeiten aufgeheitert. Bitte nicht bewerten :D
 */

void print_n_times (int n, char * str)
{
	for (int i = 0; i < n; i++) {
		kprintf("%s", str);
	}
}

void print_E () {
	for (int i = 0; i<4; i++) {
		print_n_times(10, ELF);
		kprintf("\n");
	}

	for (int i = 0; i<6; i++) {
		print_n_times(4, ELF);
		kprintf("\n");
	}

	for (int i = 0; i<4; i++) {
		print_n_times(10, ELF);
		kprintf("\n");
	}

	for (int i = 0; i<6; i++) {
		print_n_times(4, ELF);
		kprintf("\n");
	}

	for (int i = 0; i<4; i++) {
		print_n_times(10, ELF);
		kprintf("\n");
	}
}

void print_L() {
	for (int i = 0; i < 20; i++) {
		print_n_times(4, "\t");
		for (int i = 0; i < 4; i++) {
				kprintf("ELF");
		}
		kprintf("\n");
	}
	for (int i = 0; i < 4; i++) {
		print_n_times(4, "\t");
		for (int i = 0; i < 10; i++) {
			kprintf("ELF");
		}
		kprintf("\n");
	}
}

void print_F() {
	for (int i = 0; i<4; i++) {
		print_n_times(8, "\t");
		print_n_times(10, ELF);
		kprintf("\n");
	}

	for (int i = 0; i<6; i++) {
		print_n_times(8, "\t");
		print_n_times(4, ELF);
		kprintf("\n");
	}

	for (int i = 0; i<4; i++) {
		print_n_times(8, "\t");
		print_n_times(10, ELF);
		kprintf("\n");
	}

	for (int i = 0; i<10; i++) {
		print_n_times(8, "\t");
		print_n_times(4, ELF);
		kprintf("\n");
	}
	kprintf("\n");

}

void print_O()
{
	kprintf(RED);
	print_n_times(12, "\t");

	kprintf(GRN);
	kprintf("  ");
	print_n_times(7, OS);
	kprintf("  ");
	kprintf("\n");

	print_n_times(12, "\t");
	kprintf(CYN);
	print_n_times(9, OS);
	kprintf("\n");

	for (int i = 0; i < 3; i++) {
	print_n_times(12, "\t");
		kprintf(BLU);
		print_n_times(3, OS);
		kprintf("\t    ");
		print_n_times(3, OS);
		kprintf("\n");
	}

	for (int i = 0; i < 2; i++) {
		print_n_times(12, "\t");
		kprintf(MAG);
		print_n_times(3, OS);
		kprintf("\t    ");
		print_n_times(3, OS);
		kprintf("\n");
	}

	print_n_times(12, "\t");
	kprintf(RED);
	print_n_times(9, OS);
	kprintf("\n");

	kprintf(YEL);
	print_n_times(12, "\t");
	kprintf("  ");
	print_n_times(7, OS);
	kprintf("  ");
	kprintf("\n");
	kprintf(RESET);
}

void print_S() {
	kprintf(GRN);
	print_n_times(15, "\t");
	kprintf("  ");
	print_n_times(8, OS);
	kprintf("\n");

	kprintf(CYN);
	print_n_times(15, "\t");
	print_n_times(9, OS);
	kprintf("\n");

	kprintf(BLU);
	print_n_times(15, "\t");
	print_n_times(3, OS);
	kprintf("\n");

	print_n_times(15, "\t");
	print_n_times(8, OS);
	kprintf("\n");

	print_n_times(15, "\t");
	kprintf("  ");
	print_n_times(8, OS);
	kprintf("\n");

	kprintf(MAG);
	print_n_times(15, "\t");
	kprintf("\t    ");
	print_n_times(3, OS);
	kprintf("\n");

	print_n_times(15, "\t");
	kprintf("\t    ");
	print_n_times(3, OS);
	kprintf("\n");

	kprintf(RED);
	print_n_times(15, "\t");
	print_n_times(9, OS);
	kprintf("\n");

	kprintf(YEL);
	print_n_times(15, "\t");
	print_n_times(8, OS);
	kprintf("\n");
	kprintf(RESET);

}

void elf()
{
	print_n_times(138, "=");
	kprintf("\n");
	print_E();
	print_n_times(24, "\033[A");
	print_L();
	print_n_times(24, "\033[A");
	print_F();
	print_n_times(10, "\033[A");
	print_O();
	print_n_times(9, "\033[A");
	print_S();
	print_n_times(138, "=");
	kprintf("\n");

	print_n_times(14, "\t");
	print_n_times(4, " ");
	kprintf("Code goes wW!wW!wW!wW!\n\n");
}

