#include "utfconvert.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Text "Invalid input!" in the function.
void invalidInput() {
	printf("Invalid input!\n");
}

/**
 * Check every bit in significant byte if bits are in correct form.
 * @param[in] code -> code to be checked
 * @param[in] bytes -> how many bytes code has
 */
bool checkSignificantByte(unsigned int code, int bytes) {
	if (bytes == 1) {
		return !(code & 0x80);
	}
	
	for (int i = 0; i < bytes + 1; ++i) {
		if (i == bytes) {
			if (code & (0x80 >> i)) {
				return false;
			}
			return true;
		}
		if (code & (0x80 >> i)) {
			continue;
		}
		return false;
	}
	return true;
}

/**
* Check if given code is in correct form
* exp: 1 byte = 0xxxxxxx ...
* @param[in] code -> given hex code
* @param[in] bytes -> how many bytes given code has
*/

bool isValid(unsigned int code, int bytes) {
	if (bytes == -1) return false;
	unsigned int tmp = code;
	for (int i = 0; i < (bytes - 1); ++i) {

		// 0x80 = 1000 0000
		// 0x40 = 0100 0000
		// All continuation bytes are checked with these hex numbers
		if ((tmp & 0x80) && ((tmp & 0x40) == 0x00)) {
			tmp >>= 8;
			continue;
		}
		return false;
	}

	// Now significant byte is going to be checked
	return checkSignificantByte(tmp, bytes);
}

/**
* @param[in] code -> return amount of bytes from given code
*/
int bytesCount(unsigned int code) {
	if (code <= 0x7F) {
		return 1;
	} else if (code > 0x7F && code <= 0xFFFF) {
		return 2;
	} else if (code > 0xFFFF && code <= 0xFFFFFF) {
		return 3;
	} else if (code > 0xFFFFFF && code <= 0xFFFFFFFF) {
		return 4;
	}
	return -1;
}

/**
* @param[in] unicode -> given unicode to be checked.
* Intervals from assignment.
*/
bool checkUniInterval(unsigned int unicode) {
	return ((unicode <= 0xD7FF)
		|| (unicode >= 0xE000 && unicode <= 0x10FFFF));
}

/**
* Print result to correcnt form
* @param[in] unic -> Unicode
* @param[in] bytes -> Amount of bytes of given unicode
* Function starts reading from right
*/

void printResult(unsigned int unic, int bytes) {
	// Significant byte is first so I have to shift all
	// continuation bytes to the left
	unsigned int tmp = (unic >> 8 * (bytes-1));

	// All 1 bytes unicodes are easy to print.
	if (bytes == 1) {
		printf("%.6d", unic);
		return;
	}
	// 0xFF = 1111 1111 -> take mask
	unsigned int result = 0x00;
	for (int i = 1; i < bytes + 1; ++i) {
		result = (tmp & 0xFF);
		printf("%.3d", result);

		tmp = (unic >> 8 * (bytes - (i + 1)));
	}
}

int main(void){

	// First scanned number
	int hexCount = -1;

	/***** FIRST NUMBER *****/
	// validCount takes returned value from scanf and is checked
	unsigned int validCount = scanf("%d", &hexCount);
	if (validCount != 1 || hexCount < 0) {
		invalidInput();
		return 1;
	}
	// ******************************

	/***** HEXADECIMAL CODES *****/

	// Scanned hexadecimal code
	unsigned int hexCode = 0;

	// How many bytes scanned hex code has
	int bytes = 0;

	// Reading hexadecimal codes
	for (int i = 0; i < hexCount; ++i) {

		if (scanf("%X", &hexCode) != 1) {
			invalidInput();
			return 1;
		}

		bytes = bytesCount(hexCode);

		// Check form
		if (!isValid(hexCode, bytes)) {
			invalidInput();
			return 1;
		}

		unsigned int unicode = hexCode;

		// All hexadecimale codes which have only 1 byte are unicodes.
		// So there is no reason to cut bites.
		if (bytes != 1) {
			unicode = utf8ToUni(hexCode, bytes);
		}

		// Check interval
		if (!(checkUniInterval(unicode))) {
			invalidInput();
			return 1;
		}

		// First ten bites of unicode
		unsigned int firstTenBites = 0x00;
		// Last ten bites of unicode
		unsigned int secondTenBites = 0x00;

		// 0xFFFF -> biggest 2 bytes number
		if (unicode > 0xFFFF) {
			// If unicode is more than 2 bytes there is a reason to cut those 
			// already mentioned bites and save them to the variables : firstTenBites, secondTenBites.
			uniToUtf16(unicode, &firstTenBites, &secondTenBites);
			unsigned int finalResult = (firstTenBites << 8 * bytesCount(firstTenBites)) | secondTenBites;
			printResult(finalResult, bytesCount(finalResult));
			// 1.Bonus print
			printf(" 0x%X%X\n", firstTenBites, secondTenBites);

		} else {
			int unicodeBytes = bytesCount(unicode);
			printResult(unicode, unicodeBytes);
			// 1. Bonus print
			printf(" 0x%.4X\n", unicode);
		}
	}
	return 0;
}