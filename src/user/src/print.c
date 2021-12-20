#include <stdarg.h>
#include <limits.h>
#include <syscall.h>

#define ASCII_NUM_OFFSET 48
#define HEX "0123456789abcdef"


void printf (char* str, ...);

void string_send_u (unsigned char * str)
{
	while(*str) {
		write(str[0]);
		str++;
	}
	return;
}

void  arr_reverse_u(unsigned char* origin, unsigned char* dest, int length)
{
	for(int i = 0; i < length; i++){
		dest[i] = origin[length-1-i];
	}
}

void pad_and_flush_u(int padw, char padc, unsigned char* str)
{
	while(padw > 0){
		write(padc);
		padw--;
	}
	string_send_u(str);
}

void fill_buffer_u(unsigned int x, int width, unsigned int base, int feld, char zeropad, char c)
{
	int j = 0;
	unsigned char buf [width+2];
	while (x > base-1) {
		buf[j] = HEX[x % base];
		x = x/base;
		j++;
	}
	buf[j] = HEX[x];

	//Pointerschreibweise
	if (c == 'p'){
		if (zeropad == '0'){
			write('0');
			write('x');
			feld -= 2;
		} else {
			buf[j+1] = 'x';
			buf[j+2] = '0';
			j += 2;
		}
	}

	unsigned char xp [j+2];
	arr_reverse_u(buf, xp, j+1);
	xp[j+1] = '\0';

	pad_and_flush_u(feld-j-1, zeropad, xp);
}

int format_u(char * point, va_list args, int * z)
{
	unsigned char l[2];
	unsigned char * string;
	unsigned int x;
	int i;
	int j = 0;
	int char_incs = 0;
	char zeropad = ' ';
	unsigned int feld = 0;

	//Calculate Padding
	if(*point >= '0' && *point <= '9'){
		if (point[0] - ASCII_NUM_OFFSET == 0){
			zeropad = '0';
		}
		while (point[0] >= ASCII_NUM_OFFSET && point[0] < ASCII_NUM_OFFSET+10) { //Feldbreite
			feld *= 10;
			feld += ((int) point[0] -ASCII_NUM_OFFSET);
			point++;
			char_incs++;
		}
	}

	switch(*point){
		//Char
		case 'c':
			l[0] = (unsigned char) va_arg(args, int);
			l[1] = '\0';
			j++;
			pad_and_flush_u(0, ' ', l);
			*z = 1;
			break;

		//String
		case 's':
			string = (unsigned char*) va_arg(args, char *);

			while(*string){
				string++;
				j++;
			}

			string -= j;
			pad_and_flush_u(feld-j, ' ', string);
			*z = 1;
			break;

		//Pointer
		case 'p':
			x = va_arg(args, unsigned int);

			fill_buffer_u(x, 8, 16, feld, zeropad, 'p');
			*z = 1;
			break;

		//Hexadezimalzahl
		case 'x':
			x = va_arg(args, unsigned int);

			fill_buffer_u(x, 8, 16, feld, zeropad, 0);
			*z = 1;
			break;

		//Integer
		case 'i':
			i = va_arg(args, int);
			if (i<0) {
				x = -i;
				write('-');
				feld--;
			} else {
				x = (unsigned int) i;
			}

			fill_buffer_u(x, 10, 10, feld, zeropad, 0);
			*z = 1;
			break;

		//Unsigned Integer
		case 'u':
			x = va_arg(args, unsigned int);

			fill_buffer_u(x, 10, 10, feld, zeropad, 0);
			*z = 1;
			break;

		//Prozentzeichen
		case '%':
			write('%');
			break;

		//Not a format specifier
		default:
			printf("\nWarning: Unknown format specifier: %%%c\n", *point);
			break;
	}

	char_incs++;
	return char_incs;
}

void printf (char* str, ...)
{
	va_list args;
	int j = 0;		//number of used arguments
	int z = 0;		//argument used because of valid format specifier
	char c;

	while (*str) {
		c = str[0];
		if (c == '%') {
			va_start(args, str);

			//Discard previous arguments
			for (int i = 0; i < j; i++){
				va_arg(args, unsigned int);
			}

			str += format_u(&str[1],args, &z) + 1;
			va_end(args);

			j += z;
			z = 0;
			continue;
		}
		write(str[0]);
		str++;
	}
}

