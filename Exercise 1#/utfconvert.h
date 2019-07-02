#ifndef UTFCONVERT_H
#define UTFCONVERT_H

/**
 * Transforms a hexadecimal number representing
 * a UTF-8 encoded character into its Unicode equivalent
 *
 * @param[in] utf8code  UTF-8 code
 * @param[in] bytes how many bytes utf8code has
 * @return Unicode value of the UTF-8 encoded character
 */
unsigned int utf8ToUni(unsigned int utf8code, int bytes);


void showbits(unsigned int x);
/**
 * Encodes a unicode value using UTF-16 coding
 *
 * @param[in] unicode  Unicode value to encode
 * @param[in out] firstT upper first ten bites
 * @param[in out] secondT lower ten bites
 * @return two hexadecimal numbers which will be joined.
 */
void uniToUtf16(unsigned int uni, unsigned int *firstT, unsigned int *secondT);

/**
 * Standard librabry has power function but it works with double I only need int
 * Was not sure baout type collision so I made my own power function for only int numbers.
 * @param[in] exp -> 2^exp
 */
int myPower(int exp);

/** 
 * When all  continuation bytes are valid the significant byte is going to be checked.
 * @param[in] bytes -> represents how many bytes given code has.
 * Based on this 'bytes' parameter the corresponding macro is returned.
 */

int firstByteChecker(int bytes);

#endif
