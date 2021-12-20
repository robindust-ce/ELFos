#include <bit_op.h>

int bitslice (int val, int high, int low)
{
	int slicer = 0;
	for (int i = high; i >= low; i--){
		slicer |= (1 << i);
	}
	return (val & slicer);
}

int set_bits (int buc, int bits, int shamt){
	return buc |= (bits << shamt);
}
