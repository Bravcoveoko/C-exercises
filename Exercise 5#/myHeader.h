#define CHUNK_SIZE 200 // Sluzi na realloc 

/*
* Struktura pre datum
* Vybral som int typy kvoli tomu aby sa mi lepsie potom zoradovali
*/
typedef struct _date {
	int year;
	int month;
	int day;
} Date;

/*
* Struktura pre GPS suradnice
*/
typedef struct _gps {
	char gps_ns[11];
	char gps_ew[11];
} Gps;

/*
* Strultura pre obalenie dat
* rawText -> cely nacitany riadok zo suboru
*/
typedef struct _data {
	Gps gps;
	Date date;
	int imei;
	char rawText[42];
} Data;


/*
* Preparsuje riadok a ziska datum
* line -> text z ktoreho sa ziskava datum
*/
Date getDate(char *line);

/*
* Parsuje riadok a ziskava z neho potrebne data (datum, imei, suradnice)
* line -> riadok ktory sa ma preparsovat
* data -> struktura do ktorej sa budu vkladat preparsovane udaje
*/
void parseLine(char *line, Data *data);

/*
* Premenna do ktorej sa zapisuju znaky zo suboru sa po kazdom precitani riadku precisti
* text -> premenna ktora sa ma precistit
*/
void clearStr(char *text);

/*
* Citanie samotnych riadkov zo suboru
* file -> subor z ktoreho citame
* amount -> pocet nacitanych riadkov
*/
Data *readData(FILE *file, int *amount);

/*
* Compare funkcia sluzia pre qsort funkciu ktora zosortuje data podla datumu od najnovsieho po najstrasi
* a -> jeden prvok dat
* b -> druhy prvok dat
*/
int compare(const void *a, const void *b);

/*
* Ci je dany znak cislo
* c -> znak ktory ma byt kontolovany
*/
bool isDigit(char c);

/*
* Ci dany text obsahuje iba cisla
* text -> string ktory ma byt okontrolovany
*/
bool isOnlyDigit(char *text);

/*
* Funkcia pre spracovanie prepinacu -u
* structArr -> pole dat
* outputF -> subor do ktoreho sa ma v pripade ze neni NULL zapisovat
* amount -> pocet prvkov v structArr
* param -> parameter pepiancu -u
*/
void executeFlagU(Data structArr[], FILE *outpuF, int amount, char *param);

/*
* Funkcia pre spracovanie prepinacu -i -o
* structArr -> pole dat
* outputF -> subor do ktoreho sa ma v pripade ze neni NULL zapisovat
* amount -> pocet prvkov v structArr
*/
void executeFlagIO(Data structArr[], FILE *outpuF, int amount);

/*
* Funkcia ktora rozhodne ktory prepinac sa ma spracovat
* lastOperation -> posledny prepinac ktory bol zadany
* structArr -> pole dat
* outputF -> vystupny subor
* amount -> pocet prvkov
* param -> parameter
*/
void executeOperation(char lastOperation, Data structArr[], FILE *outpuF, int amount, char *param);