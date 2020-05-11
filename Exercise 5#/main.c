#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "myHeader.h"




int main(int argc, char *argv[]){

	// Premenna na kontrolu poctu vstupnych argumentov
	// 1 => nazov programu
	int countCheck = 1;

	char *inputFile = NULL;
	char *outputFile = NULL;

	int c;
	bool input = false;

	// Premenna aby som vedel ktory prepinac bol naposledy
	int lastOperation;
	while ((c = getopt (argc, argv, "i:o:ult")) != -1){
		lastOperation = c;
		switch(c) {
			case 'i':
				// i prepinac bere jeden parameter + sameho seba
				countCheck += 2;
				input = true;
				inputFile = optarg;
				break;
			case 'o':
				countCheck += 2;
				outputFile = optarg;
				break;
			case 'u':
				countCheck += 2;
				break;
			case 'l':
				// l prepinac bere 3 parametre + sameho seba
				countCheck += 4;
				break;
			case 't':
				countCheck += 3;
				break;
				return 0;
		}
	}

	// Kontrola ci sedi pocet argumentov to ze ci su spravne logicky sa kontroluje inokedy. 
	if (countCheck != argc || !input) {
		return 0;
	}

	// Pokus otvorit vstupny subor
	FILE *inputF = fopen(inputFile, "r");

	if (!inputF) {
		return 0;
	}

	// Pokus otvorit vystupny subor
	FILE *outpuF = NULL;
	if (outputFile != NULL) {
		outpuF = fopen(outputFile, "w");

		if (!outpuF) {
			return 0;
		}
	}

	int amount = 0;
	Data *structArr = readData(inputF, &amount);

	fclose(inputF);

	executeOperation(lastOperation, structArr, outpuF, amount, argv[optind]);

	// Zatvorit a vsetko uvolnit
	if (outpuF) {
		fclose(outpuF);
	}

	free(structArr);

	return 0;
}