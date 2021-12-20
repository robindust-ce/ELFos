#ifndef BIT_OP_H
#define BIT_OP_H

#define BITSET(val, bit) (((val) & (1 << bit)) == (1 << bit)) ? 1 : 0

int bitslice (int val, int high, int low);
int set_bits (int buc, int bits, int shamt);

#endif
