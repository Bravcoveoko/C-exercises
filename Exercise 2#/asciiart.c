#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asciifont.h"

#define UNUSED(var) ((void) (var))
// 0x80 = 1000 0000
// Used for get every bit from given character
#define BYTE_FOR_MASK 0x80
#define HASH_TAG '#'
#define WHITE_SPACE ' '

/**
* Return 1 or 0 based on mod
* @param[in] mod -> modulo after division
**/
int rest(int mod)
{
    return (mod > 0) ? 1 : 0;
}

/**
* Printing and doing some bit shifts to the left
* @param[in] hexCode -> given hexadecimal code from FONT_DATA
**/
void printBytes(unsigned int hexCode)
{
	for (int bit = 0; bit < 8; ++bit) {
		if (hexCode & (BYTE_FOR_MASK >> bit)) {
			putchar(HASH_TAG);
		} else {
			putchar(WHITE_SPACE);
		}
	}
}


void printCharacters(int length, char *text, long inputLength)
{
	// maxInLine represents how many characters can be put in one line (only div)
	div_t maxInLine = div(inputLength, 8);
	div_t division = div(length, maxInLine.quot);
	// How many lines we have to print out based on given text
	int numberOfLines = division.quot + rest(division.rem);

	int start = 0;
	int end = 0;

	// This for loop represents how many lines I will have in terminal
	for (int line = 0; line < numberOfLines; ++line) {

		// Previous value of 'end' variable
		int tmpEnd = end;

		// How many characters has to be printed. It is some kind of interval. The ending limit.
		end = ((end + maxInLine.quot) <= length) ? (end + maxInLine.quot) : length;
		if (end == length) {
			start = tmpEnd;
		} else {
			start = end - maxInLine.quot;
		}

		// Previous value of variable 'start'
		int tmpStart = start;

		// This for loop represents lines in every character 'cause every character is 8x8 bits form 
		for (int lineInChar = 0; lineInChar < 8; ++lineInChar) {

			// In this for loop we get the hexadecimal value.
			for (;start < end; ++start) {
				printBytes(FONT_DATA[(int)text[start]][lineInChar]);
			}
			start = tmpStart;
			printf("\n");
		/**** If you dont know what am I doing with all these for loops ****/
		/* First I have to find out how many characters I can put in one line
		Then I print first hexadecimal code every character in line then print
		second hexadecimal code of every character in line and so on
		*/
		}
	}
}


int main(int argc, char *argv[])
{
	// Incorrect amount of parameters.
	if (argc != 2) {
		fprintf(stderr, "CHYBA: Zly pocet vstupnych parametrov.\n");
		return 1;
	}

	char *endPointer;
	long inputLength;

	inputLength = strtol(argv[1], &endPointer, 10);

	// *endPointer keeps decimal value of first non-decimal character. If there is not non-decimal character
	// it keeps value of 0.
	if (*endPointer != 0 || inputLength < 0) {
		fprintf(stderr, "CHYBA: Zadany parameter nie je cislo.\n");
		return 1;
	}

	// Has to be greater or equal 8.
	if (inputLength < 8) {
		fprintf(stderr, "CHYBA: Zadany parameter musi byt vacsi ako 8.\n");
		return 1;
	}

	// Variable to keeps read characters
	signed long int c;
	// 101 for the ending 0
	char text[101] = {'\0'};
	// length of read text
	int count = 0;

	// Reading characters until EOF or \n or non-ASCII
	while((c = getchar()) != EOF && c != '\n' && c < 128 && c >= 0) {
		text[count] = c;
		++count;
	}

	// Check if stoppage is occasioned by high value or less 
	if (c != EOF && c != '\n') {
		fprintf(stderr, "CHYBA: Vstup obsahuje neplatny ASCII znak.\n");
		return 1;
	}

	// Final printing
	printCharacters(count, text, inputLength);	
    return 0;
}
