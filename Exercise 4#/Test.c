#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // praca so znakmi
#include <string.h> // praca s retazcami
#include <unistd.h> // funkcia getopt()
#define CHUNK_SIZE 10 // Sluzi na alokaciu
 
// Funkcie som si zadefinoval este pred tym ako su volane.
int addItems(char **arr, int index, char *item);
void freeArr(char **arr, int size);
int caesar_cipher(int c, int k);
char *readInputText(void);
void trim(char *text, int isFlagW);
int isDigitOnly(char *text);
 
// ********** FUNKCIE PRE VSTUPNE PREPINACE **********
// Funkcia pre prepinac -e
int flagE(char *inputText, int num) {
	size_t len = strlen(inputText) + 1;
	char *newText = calloc(len, sizeof(char));

	if (!newText) {
		free(inputText);
		return 0;
	}

	printf("Cislo: %d\n", num);

	// Pamatam si zaciatocny pointer
	char *oldP = inputText;
	size_t count = 0;
	// Dereferencujem a pokial sa hodnota nerovna 0 tak pokracujem
	while(*inputText) {
		char c = *inputText;
		if (isalpha(c)) {
			// Kazde jedno pismenko zasifrujem
			c = caesar_cipher(c, num);
		}

		newText[count++] = c;
		inputText++;
	}

	// Vratim sa na zaciatok
	inputText = oldP;
	// Prekopirujem obsah novej pamate do starej a nasledne novu pamat uvolnim
	memmove(inputText, newText, len);
	free(newText);

	return 1;
}


// Funkcia pre prepinac -r 
int flagR(char *inputText, char *substr) {
	size_t len = strlen(substr);
	// Alokujem pamat pre hviezdicky.
	char *stars = calloc(len + 1, sizeof(char));

	if(!stars) {
		free(inputText);
		return 0;
	}
	// Pamat pre hviezdicky naplnim znakmi '*'
	memset(stars, '*', len);

	char *p;
	// Kazdy vyskyt substringu nahradim 
	while((p = strstr(inputText, substr))) {
		strncpy(p, stars, len);
	}

	// Nasledne uvolnim hviezdky
	free(stars);

	return 1;

}

// Funkcia pre prepinac -c
int flagC(char *inputText) {
	size_t len = strlen(inputText) + 1;
	char *newText = calloc(len, sizeof(char));

	if (!newText) {
		free(inputText);
		return 0;
	}

	char *oldP = inputText;
	size_t count = 0;
	// Dereferencujem
	while(*inputText) {
		// Ak sa jedna o pismeno vloz inac preskoc
		if (isalpha(*inputText)) {
			newText[count++] = *inputText;
		}
		inputText++;  
	}

	inputText = oldP;
	
	memmove(inputText, newText, count);
	inputText[count] = 0;

	free(newText);

	return 1;

}

// Funkcia pre prepinac -u a -l
/* funkcia pre prepinac -u a pre -l som spojil cez premennu 'flag' ktora moze nadobudat hodnotu 0 alebo nieco nenulove */
void flagUL(char *inputText, int flag) {
	char *oldP = inputText;

	while(*inputText) {
		// Podla toho aku hodnotu obsahuje flag tak sa vykona operacia
		// flag = 0 --> jedna sa o prepinac -l
		// flag = <> 0 --> jedna sa o prepinac -u
		*inputText = (flag) ? toupper(*inputText) : tolower(*inputText);
		inputText++;
	}

	inputText = oldP;
}
 
// funkcia pre prepinac -a
// fW = je "boolean" hodnota ktora mi vravi ci je zavolany klasicky -a prepinac alebo -w prepinac.
// pre prepinac -w len upravim dany text 
int flagA(char *inputText, int fw) {
	size_t len = strlen(inputText) + 1;
	char *tmp = (char *)calloc(len, sizeof(char));

	if (!tmp) {
		free(inputText);
		return 0;
	}

	char *p = inputText;
	int count = 0;
	while(*p) {
		tmp[count++] = (fw && isalnum(*p)) ? *p : (isalpha(*p)) ? *p : ' ';
		p++;
	}

	trim(tmp, fw);

	len = strlen(tmp) + 1;
	// Presunem novu pamat do starej a uvolnim.
	memmove(inputText, tmp, len);
	free(tmp);

	return 1;
}
 
// ********** FUNKCIE PRE VYSTUPNE PREPINACE **********
void trim(char *text, int isFlagW) {
    // Prve medzery
    size_t count  = 0;
    while(*(text + count) == ' ') {
        ++count;
    }
    if (!isFlagW) {
    	count = (count > 1) ? count - 1 : count;
    }
    size_t len = strlen(text + count) + 1;
    memmove(text, text + count, len);
    // ***************************
    // Posledne medzery
    size_t length = strlen(text);
    char *end = text + length;
    count = 0;
    while (length > 0 && *(end - count - 1) == ' ') {
        ++count;
    }
    if (!isFlagW) {
    	count = (count >= 1) ? count - 1 : count;
    }
    memmove(end - count, end, sizeof(char));
    //*********************************
    // komprimuje medzery v strede
    count = 0;
    size_t index = 0;
    while(*(text + index) != '\0') {
        if (*(text + index) == ' ' && *(text + index + 1) == ' ') {
            count = 0;
            while (text[index + count] == ' ') {
                ++count;
            }
            len = strlen(text + index + count) + 1;
            memmove(text + index + 1, text + index + count, len);
        }
        ++index;
    }
}

// Funkcia pre prepinac -w
int flagW(char *inputText) {

	// Vyuzijem upravenu funkciu pre prepinac -a
	if (!flagA(inputText, 1)) {
		return 0;
	}

	size_t len = strlen(inputText);
	// Po zavolanej funkcii flagA dostanem upraveny (trim) text.
	// Nahradim vsetky medzery znakom '\n' -> novy riadok.
	int index = 0;
	while(*inputText) {
		*inputText = (*inputText == ' ' && (index != len - 1 && index != 0)) ? '\n' : *inputText;
		inputText++;
		index++;
	}

	return 1;
}

// Funkcia pre prepinac -g
// num => po kolko sa ma text rozdelit.
int flagG(char **inputText, int num) {
	size_t len = strlen(*inputText);
	div_t division = div(len, num);

	// Prepocitavam kolko medzier vznikne.
	int whiteSpaces = (division.rem) ? division.quot : (division.quot - 1);
	size_t newLen = len + whiteSpaces + 1;

	if (!newLen) {
		free(*inputText);
		return 0;
	}

	// Alokujem novu pamat
	char *newText = calloc(newLen, sizeof(char));

	if (!newText) {
		free(*inputText);
		return 0;
	}

	// Nakolko budem do inputTextu presuvat pointer na novovytvorenu pamat tak si musim pamat pointer na tu staru pamat ktoru zahodim.
	char *pOld = *inputText;

	memset(newText, ' ', newLen - 1);
	int cycle = 1;
	int count = 0;
	int i;
	// Presucam jednotlive pismena zo starej pamate na novu
	for(i = 0; i < len; i++) {
		if (i == num * cycle) {
			cycle++;
			count++;
			i--;
		}else {
			newText[count++] = (*inputText)[i];
		}
	}

	// Vymenim pointre a nasledne uvolnim staru pamat.
	*inputText = newText;

	free(pOld);
	return 1;
}

// ********** POMOCNE FUNKCIE **********

// Funkcia nacitava vstup z terminalu.Vstupny text neni ohraniceny maximalnou dlzkou nakolko pracujem s dynamickou alokaciou
char *readInputText(void) {

	char c = 0;
    int capacity = 10;
    char *text = (char *)calloc(capacity, sizeof(char));

    if (!text) {
        return NULL;
    }

    int count = 0;
    // Nacitavam znaky pokial neni novy riadok alebo EOF
    while((c = getchar()) != EOF && c != '\n') {
        ++count;

        // Ak sa uz je dosiahnuta maximalna dlzka naalokovanej pamata tak ju realokujem o CHANK_SIZE.
        if (count == capacity) {
            capacity += CHUNK_SIZE;
            char *tmp = realloc(text, capacity);

            if (!tmp) {
                free(text);
                return NULL;
            }

            text = tmp;
        }

        text[count - 1] = c;
    }
    text[count] = 0;

    return text;
}

// male pismena => <97, 122>
// velke pismena => <65, 90>
// Sifruje dane pismeno podla 'k'
int caesar_cipher(int c, int k) {
	int n = 0;
	if (c >= 97 && c <= 122) {
		n = c % 97;
		if (k > 0) {
			return ((n + k) % 26) + 97;
		}else {
			return (n + k < 0) ? (123 + ((n + k) % 26)) : (n + k) + 97;
		}
	}else {
		n = c % 65;
		if (k > 0) {
			return ((n + k) % 26) + 65;;
		}else {
			return (n + k < 0) ? (91 + ((n + k) % 26)) : (n + k) + 65;
		}
	}
}

/* Vytvori sa pomocne zoradene pole (najprv vstupne potom vystupne prepinace)
Tato funkcia pridava jednotlive prepinace (spolu s ich parametrami) do pomocneho pola*/
int addItems(char **arr, int index, char *item) {
	size_t len = strlen(item) + 1;
	arr[index] = (char *)calloc(len, sizeof(char));
	if (arr[index] == NULL) {
		return 0;
	}

	strcpy(arr[index], item);
	return 1;
}

// Uvolnuje pomocne pole. Najprv kazdu zlozku pola a potom cele pole.
void freeArr(char **arr, int size) {
	for(int i = 0; i < size; i++) {
		free(arr[i]);
	}

	free(arr);
}

// Skontroluje ci dany text obsahuje iba cisla + zaporne znamienko.
int isDigitOnly(char *text) {
	size_t len = strlen(text);
	int i = 0;
	if (len > 0) {
		if (text[0] < '0' || text[0] > '9') {
			if (text[0] != '-') {
				return 0;
			}
			i = 1;
		}
	}
	for(; i < len; i++) {
		if (text[i] < '0' || text[i] > '9') {
			return 0;
		}
	}

	return 1;
}

/* Prechadza argv. Tato funkcia spracuvava jednotlive vstupne/vystupne prepinace a uklada ich zoradene (najprv vstupne potom vystupne) 
do pomocneho pola. V tejto funkcii je overovana aj spravnom prepinacov (ci existuje dany prepinac, ci ma pozadovany parameter...). Pri hociakej chybe 
sa vseky alokovane pamate uvolnia.
*count => adresa premennej count 
*/
char **checkParamsAndSort(int *count, int argc, char *argv[]) {
	int i;
	// Pomocne pole
	char **sortedParams = malloc((argc -1) * sizeof(char *));
	// Ci sa ma preskocit prepianc -g PARAM na zaklade prepinacu -c
	int isFlagCused = 0;

	// Najprv sa spracuju vstupne prepinace.
	// Tento prvy for loop kontroluje aj spravnost VSETKYCH prepinacov.
	for(i = 1; i < argc; i++) {
		// Prepinac -e a jeho Parameter.
		if (!strcmp(argv[i], "-e")) {
			if (i + 1 <= argc -1 && isDigitOnly(argv[i + 1])) {
				// Do pomocneho pola sa prida -e
				if (!addItems(sortedParams, (*count)++, "-e")) {
					freeArr(sortedParams, *count);
					return NULL;
				}
				// Do pomocneho pola sa prida parameter prepinacu -e
				if (!addItems(sortedParams, (*count)++, argv[i + 1])) {
					freeArr(sortedParams, *count);
					return NULL;
				}
				// ked uspeje vlozenie parametru prepinaca -e musi sa 'manualne' navysit premenna 'i' aby sme preskocili parameter a pokracovali dalej.
				i++;
			// Parameter bud neexistuje alebo ma nespravnu hodnotu.
			}else {
				freeArr(sortedParams, *count);
				return NULL;
			}
		// Prepinac -r
		}else if (!strcmp(argv[i], "-r")) {
			// Presny postup ako pri parametru -e
			if (i + 1 <= argc - 1) {
				if (!addItems(sortedParams, (*count)++, "-r")) {
					freeArr(sortedParams, *count);
					return NULL;
				}
				if (!addItems(sortedParams, (*count)++, argv[i + 1])) {
					freeArr(sortedParams, *count);
					return NULL;
				}
				i++;
			}else {
				freeArr(sortedParams, *count);
				return NULL;
			}
		// Vstupne prepinace bez parametru
		}else if (strstr("-c-a-l-u", argv[i]) != NULL) {
			if (!strcmp(argv[i], "-c")) {
				isFlagCused = 1;
			}
			if (!addItems(sortedParams, (*count)++, argv[i])) {
				freeArr(sortedParams, *count);
				return NULL;
			}
		// Vystupny prepninac bez parametru. Avsak ten sa preskoci nakolko preberame iba vstupne prepinace.
		}else if (!strcmp(argv[i], "-w")) {
			continue;
		// Vystupny prepinac -g
		}else if (!strcmp(argv[i], "-g")){
			// Okontroluje sa ci existuje parameter a ak vobec tak ci je spravny
			if (i + 1 <= argc - 1 && isDigitOnly(argv[i + 1]) && atoi(argv[i + 1]) > 0) {
				i++;
			}else {
				freeArr(sortedParams, *count);
				return NULL;
			}
		// Existuje neakceptovalny prepinac.
		}else {
			freeArr(sortedParams, *count);
			return NULL;
		}
	}

	// V tomto momente vieme ze vsetky prepinace su korektne a mozme pridat aj vystupne prepinace
	for (i = 1; i < argc; i++) {
		// preskocia sa vystupne prepinace s parametrami
		if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "-r")) {
			i++;
		// Taktiez sa preskocia vstupne prepinace bez parametrov
		}else if (strstr("-c-a-l-u", argv[i]) != NULL) {
			continue;
		// Prida sa prepianc -w
		}else if (!strcmp(argv[i], "-w")) {
			if (!addItems(sortedParams, (*count)++, argv[i])) {
				freeArr(sortedParams, *count);
				return NULL;
			}
		// Prepinac -g s parametrom sa prida ak bol parameter -c pouzity inac sa preskoci.
		}else if (!strcmp(argv[i], "-g")) {
			if (!isFlagCused) {
				i++;
				continue;
			}
			// Nasledujuce ify pridaju parameter + jeho parameter.
			if (!addItems(sortedParams, (*count)++, argv[i])) {
				freeArr(sortedParams, *count);
				return NULL;
			}
			if (!addItems(sortedParams, (*count)++, argv[i + 1])) {
				freeArr(sortedParams, *count);
				return NULL;
			}
			i++;
		}
	}

	// Vracam usporiadane pole (najprv vstupne potom vystupne)
	return sortedParams;
}
 
// Hlavna spustacia funkcia, ktora vykona postupnost
// krokov na spracovanie riadkov textu.
// Vstupmi su zname parametre 'argc' a 'argv'.
void run_text_processor(int argc, char *argv[]) {

	int count = 0;

	char **allInputs;

	// Do funkcie davam adresu premennej 'count' co mi reprezentuje dlzku pola ktora tato funkcia vracia
	// Nakolko som do funkcie checkParamsAndSort poslal adresu premennej 'count' tak ju mozem zmenit.
	// Count sluzi na spravne prechadzanie polom.
	allInputs = checkParamsAndSort(&count, argc, argv);

	if (allInputs == NULL) {
		return;
	}

	// Od tohoto momentu su vsetky prepinace zoradene a vieme ze su korektne

	// Nacitame riadok
	char *inputText = readInputText();
	if (!inputText) {
		freeArr(allInputs, count);
		return;
	}

	// Spracuvavame jednotlive parametre
	for(int i = 0; i < count; i++) {
		// spracuje sa prepinac -e s parametrom
		if (!strcmp(allInputs[i], "-e")) {
			if (!flagE(inputText, atoi(allInputs[i + 1]))) {
				// InputText je uz davno uvolneny.
				freeArr(allInputs, count);
				return;
			}
			i++;
		// spracuje sa prepinac -r
		}else if (!strcmp(allInputs[i], "-r")) {
			if (!flagR(inputText, allInputs[i + 1])) {
				// InputText je uz davno uvolneny.
				freeArr(allInputs, count);
				return;
			}
			i++;
		// Spracuje sa prepinac -c
		}else if (!strcmp(allInputs[i], "-c")) {
			if (!flagC(inputText)) {
				// InputText je uz davno uvolneny.
				freeArr(allInputs, count);
				return;
			}
		// Spracuje sa prepinac -a
		}else if (!strcmp(allInputs[i], "-a")) {
			if (!flagA(inputText, 0)) {
				// InputText je uz davno uvolneny.
				freeArr(allInputs, count);
				return;
			}
		// Spracuje sa prepinac -l
		}else if (!strcmp(allInputs[i], "-l")) {
			flagUL(inputText, 0);
		// Spracuje sa prepinac -u
		}else if (!strcmp(allInputs[i], "-u")) {
			flagUL(inputText, 1);
		// Spracuje sa prepinac -w
		}else if (!strcmp(allInputs[i], "-w")) {
			if (!flagW(inputText)) {
				// InputText je uz davno uvolneny.
				freeArr(allInputs, count);
				return;
			}
		// Spracuje sa prepinac -g s parametrom
		}else if (!strcmp(allInputs[i], "-g")) {
			if(!flagG(&inputText, atoi(allInputs[i + 1]))){
				// InputText je uz davno uvolneny.
				freeArr(allInputs, count);
				return;
			}
			i++;
		}
	}

	// Vysledok
	printf("%s\n", inputText);

	// Na konci programu vsetko uvolnim
	free(inputText);
	freeArr(allInputs, count);
 
}
 
int main(int argc, char *argv[]) {
    run_text_processor(argc, argv);
    return 0;
}