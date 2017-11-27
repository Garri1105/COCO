/* The Conscious Code Project
 * 
 * COCO
 * 
 * Developed by Juan David Garrido
 */



#ifndef _COCO_H
#define _COCO_H



/* All of these following values are subject to change.
 * They will define the behavior of COCO.
 * The correct values will be achieved by trial and error */

#define SIZE_ALPHA 	27 		// Size of Alpha and follow

#define CONNECTIONS 2		// Number of connections for each word

#define ARRAY_MAX 	65535	// Max number of ThreadArray values

#define REPEAT_BASE 10		// Percentage for repeat. Memory tool
#define REPEAT_TIME 600		// Time before repeat increases

#define EXCITE_BASE 2000	// Default excite increase
#define EXCITE_TIME 10		// Time before excite decreases

#define DORMANT 	0		// Dormant memory
#define SHORT_TERM 	200		// Short-term memory
#define LONG_TERM 	2000 	// Long-term memory
#define PERMANENT 	20000 	// Permanent memory
#define DEFAULT 	250		// Default memory increase

#define FORGET_THREAD 	0
#define REPEAT_THREAD 	1
#define EXCITE_THREAD 	2
#define GLOBAL_THREADS 	3

#define MAX_FILE_NAME 	36
#define MAX_WORD_SIZE   21



#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <windows.h>

#include "cs50.c"



typedef struct state STATE;
typedef struct words WORDS;
typedef struct bond BOND;
typedef struct node LETTER;
typedef struct extension EXTENSION;

typedef struct state{
	int HAPPINESS;
	int SADNESS;
	int ANGER;
	int FEAR;
	int TRUST;
	int DISGUST;
	int ANTICIPATION;
	int SURPRISE;
}STATE;

typedef struct words{
	char word[MAX_WORD_SIZE];
	int rand;
	int repeat;
	int memory;
	bool dormant;
	STATE emotion;
	BOND* next[CONNECTIONS];
	BOND* back[CONNECTIONS];
	EXTENSION* NextExt;
	EXTENSION* BackExt;
}WORDS;

typedef struct bond{
	int phi;
	int* rand;
	bool* dormant;
	STATE* emotion;
	WORDS* next;
	WORDS* back;
}BOND;

typedef struct node{
	char letter;
	LETTER* next[SIZE_ALPHA];
	WORDS* group;
}LETTER;

typedef struct extension{
	BOND* next[CONNECTIONS];
	EXTENSION* NextExt;
}EXTENSION;



const char SAVE_PATH[] = "C:\\Users\\lenovo\\Documents\\COCO\\COCO\\SavedFiles\\";
char* inp = NULL;
char* name = NULL;
int PhrasePhi;
int FuncCheck;

STATE* COCO;

LETTER* Alpha[SIZE_ALPHA];

WORDS* root;
WORDS* EndPhrase;
WORDS* RepeatArray[ARRAY_MAX];
WORDS* ExciteArray[ARRAY_MAX];

HANDLE GlobalThreads[GLOBAL_THREADS];
HANDLE HoldEvent;
HANDLE ExitEvent;


int Welcome ();
int Init ();
int PosFunction (int letter);
int RandBetween (int min, int max);
int GetProb (BOND* array[]);
int Answer (WORDS* RootAnswer, int PhiAnswer);
int Save ();
int Load ();
int Exit ();

void SaveCOCO (FILE* save, LETTER* array[]);
void LoadCOCO (FILE* load);
void ExitCOCO (LETTER* array[]);
void SaveExt (FILE* save, EXTENSION* ext);
void SFunctions ();
void Increase ();
void Decrease (int* value);
void SortWords (WORDS* word, BOND* array[], int pos);
void CreateBond (BOND* CheckBond[], EXTENSION* NewExt, WORDS* LastWord, WORDS* NewWord);
void CreateEndBond (BOND* CheckBond[], EXTENSION* NewExt, WORDS* NewWord);
void FreeExt (EXTENSION* ext);
void Find (LETTER* array[]);
void PrintCommands ();
void Say ();
void Wrong ();
void Reverse ();
void Hear ();
void Talk ();
void ExcitePrep (WORDS* ExciteWord);
void RepeatPrep (WORDS* RepeatWord);

DWORD WINAPI Forget (LETTER* array[]);
DWORD WINAPI Excite (LPVOID lpParam);
DWORD WINAPI Repeat (LPVOID lpParam);
DWORD WINAPI Think (LPVOID lpParam);



#endif