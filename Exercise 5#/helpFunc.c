#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "myHeader.h"

Date getDate(char *line) {
	char *pold = line;

	// Rozdelim si string na rok, mesiac a den
	while(*line) {
		*line = (*line == '-') ? 0 : *line;
		line++;
	}

	line = pold;

	Date date;

	//Nasledne menim string na integer a posuvam pointer
	date.year = atoi(line);
	line = line + (strlen(line) + 1);

	date.month = atoi(line);
	line = line + (strlen(line) + 1);

	date.day = atoi(line);

	line = pold;

	return date;


}

void parseLine(char *line, Data *data) {
	// Ulozim celu riadok do rawText -> pre lepsi vypis
	strcpy(data->rawText, line);
	char *pold = line;

	// Rozdelim si riadok na: Datum, imei, gps suradnice
	while(*line) {
		*line = (*line == ' ' || *line == '\n') ? 0 : *line;
		line++;
	}

	// Spracujem jednotlive podStringy

	line = pold;
	size_t len = strlen(line);
	data->date = getDate(line);

	line = line + len + 1;

	data->imei = atoi(line);

	line = line + (strlen(line) + 1);
	Gps gps;
	memset(gps.gps_ns, 0, 11);
	memset(gps.gps_ew, 0, 11);
	strcpy(gps.gps_ns, line);
	line = line + (strlen(line) + 1);
	strcpy(gps.gps_ew, line);

	data->gps = gps;
	line = pold;

}

void clearStr(char *text) {
	// Celemu vstupnemu textu nastavim vsade 0
	memset(text, 0, 42);
}

Data *readData(FILE *file, int *amount) {

	char line[42] = {0};

	// Alokacia pola
	Data *structArr = calloc(CHUNK_SIZE, sizeof(Data));

	if (!structArr) {
		return NULL;
	}

	
	int index = 0;
	// Sluzia na realokaciu. Vzdy sa realokuje o CHUNK_SIZE * multiple
	int multiple = 1;
	char c;
	int textIndex = 0;

	// Citam subor po znaku dokym neni koniec suboru
	while((c = fgetc(file)) != EOF) {

		// Ked precitam \n znak tak sa viem ze jeden riadok je precitany
		if (c == '\n') {

			(*amount)++;
			Data data;
			parseLine(line, &data);
			clearStr(line);

			textIndex = 0;
			structArr[index++] = data;

			if (index >= (CHUNK_SIZE * multiple)) {
				multiple++;
				// Pripade potreby treba pole realokovat
				Data *tmp = (Data *)realloc(structArr, (index + CHUNK_SIZE) * sizeof(Data));
				if (!tmp) {
					free(structArr);
					return NULL;
				}

				structArr = tmp;
				}
				continue;
			}

		line[textIndex++] = c;
	}
	return structArr;
}

int compare(const void *a, const void *b) {

	Data *d1 = (Data *)a;
	Data *d2 = (Data *)b;

	// Najprv sa porovnava rok, mesiac a den

	if (d1->date.year > d2->date.year) return -1;
	if (d1->date.year < d2->date.year) return 1;
	if (d1->date.month > d2->date.month) return -1;
	if (d1->date.month < d2->date.month) return 1;
	if (d1->date.day > d2->date.day) return -1;
	if (d1->date.day < d2->date.day) return 1;
	return 0;
}

bool isDigit(char c) {
	// Ordinalna hodnota znaku
	return (c >= 48 && c <= 57);
}

bool isOnlyDigit(char *text) {
	while(*text) {
		if (!isDigit(*text)) {
			return false;
		}
		text++;
	}
	return true;
}

void executeFlagU(Data structArr[], FILE *outpuF, int amount, char *param) {
	if (!isOnlyDigit(param)) {
		return;
	}

	int givenImei = atoi(param);

	// Ak sa vystupny subor podaril otvorit
	if (outpuF) {
		for(int i = 0; i < amount; i++) {
			if (structArr[i].imei == givenImei) {
				fprintf(outpuF, "%s\n", structArr[i].rawText);
			}
		}

	// Ak vystupny subor nebol definovany (prepianc -o nebol zadany)
	}else {
		for(int i = 0; i < amount; i++) {
			if (structArr[i].imei == givenImei) {
				printf("%s\n", structArr[i].rawText);
			}
		}
	}
}

void executeFlagIO(Data structArr[], FILE *outpuF, int amount) {
	// Bol zadany len prepinac -i alebo aj -o. Tak treba data sortovat podla datumu
	qsort(structArr, amount, sizeof(Data), compare);

	// Rovnaky vypis ako vo funkcii executeFlagU
	if (outpuF) {
		for(int i = 0; i < amount; i++) {
			fprintf(outpuF, "%s\n", structArr[i].rawText);
		}
	}else {
		for(int i = 0; i < amount; i++) {
			printf("%s\n", structArr[i].rawText);
		}
	}
}

void executeOperation(char lastOperation, Data structArr[], FILE *outpuF, int amount, char *param) {

	// Jednotlive spracovanie prepinacov
	switch(lastOperation) {
		case 'i':
		case 'o':
			executeFlagIO(structArr, outpuF, amount);
			break;
		case 'u':
			executeFlagU(structArr, outpuF, amount, param);
			break;
		default:
			return;	
	}
}