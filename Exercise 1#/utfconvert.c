#include "utfconvert.h"

/* 
There are four different option for significant byte:
1) code in binary is 1 byte long so the valid form starts with 0xxxxxxx
   and macro FIRST_SIGNIFICANT_BYTE_1 check its significant byte form.
2) Code in binary is 2 bytes long so the valid form starts with 110xxxxx 10xxxxxx
   and macro FIRST_SIGNIFICANT_BYTE_2 check its significant byte form.
3) 3 bytes long -> 1110xxxx 10xxxxxx 10xxxxxx form -> FIRST_SIGNIFICANT_BYTE_3 checks it.
4) 4 bytes long -> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx -> FIRST_SIGNIFICANT_BYTE_4 checks it.

0x80 -> 1000 0000(2)
0xCO -> 1100 0000(2)
0xE0 -> 1110 0000(2)
0xF0 -> 1111 0000(2)

With every given hex code I do bitwise operation &.
*/
#define FIRST_SIGNIFICANT_BYTE_1 0X80
#define FIRST_SIGNIFICANT_BYTE_2 0xC0
#define FIRST_SIGNIFICANT_BYTE_3 0xE0
#define FIRST_SIGNIFICANT_BYTE_4 0xF0


int firstByteChecker(int bytes) {
	switch (bytes){
		case 1:
			return FIRST_SIGNIFICANT_BYTE_1;
		case 2:
			return FIRST_SIGNIFICANT_BYTE_2;
		case 3:
			return FIRST_SIGNIFICANT_BYTE_3;
		default:
			return FIRST_SIGNIFICANT_BYTE_4;
	}
}

int myPower(int exp) {
	return 1 << exp;
}

unsigned int utf8ToUni(unsigned int code, int bytes) {
	// 0x3F = 0011 1111 -> used for cutting those bites.
	unsigned int pattern = 0x3F;
	// To cut significant byte
	unsigned int lastPattern = ~(firstByteChecker(bytes) | myPower(7 - bytes));
	unsigned int result = 0x0;
	unsigned int tmp = code;
	unsigned int tmp2;

	// 
	for (int i = 0; i < bytes; ++i){
		// (bytes - 1) represents significant byte
		// Evety other value is continuation byte
		// If cut is done we have to shift 6*i bites to the left
		if (i == bytes - 1) {
			tmp2 = (tmp & lastPattern);
			tmp2 <<= (6 * i);
			result |= tmp2;
			continue;
		}
		tmp2 = (tmp & pattern);
		if (i > 0) {
			tmp2 <<= (6 * i);
		}
		result |= tmp2;
		tmp >>= 8;
	}
	return result;
}

void uniToUtf16(unsigned int uni, unsigned int *firstT, unsigned int *secondT) {
	unsigned int uni2 = uni - 0x10000;

	*firstT = (uni2 >> 10) + 0xD800;
	*secondT = (uni2 & 0x3FF) + 0xDC00;
}
