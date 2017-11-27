/* The Conscious Code Project
 * 
 * COCO
 * 
 * Developed by Juan David Garrido
 */
 

/* MISSING:

 
 * Consciousness:
 
 * A people data base: Coco has to recognize who she is talking to...
 * That way she will be able to adapt her personality accordingly...
 * Does she trust him? Does she like him? etc.
 * Instinct: There are some features that must be intrinsic and not learned...
 * The more senses she gets, the less instinctual features she should have...
 * Some learning is impossible without other senses and emotions
 * Hear() and Talk() should be threads
 * Random word creation
 * Phi implementation doesn't seem efficient
 * GetProb() and Answer() might need rework
 * Pattern recognition i.e. After a consonant, a vowel follows
 * Constant revision of own output
 * Constant predictions of outcomes. Whether a prediction succeds or not, something must always happen emotionally
 * Emotions. Reward-punishment system. Program's learning key
 * Store events. Memories. Big struct object containing emotions, words, etc.
 * Spontaneity function as a thread. Think()
 * Other senses. Hearing would be the easier bet
 * Mirror Neurons
 * When emotions are available, redo every rand function
 * Emotions:
 * * * * Anger: Something in the way of an idealized future. DO SOMETHING ABOUT IT
 * * * * Sadness: Something in the way of an idealized future. IMPOTENCE, NOTHING TO DO ABOUT IT
 * * * * Fear: Self-preservation. NOT KNOWING WHAT MIGHT HAPPEN. DO SOMETHING ABOUT IT
 * * * * Surprise: Unexpected things. PREDICTION FAILED
 * * * * Anticipation: Expected things. PREDICTION SUCCEEDED
 * * * * Disgust: Uncomfortable. Self-preservation. UNFAVORABLE PREDICTIONS
 * * * * Trust: Accepting something. FAVORABLE PREDICTIONS
 * * * * Happiness: Idealized future. IF SOMETHING IDEAL HAPPENS -> BE HAPPY

 
 * Program behavior:
 
 * Modify the compiler to link libraries cs50.h and COCO.h...
 * Using cs50.c and COCO.c is ugly and it exposes my code
 * Valgrind memcheck
 * SortWords() might not need to be utilized anymore because of Excite()...
 * SortWords makes some things easier though, like finding EndBond when answering and loading
 * Talk(): Print upper and lowercase correctly
 * Talk(): Use ? ! , .
 * Deal with special characters and numbers
 * /functions
 */
 

#include "COCO.h"


/* Greets user and initializes COCO */
int Welcome ()
{
	char c;
	
	printf("New:  1\n");
	printf("Load: 2\n");
	printf("Exit: Any Key\n\n");
	
	c = GetChar();
	
	if (c == '1')
	{
		if (Init())
		{
			printf("Error initializing COCO\n");
			return 1;
		}
		
		SetEvent(HoldEvent);    // Allows threads to run after Init() was succesfully called
		
		printf("This is Coco\n");
		printf("She knows absolutely nothing... not even her own name!\n");
		printf("She is just like a baby\n");
		printf("Talk to her and start teaching her things!\n\n");
	}
	else if (c == '2')
	{
		if (Load())
		{
			printf("Error loading COCO\n");
			return 2;
		}
	}
	else
	{
		printf("Exiting COCO...\n");
		return 3;
	}
	
	return 0;
}


/* Initialize the necessary variables to start COCO */
int Init ()
{	
	LETTER* FirstNode = (LETTER*) calloc(1,sizeof(LETTER));
	FirstNode->letter = 'a';
	Alpha[0] = FirstNode;
	
	EndPhrase = (WORDS*) calloc(1, sizeof(WORDS));
	EndPhrase->rand = INT_MAX;
	
	HoldEvent = CreateEvent(NULL, 
							TRUE, 
							FALSE, 
							NULL);
							
	ExitEvent = CreateEvent(NULL, 
							TRUE, 
							FALSE, 
							NULL);
	
	GlobalThreads[FORGET_THREAD] = CreateThread(NULL, 
								0, 
								(LPTHREAD_START_ROUTINE)Forget, 
								Alpha, 
								0, 
								NULL);
	if (GlobalThreads[FORGET_THREAD] == NULL)
	{
		printf("Error creating ForgetThread\n");
		return 1;
	}
	
	
	GlobalThreads[REPEAT_THREAD] = CreateThread(NULL, 
								0, 
								(LPTHREAD_START_ROUTINE)Repeat, 
								NULL, 
								0, 
								NULL);
	if (GlobalThreads[REPEAT_THREAD] == NULL)
	{
		printf("Error creating RepeatThread\n");
		return 2;
	}
	
	
	GlobalThreads[EXCITE_THREAD] = CreateThread(NULL, 
								0, 
								(LPTHREAD_START_ROUTINE)Excite, 
								NULL, 
								0, 
								NULL);
	if (GlobalThreads[EXCITE_THREAD] == NULL)
	{
		printf("Error creating ExciteThread\n");
		return 3;
	}
	
	return 0;
}


/* Returns a number to find the letters
 * of the Alphabet more easily */
int PosFunction (int letter)
{
	if (letter == 39)		// Character '
		return 27;

	return (letter % 32) - 1;	
}


/* Returns a random number 
 * between min and max */
int RandBetween (int bot, int top) 
{
    return rand() % (top - bot + 1) + bot;
}
																						

/* Get probability of certain word firing after the other */
int GetProb (BOND* array[])
{
	int prob, n;
	
	for (n = 0, prob = 0; n < CONNECTIONS; n++)		
	{
		if (array[n] == NULL)
			return prob;
			
		if (*(array[n]->rand) != INT_MAX)			// If rand is INT_MAX it means that the word is the NULL character
			prob = prob + *(array[n]->rand);		// Add the rand value to prob value
		else	
			prob++;									// Avoid a dividing-by-zero error
	}
	
	return prob;
}
	
																
/* Answers to anything that the user said */
int Answer (WORDS* RootAnswer, int PhiAnswer)
{
	int prob, choose, RandNum, n, m, l, k;
	BOND* CurrWord, *SelectRand[CONNECTIONS];
	EXTENSION* CurrExt;
	
	for (m = 0;;)
	{
		PhiAnswer++;
		
		for (n = 0, l = 0, memset(SelectRand, 0, sizeof(SelectRand)); n < CONNECTIONS; n++)			// Finds the appropiate responses
		{
			CurrWord = RootAnswer->next[n];
			if (CurrWord != NULL)	
			{
				if (PhiAnswer - CurrWord->phi == 0)		// If phi value matches
				{	
					SelectRand[l] = CurrWord;			// All the appropiate responses will be stored in SelectRand[]
					l++;
				}
				
				if (n == CONNECTIONS - 1 && RootAnswer->NextExt != NULL)	// If there are extensions
				{
					CurrExt = RootAnswer->NextExt;		
					
					for (k = 0; k < CONNECTIONS; k++)						// Look them up
					{
						CurrWord = CurrExt->next[k];
						if (CurrWord != NULL)
						{
							if (PhiAnswer - CurrWord->phi == 0)		
							{	
								SelectRand[l] = CurrWord;
								l++;		
							}
							
							if (k == CONNECTIONS - 1 && CurrExt->NextExt != NULL)		// If there is another extension
							{
								CurrExt = CurrExt->NextExt;		// Check next extension
								k = -1;
							}
						}
						else											
							break;
					}
				}
			}
			else
				break;
		}
		
		if (*SelectRand != NULL)		// Randomly select a response
		{
			prob = GetProb(SelectRand);		// Gets prob
		
			for (n = 0; n < CONNECTIONS; n++)
			{
				choose = RandBetween(1, prob);		// Uses prob to choose a ronadom number
				RandNum = *(SelectRand[n]->rand);	// Word's rand
				
				if (choose <= RandNum)			// If choose is less than the word's rand 
				{
					RootAnswer = SelectRand[n]->next;	// New root to use in next Answer()
					printf("%s ", RootAnswer->word);	// Print selected answer
					m++;
					break;
				}
			}
		}
		else
			break;
	}
	
	if (m == 0)		// If phi selection wasn't succesful
	{
		prob = GetProb(RootAnswer->next);	
		
		for (n = 0; n < CONNECTIONS; n++)	// Same random selection as above but using ALL root connections
		{
			CurrWord = RootAnswer->next[n];
			
			if (CurrWord == NULL)
				break;
			
			choose = RandBetween(1, prob);
			RandNum = *(CurrWord->rand);
		
			if (choose <= RandNum)
			{
				if (strcmp(CurrWord->next->word, "\0") != 0)
				{
					printf("%s ", CurrWord->next->word);
					m++;
				}
					
				PhiAnswer = CurrWord->phi;		// New phi to use in next Answer()
				RootAnswer = CurrWord->next;	// New root to use in next Answer()
				m += Answer(RootAnswer, PhiAnswer);
				break;
			}
		}
	}
	
	return m;
}


/* Saves File */
int Save ()
{
	char c;
	FILE* save = NULL;
	struct dirent* CheckFiles;
	
	DIR* SavedFiles = opendir(SAVE_PATH);
	if (SavedFiles == NULL)
	{
		printf("Error opening directory");
		return 1;
	}
	
	if (name == NULL)
	{
		get_name:
		printf("Name of new file: ");
		name = GetFileName();
	}
		
	while ((CheckFiles = readdir(SavedFiles)) != NULL)
	{	
		if (!strcmp(CheckFiles->d_name, name))
		{
			printf("There is already a file with this name. Do you wish to overwrite it?\n");
			printf("Yes: 1\n");
			printf("No:  2\n");
			
			for (;;)
			{
				c = GetChar();
			
				if (c == '1')
					break;
				else if (c == '2')
					goto get_name;
				else
					printf("Invalid command\nTry again: ");
			}
		}
	}
	
	int length = strlen(SAVE_PATH) + strlen(name);
	char file[length];
	sprintf(file, "%s%s", SAVE_PATH, name);
	
	save = fopen(file, "w");
	if (save == NULL)
	{
		printf("Error creating save file\n\n");
		return 2;
	}
	
	printf("Saving, please wait...\n");
	
	SaveCOCO(save, Alpha);
	
	fclose(save);
	
	printf("Saved\n\n");
	
	return 0;
}


/* Loads File */
int Load ()
{		
	if (*Alpha != NULL)
		if(Exit())
			printf("Loaded with errors\n");
		
	Init();
	
	int l, m, n = 0;
	FILE* LoadedFile = NULL;
	struct dirent* PrintFiles;
	
	DIR* SavedFiles = opendir(SAVE_PATH);
	if (SavedFiles == NULL)
	{
		printf("Error opening directory\n");
		return 1;
	}
	
	printf("Available files:\t");
	
	while ((PrintFiles = readdir(SavedFiles)) != NULL)
		if (*(PrintFiles->d_name) != '.')
			n++;
	
	char FileNames[n][MAX_FILE_NAME];
	
	rewinddir(SavedFiles);
	
	for (n = 0; (PrintFiles = readdir(SavedFiles)) != NULL;)
		if (*(PrintFiles->d_name) != '.')
		{
			memcpy(FileNames[n], PrintFiles->d_name, sizeof(FileNames[n]));
			n++;
		}
			
	for (m = 0; m < n; m++)
	{
		for (l = 0;; l++)
		{
			if (FileNames[m][l] == '.')
				break;
				
			printf("%c", FileNames[m][l]);
		}
		
		printf("\n\t\t\t");
	}
	
	printf("\nSelect a file to load:  ");
	
	do
	{
		name = GetFileName();
		
		for (m = 0; m < n; m++)
		{
			if (!strcmp(name, FileNames[m]))
			{
				printf("\nLoading %s...\n", name);
				int length = strlen(SAVE_PATH) + strlen(name);
				char file[length];
				sprintf(file, "%s%s", SAVE_PATH, name);
				
				LoadedFile = fopen(file, "r");
				if (LoadedFile == NULL)
				{
					printf("Error opening load file\n");
					return 2;
				}
				
				break;
			}
			
			if (m == n - 1)
			{
				printf("%s doesn't exist\n", name);
				printf("Try again:  ");
			}
		}
	}
	while (LoadedFile == NULL);
	
	LoadCOCO(LoadedFile);
	
	SetEvent(HoldEvent);
	
	closedir(SavedFiles);
	fclose(LoadedFile);
    
	printf("Done\n\n");
	
	return 0;
}


/* Frees memory and exits the program */
int Exit ()
{
	int n;
	
	SetEvent(ExitEvent);
	
	if (WaitForMultipleObjects(GLOBAL_THREADS, GlobalThreads, TRUE, 5000) == WAIT_TIMEOUT)
	{
		printf("Threads failed to close\n");
		return 1;
	}
	
	for (n = 0; n < GLOBAL_THREADS; CloseHandle(GlobalThreads[n]), n++);
	
	CloseHandle(HoldEvent);
	CloseHandle(ExitEvent);
	
	free(EndPhrase);
	
	ExitCOCO(Alpha);
	
	return 0;
}


/* Saves COCO */
void SaveCOCO (FILE* save, LETTER* array[])
{
	int n, m;
	WORDS* SaveWord;
	
	for (n = 0; n < SIZE_ALPHA; n++)
	{
		if (array[n] != NULL)
		{
			if (array[n]->group != NULL)
			{
				SaveWord = array[n]->group;
				
				fprintf(save, "%s", SaveWord->word);
				fprintf(save, " %i", SaveWord->rand);
				fprintf(save, " %i", SaveWord->repeat);
				fprintf(save, " %i", SaveWord->memory);
				fprintf(save, " %i", SaveWord->dormant);
				fprintf(save, " %i", SaveWord->emotion.HAPPINESS);
				fprintf(save, " %i", SaveWord->emotion.SADNESS);
				fprintf(save, " %i", SaveWord->emotion.ANGER);
				fprintf(save, " %i", SaveWord->emotion.FEAR);
				fprintf(save, " %i", SaveWord->emotion.TRUST);
				fprintf(save, " %i", SaveWord->emotion.DISGUST);
				fprintf(save, " %i", SaveWord->emotion.ANTICIPATION);
				fprintf(save, " %i\n", SaveWord->emotion.SURPRISE);
				
				for (m = 0; m < CONNECTIONS && SaveWord->next[m] != NULL; m++)
				{
					if (SaveWord->next[m]->phi == 0)
						fprintf(save, "0");
					else
						fprintf(save, " %s %i", SaveWord->next[m]->next->word, SaveWord->next[m]->phi);
				}
				
				if (SaveWord->NextExt != NULL)
					SaveExt(save, SaveWord->NextExt);
					
				fprintf(save, "\n");
			}
			
			SaveCOCO(save, array[n]->next);
		}
	}
	
	return;
}


/* Loads COCO */
void LoadCOCO (FILE* load)
{
	int ExtNum, phi, pos, length, num, n, m, l;
	char LoadInp[MAX_WORD_SIZE];
	LETTER* NewNode, *OldNode;
	WORDS* LastWord, *NewWord;
    BOND* NewBond, *EndBond;
	EXTENSION* Next, *NewExt, *NewExt2;
	
	while (fscanf(load, "%s", LoadInp) == 1)
	{		
		length = strlen(LoadInp);
	
		/* Main Hear() function */
		for (n = 0, m = 0, l = 0; n <= length; n++)
		{
			pos = PosFunction(LoadInp[n]);		
					
			if (m == 0)			
			{
				NewWord = (WORDS*) calloc(1, sizeof(WORDS));			
				NewWord->word[m] = LoadInp[n];							
				
				if (Alpha[pos] == NULL)
				{
					NewNode = (LETTER*) calloc(1, sizeof(LETTER));
					NewNode->letter = LoadInp[n];
					Alpha[pos] = NewNode;
				}
				else
				{
					NewNode = Alpha[pos];									
				}
				
				m++;													
			}
			else if (LoadInp[n] == '\0')			
			{		
				NewWord->word[m] = LoadInp[n];	
				
				fscanf(load, "%d", &num);
				if (num != 0)
				{
					for(l = 0; l < ARRAY_MAX; l++)
					{
						if (ExciteArray[l] == NULL) 
						{
							ExciteArray[l] = NewWord;
							break;
						}
					}
				}
				NewWord->rand = num;
				
				fscanf(load, "%d", &num);
				if (num != 100)
				{
					for(l = 0; l < ARRAY_MAX; l++)
					{
						if (RepeatArray[l] == NULL) 
						{
							RepeatArray[l] = NewWord;
							break;
						}
					}
				}
				NewWord->repeat = num;
				
				fscanf(load, "%d", &num);
				NewWord->memory = num;
				
				fscanf(load, "%d", &num);
				NewWord->dormant = num;
				
				fscanf(load, "%d", &num);
				NewWord->emotion.HAPPINESS = num;
				
				fscanf(load, "%d", &num);
				NewWord->emotion.SADNESS = num;
				
				fscanf(load, "%d", &num);
				NewWord->emotion.ANGER = num;
			
				fscanf(load, "%d", &num);
				NewWord->emotion.FEAR = num;
				
				fscanf(load, "%d", &num);
				NewWord->emotion.TRUST = num;
				
				fscanf(load, "%d", &num);
				NewWord->emotion.DISGUST = num;
				
				fscanf(load, "%d", &num);
				NewWord->emotion.ANTICIPATION = num;
				
				fscanf(load, "%d", &num);
				NewWord->emotion.SURPRISE = num;
				
				NewNode->group = NewWord;	
				
				fgetc(load);
				while(fgetc(load) != '\n'){};
				
				m = 0;			
			}		
			else if (NewNode->next[pos] == NULL)			
			{																
				NewWord->word[m] = LoadInp[n];
				OldNode = NewNode;
				NewNode = (LETTER*) calloc(1, sizeof(LETTER));
				NewNode->letter = LoadInp[n];
				OldNode->next[pos] = NewNode;
				m++;
			}
			else
			{
				NewWord->word[m] = LoadInp[n];
				NewNode = NewNode->next[pos];
				m++;
			}
		}
	}
	
	rewind(load);
	
	while (fscanf(load, "%s", LoadInp) == 1)
	{
		length = strlen(LoadInp);
		
		for (m = 0; m < length + 1; m++)
		{
			pos = PosFunction(LoadInp[m]);
			
			if (m == 0)
				NewNode = Alpha[pos];
			else if (m != length)
				NewNode = NewNode->next[pos];
			else
				LastWord = NewNode->group;
		}
		
		while(fgetc(load) != '\n'){}
		
		for(n = 0; fscanf(load, "%s", LoadInp) == 1; n++)
        {    
            if (n == 0 && isalpha(*LoadInp) == 0)
            {
                EndBond = (BOND*) calloc(1, sizeof(BOND));
				EndBond->next = EndPhrase;
				EndBond->rand = &(EndPhrase->rand);
				LastWord->next[n] = EndBond;
            }
            else
			{
				length = strlen(LoadInp);
				
				for (m = 0; m < length + 1; m++)
				{
					pos = PosFunction(LoadInp[m]);
				
					if (m == 0)
						NewNode = Alpha[pos];
					else if (m != length)
						NewNode = NewNode->next[pos];
					else
						NewWord = NewNode->group;
				}
				
				fscanf(load, "%d", &phi);
				
				NewBond = (BOND*) malloc(sizeof(BOND));		
				NewBond->rand = &(NewWord->rand);
				NewBond->dormant = &(NewWord->dormant);
				NewBond->emotion = &(NewWord->emotion);			
				NewBond->phi = phi;						
					
				NewBond->next = NewWord;						
				NewBond->back = LastWord;
				
				if (n < CONNECTIONS)
				{
					LastWord->next[n] = NewBond;							
					
					for (m = 0; m < CONNECTIONS; m++)			
					{
						if (NewWord->back[m] == NULL)			
						{												
							NewWord->back[m] = NewBond;						
							break;																
						}
						else if (m == CONNECTIONS - 1)
						{
							if (NewWord->BackExt == NULL)
							{	
								NewExt = (EXTENSION*) calloc(1, sizeof(EXTENSION));
								NewWord->BackExt = NewExt;
							}
							else
							{
								NewExt = NewWord->BackExt;
							}
							
							for (l = 0; l < CONNECTIONS; l++)
							{
								if (NewExt->next[l] == NULL)
								{
									NewExt->next[l] = NewBond;
									break;
								}   
								else if (l == CONNECTIONS - 1)
								{
									if (NewExt->NextExt == NULL)
										{
										NewExt2 = (EXTENSION*) calloc(1, sizeof(EXTENSION));
										NewExt->NextExt = NewExt2;
										NewExt = NewExt2;
									}
									else 
									{
										NewExt = NewExt->NextExt;
									}
                                
									l = -1;
								}
							}
						}																					
					}	
				}
				else 
				{
					if (LastWord->NextExt == NULL)
					{
						NewExt = (EXTENSION*) calloc(1, sizeof(EXTENSION));
						LastWord->NextExt = NewExt;
						NewExt->next[0] = NewBond;
					}
					else	
					{
						Next = LastWord->NextExt;
						
						ExtNum = n / CONNECTIONS - 1;
						
						if ((n % CONNECTIONS) == 0)
						{
							NewExt = (EXTENSION*) calloc(1, sizeof(EXTENSION));
							ExtNum--;
						}
				
						for (m = 0; m < ExtNum; Next = Next->NextExt, m++);       
						
						if (ExtNum < n / CONNECTIONS - 1)
						{
							Next->NextExt = NewExt;
							NewExt->next[0] = NewBond;
						}
						else
						{	
							m = n - CONNECTIONS * (ExtNum + 1);
								
							Next->next[m] = NewBond;
						}                
					}       	        
				}
			}
			
			if (fgetc(load) == '\n')
				break;
			else
				fseek(load, -1, SEEK_CUR);   
        }
	}	
	
	return;
}


/* Exits COCO */
void ExitCOCO (LETTER* array[])
{
	int n, m;
	WORDS* FreeWord;
	
	for (n = 0; n < SIZE_ALPHA; n++)
	{
		if (array[n] != NULL)
		{
			if (array[n]->group != NULL)
			{
				FreeWord = array[n]->group;
			
				for (m = 0; m < CONNECTIONS && FreeWord->next[m] != NULL; free(FreeWord->next[m]), m++);
	
				if (FreeWord->NextExt != NULL)
					FreeExt(FreeWord->NextExt);
									
				free(FreeWord);
			}
				
			ExitCOCO(array[n]->next);
			
			free(array[n]);
			array[n] = NULL;
		}
	}
	
	return;
}


/* Saves extensions recursively */
void SaveExt (FILE* save, EXTENSION* ext)
{
	int n;
	
	for (n = 0; n < CONNECTIONS && ext->next[n] != NULL; fprintf(save, " %s %i", ext->next[n]->next->word, ext->next[n]->phi), n++);
	
	if (ext->NextExt != NULL)
		SaveExt(save, ext->NextExt);
	
	return;
}


/* Access SpecialFunctions */
void SFunctions (char* function)
{
	/* Function Find() */
	if (!strcmp(function, "find"))
	{
		Find(Alpha);
		return;
	}
	
	/* Function PrintCommands() */
	else if (!strcmp(function, "commands"))
    {
		PrintCommands();
		return;
	}
	
    /* Function Say() */
	else if (!strcmp(function, "say")) 
	{
		Say(Alpha);		
		return;
	}
	
	/*Function Wrong() */
	else if (!strcmp(function, "wrong"))
	{
		Wrong();
		return;
	}
	
	/* Function Reverse() */
	else if (!strcmp(function, "reverse"))
	{
		Reverse();
		return;
	}
	
	/* Function Save() */
	else if (!strcmp(function, "save"))
	{
		if (Save())
			printf("Error saving COCO\n");
			
		return;
	}
	
	/* Function Load() */
	else if (!strcmp(function, "load"))
	{
		printf("Save and load:   1\n");
		printf("Load:            Any Key\n");
		
		if(GetChar() == '1')
			if(Save())
				printf("Error saving COCO\n");
			
		if (Load())
			printf("Error loading COCO\n");
			
		return;
	}
	
	/* Function Exit() */
	else if (!strcmp(function, "exit"))
	{
		printf("Save and exit:   1\n");
		printf("Exit:            Any Key\n");
		
		if(GetChar() == '1')
			if(Save())
				printf("Error saving COCO\n");
		
		printf("Exiting COCO...\n");
		if (Exit())
			printf("Exited with errors\n");
		
		return;
	}
	
	else
	{
		printf("Invalid command\n\n");
		return;
	}
}


/* Increases the memory value of a word */
void Increase (WORDS* IncreaseWord)
{
	int EmotionalState;
	STATE* emotion = &(IncreaseWord->emotion);
	
	/* Gets all the emotional values of IncreaseWord */
	EmotionalState = emotion->HAPPINESS + 
					 emotion->SADNESS   + 
					 emotion->ANGER     +
					 emotion->FEAR		+
					 emotion->TRUST 	+
					 emotion->DISGUST   + 
					 emotion->SURPRISE  + 
					 emotion->ANTICIPATION;
	
	/* Increases the memory value depending on the
	 * repeat value and the EmotionalState */
	IncreaseWord->memory += DEFAULT * (IncreaseWord->repeat/100.00) + EmotionalState;		
}


/* Decreases the memory value of a word */
void Decrease (int* value)
{
	int initial = *value;
	
	if (initial > PERMANENT)						// If memory value is bigger than PERMANENT
		*value = PERMANENT;							// Value will always be PERMANENT

	else if (initial > LONG_TERM)					// Else if memory value is bigger than LONG_TERM
		(*value)--;									// Then decrease value linearly and eventually go below LONG_TERM
	
	else if (initial > SHORT_TERM)					// Else if memory value is bigger than SHORT_TERM
		*value -= sqrt(initial - SHORT_TERM);		// Then decrease value exponentially and eventually go below SHORT_TERM
	
	else if (initial > DORMANT)						// Else if memory value is bigger than DORMANT
		if (RandBetween(1, 10) < 2)					// Random linear decrease 
			(*value)--;
	
	return;
}											
	
	
/* Sorts array of words based 
 * on value of rand */
void SortWords (WORDS* word, BOND* array[], int pos)
{
	int n, m, l, NumExt;
	BOND* temp;
	EXTENSION* SortExt;
	
	if (pos != 0)		// If pos is 0 then there is no need to Sort
	{
		if (pos >= CONNECTIONS)		// If pos is greater than CONNECTIONS it means that the word is located in an extension
		{
			NumExt = pos / CONNECTIONS;		// Get the extension where the target word is
			EXTENSION* ExtArray[NumExt];	// Array used to store the addresses of all the extensions needed
			SortExt = word->NextExt;		
			ExtArray[0] = SortExt;
			
			for (n = 0; n < NumExt - 1; n++)	// Go to the extension where the target word is
			{
				SortExt = SortExt->NextExt;		
				ExtArray[n+1] = SortExt;		// While storing addresses along the way
			}
				
			pos = pos % CONNECTIONS;			// Word position within the extension
			
			for(n = 0; n < CONNECTIONS; n++)	// Find where the word should go
			{
				if (*(SortExt->next[pos]->rand) > *(word->next[n]->rand))		// Once we find the correct position of the sorting word
				{
					temp = SortExt->next[pos];		// Save the word
					
					for(m = NumExt; m > 0; m--)		// Sort
					{
						if (m == NumExt)
						{
							for (l = pos; l > 0; l--)
								SortExt->next[l] = SortExt->next[l-1];
						}
						else
						{
							for (l = CONNECTIONS - 1, ExtArray[m]->next[0] = SortExt->next[l]; l > 0; l--)
								SortExt->next[l] = SortExt->next[l-1];
						}
							
						
						if (m > 1)
							SortExt = ExtArray[m-2];
					}
						
					for (l = CONNECTIONS - 1, ExtArray[m]->next[0] = word->next[l]; l != n; l--)
						word->next[l] = word->next[l-1];
					
					word->next[n] = temp;
				}
			}
			
			return;
		}
			
		for(n = 0; n < pos; n++)
		{
			if (*(array[pos]->rand) > *(array[n]->rand))
			{
				temp = array[pos];
				for (m = pos; m != n; array[m] = array[m-1], m--);
				array[n] = temp;
				return;
			}
		}
	}	
}


/* Creates bond */
void CreateBond (BOND* CheckBond[], EXTENSION* NewExt, WORDS* LastWord, WORDS* NewWord)
{
	int n, m, l;
	BOND* NewBond;
	EXTENSION* NewExt2;
	static int ext = 0;
	
	for (n = 0; n < CONNECTIONS; n++)		// Bond loop
	{		
		if (CheckBond[n] != NULL)						// If there is a bond already
		{
			if (strcmp(CheckBond[n]->next->word, NewWord->word) == 0)		// And the bond contains the same word as NewWord
			{
				// REMOVED: Excite(), then make the bond stronger
				PhrasePhi = CheckBond[n]->phi;								// Get the phi from the word
				PhrasePhi++;												// Prepare phi for the next bond or answer()
				SortWords(LastWord, CheckBond, n + ext);					// Sort based on rand
				return;												
			}
			else if (n == CONNECTIONS - 1)
			{
				if (LastWord->NextExt == NULL)
				{
					NewExt = (EXTENSION*) calloc(1, sizeof(EXTENSION));
					LastWord->NextExt = NewExt;
				}
				else if (NewExt == NULL)
				{
					NewExt = LastWord->NextExt;
				}
				else if (NewExt->NextExt == NULL)
				{
					NewExt2 = (EXTENSION*) calloc(1, sizeof(EXTENSION));
					NewExt->NextExt = NewExt2;
					NewExt = NewExt2;
				}
				else
				{
					NewExt = NewExt->NextExt;
				}
				
				ext += CONNECTIONS;
				CreateBond(NewExt->next, NewExt, LastWord, NewWord);
				ext -= CONNECTIONS;
				return;														
			}
		}
		else 								// If none of the conditions above are met
		{					
			NewBond = (BOND*) malloc(sizeof(BOND));		// Create the new bond
			NewBond->rand = &(NewWord->rand);
			NewBond->dormant = &(NewWord->dormant);
			NewBond->emotion = &(NewWord->emotion);			
			NewBond->phi = PhrasePhi;						// Phi is used to print the correct words when talking
			PhrasePhi++;									// Prepare for next bond or answer()
			
			NewBond->next = NewWord;						// Bond both words		
			NewBond->back = LastWord;						// Both directions		 
																				
			CheckBond[n] = NewBond;						// Finish bond forward
			SortWords(LastWord, CheckBond, n + ext);		// Sort based on rand		
			
			for (m = 0; m < CONNECTIONS; m++)				// Finish bond backward
			{
				if (NewWord->back[m] == NULL)						// We need a back bond to:
				{													// Strengthen bonds when a word is pronounced
					NewWord->back[m] = NewBond;					// MISSING: Use phi to go back in a phrase to pronounce it correctly	
					return;							// Return and move on to next word 													
				}
				else if (m == CONNECTIONS - 1)
				{
					if (NewWord->BackExt == NULL)
					{	
						NewExt = (EXTENSION*) calloc(1, sizeof(EXTENSION));
						NewWord->BackExt = NewExt;
					}
					else
					{
						NewExt = NewWord->BackExt;
					}
					
					for (l = 0; l < CONNECTIONS; l++)
					{
						if (NewExt->next[l] == NULL)
						{
							NewExt->next[l] = NewBond;
							return;
						}
						else if (l == CONNECTIONS - 1)
						{
							if (NewExt->NextExt == NULL)
							{
								NewExt2 = (EXTENSION*) calloc(1, sizeof(EXTENSION));
								NewExt->NextExt = NewExt2;
								NewExt = NewExt2;
							}
							else 
							{
								NewExt = NewExt->NextExt;
							}
							
							l = -1;
						}
					}
				}																					
			}																	
		}
	}
}


/* Creates end bond */
void CreateEndBond (BOND* CheckBond[], EXTENSION* NewExt, WORDS* NewWord)
{
	int n;
	BOND* EndBond;
	static int ext = 0;
	
	if (CheckBond[0] != NULL && !strcmp(CheckBond[0]->next->word,"\0"))	// If there is already a null terminator
	{
		return;
	}
	
	for (n = 0; n < CONNECTIONS; n++)		// Create a bond signaling the end of a phrase
	{				
		if (CheckBond[n] == NULL)
		{				
			EndBond = (BOND*) calloc(1, sizeof(BOND));
			EndBond->next = EndPhrase;
			EndBond->rand = &(EndPhrase->rand);
					
			CheckBond[n] = EndBond;					// Finish bond
			SortWords(NewWord, CheckBond, n + ext);
			return;
		}
		else if (n == CONNECTIONS - 1)
		{
			if (NewWord->NextExt == NULL)
			{
				NewExt = (EXTENSION*) calloc(1, sizeof(EXTENSION));
				NewWord->NextExt = NewExt;
			}
			else
			{
				if (NewExt == NULL)
					NewExt = NewWord->NextExt;
				else
					NewExt = NewExt->NextExt;
			}
		
			ext += CONNECTIONS;
			CreateEndBond(NewExt->next, NewExt, NewWord);
			ext -= CONNECTIONS;
			return;
		}
	}
}


/* Frees extensions recursively */
void FreeExt (EXTENSION* ext)
{
	int n;
	
	for (n = 0; n < CONNECTIONS && ext->next[n] != NULL; free(ext->next[n]), n++);
	
	if (ext->NextExt != NULL)
		FreeExt(ext->NextExt);
	
	return;
}


/* Finds and displays all the information about a specified word */
void Find (LETTER* array[])
{
	char* FindInp, c, d;
	int length, pos, n;
	WORDS* word;
	BOND* NextBond;
	EXTENSION* PrintNext, *PrintBack;
	
	
	for (;;)
	{
		printf("Find: ");
	
		FindInp = GetInput();
	
		if (*FindInp == '\0' || *FindInp == '/')
		{
			printf("Invalid Input\n\n");
			goto end;
		}
		
		length = strlen(FindInp);
		
		for (n = 0; n < length; n++)
		{
			pos = PosFunction(FindInp[n]);
			
			if (array[pos] == NULL)
			{
				word = NULL;
				break;
			}
			
			if (n != length - 1)
				array = array[pos]->next;
			else
				word = array[pos]->group;
		}
			
		if (word != NULL)
		{		
			printf("\n\"%s\" found:", FindInp);
				
				printf("\n\n\tRand:\t%i", word->rand);
				
				printf("\n\n\tRepeat:\t%i", word->repeat);
				
				printf("\n\n\tMemory:\t%i\n\t\t", word->memory);
				
				if 		(word->memory > PERMANENT) 	printf("PERMANENT");
				else if (word->memory > LONG_TERM) 	printf("LONG TERM");
				else if (word->memory > SHORT_TERM)	printf("SHORT TERM");
				else if (word->memory > DORMANT) 	printf("DORMANT");
				
				printf("\n\n\tEmotion:");
					printf("\n\t\tHappiness:\t%i", word->emotion.HAPPINESS);
					printf("\n\t\tSadness:\t%i", word->emotion.SADNESS);
					printf("\n\t\tAnger:\t\t%i", word->emotion.ANGER);
					printf("\n\t\tFear:\t\t%i", word->emotion.FEAR);
					printf("\n\t\tTrust:\t\t%i", word->emotion.TRUST);
					printf("\n\t\tDisgust:\t%i", word->emotion.DISGUST);
					printf("\n\t\tSurprise:\t%i", word->emotion.SURPRISE);
					printf("\n\t\tAnticipation:\t%i\n", word->emotion.ANTICIPATION);
					
					
			printf("\nTo display connections: 1");
			printf("\nTo find a different word: 9");
			printf("\nTo exit: Any Key\n");
			
			c = GetChar();
			
			if (c == '1')
			{
				for (n = 0; n < CONNECTIONS; n++)
				{	
					NextBond = word->next[n];
						if (NextBond != NULL)
					{
						printf("\nNext bonds:\t");
						if (NextBond->next->word[0] == '\0')
							{
							printf("%i: NULL:\n\t\t\t", n + 1);
							printf("Rand:\t NULL\n\t\t\t");
							printf("Phi:\t NULL\n");
						}
						else
						{
							printf("%i: %s:\n\t\t\t", n + 1, NextBond->next->word);
							printf("Rand:\t %i\n\t\t\t", *(NextBond->rand));
							printf("Phi:\t %i\n", NextBond->phi);
						}
					}
				}
	
				printf("\n");
				
				for (n = 0; n < CONNECTIONS; n++)
				{
					NextBond = word->back[n];
					if (NextBond != NULL)
					{
						printf("\nBack bonds:\t");
						printf("%i: %s:\n\t\t\t", n + 1, NextBond->back->word);
						printf("Rand:\t %i\n\t\t\t", *(NextBond->rand));
						printf("Phi:\t %i\n", NextBond->phi);
					}
				}
			
				PrintNext = word->NextExt;
				PrintBack = word->BackExt;
				
				for (;;)
				{
					if (PrintNext != NULL) printf("\nTo display NextExt connections: 1");
					else				   printf("\nNo NextExt connections found");
					
					if (PrintBack != NULL) printf("\nTo display BackExt connections: 2");
					else                   printf("\nNo BackExt connections found");
				
					printf("\nTo find a different word: 9");
					printf("\nTo exit: Any Key\n");
				
					d = GetChar();
				
					if (d == '1' && PrintNext != NULL)
					{
						for (n = 0; n < CONNECTIONS; n++)
						{
							NextBond = PrintNext->next[n];
							if (NextBond != NULL)
							{
								printf("\nNextExt bonds:\t");
								printf("%i: %s:\n\t\t\t", n + 1, NextBond->next->word);
								printf("Rand:\t %i\n\t\t\t", *(NextBond->rand));
								printf("Phi:\t %i\n", NextBond->phi);
							}	
						}	
					
						PrintNext = PrintNext->NextExt; 
					
					}
					else if (d == '2' && PrintBack != NULL)
					{	
						for (n = 0; n < CONNECTIONS; n++)
						{	
							NextBond = PrintBack->next[n];
							if (NextBond != NULL)
							{
								printf("\nBackExt bonds:\t");
								printf("%i: %s:\n\t\t\t", n + 1, NextBond->back->word);
								printf("Rand:\t %i\n\t\t\t", *(NextBond->rand));
								printf("Phi:\t %i\n", NextBond->phi);
							}	
						}
					
						PrintBack = PrintBack->NextExt;
					}
					else if (d == '9')
					{	
						break;
					}
					else
					{
						printf("Exiting Find()...\n\n");
						free(FindInp);
						return;
					}
						
				}
			}
			else if (c == '9'){}
			else break;
		}
		else
		{
			printf("\"%s\" not found\n", FindInp);
			printf("\nTo find a different word: 9");
			printf("\nTo exit: Any Key\n");
			
			if (GetChar() == '9'){}
			else break;				
		}
		
		end:
		free(FindInp);
		array = Alpha;
	}
	
	printf("Exiting Find()...\n\n");
	free(FindInp);
	return;
}


/* Prints available commands */
void PrintCommands()
{
	printf("find\n");
	printf("save\n");
	printf("load\n");
	printf("exit\n");
	return;
}


/* When the command /say is selected, the function say will
 * recover the last word heard and link it to the answer
 * that the user will type in. That way the user can say
 * How are you? /say I am good. And the program will remember
 * that answer for next time. */
void Say ()
{
	char* SayInp;
	int pos, length, n, m;
	LETTER* NewNode, *OldNode;
	WORDS* NewWord;
	EXTENSION* NewExt = NULL;

	if (root == NULL)
	{
		printf("Incorrect use of function\n\n");
		return;
	}
	
	printf("Say: ");
	
	SayInp = GetInput();
	
	if (*SayInp == '\0' || *SayInp == '/')
	{
		printf("Invalid Input\n\n");
		return;
	}
	
	length = strlen(SayInp);
	
	PhrasePhi++;
	
	/* The function is exactly the same as hear() 
	 * but with a few changes, using PhrasePhi 
	 * and root more effectively */
	for (n = 0, m = 0; n <= length; n++)
	{
		pos = PosFunction(SayInp[n]);
		 
		if (m == 0)			// As stated above: Used to differentiate between Alpha and NewNode->next
		{
			NewWord = (WORDS*) calloc(1, sizeof(WORDS));			// NewWord is used to create the word web that is the heart of the program
			NewWord->word[m] = inp[n];								// Fill the NewWord one by one
			
			if (Alpha[pos] == NULL)
			{
				NewNode = (LETTER*) calloc(1, sizeof(LETTER));
				NewNode->letter = inp[n];
				Alpha[pos] = NewNode;
			}
			else
			{
				NewNode = Alpha[pos];									// NewNode will be used from now on
			}
			
			m++;													// Signals that Alpha will not be used anymore
		}
		else if (pos == -1 || SayInp[n] == '\0')
		{		
			NewWord->word[m] = '\0';
			
			if (NewNode->group == NULL)
				NewNode->group = NewWord;
			else
			{
				free(NewWord);
				NewWord = NewNode->group;
			}
				
			Increase(NewWord);
			ExcitePrep(NewWord);
			RepeatPrep(NewWord);
			
			CreateBond(root->next, NewExt, root, NewWord);
			
			root = NewWord;
			
			if (n == length)
				CreateEndBond(root->next, NewExt, root);
			
			m = 0;
		}
		else if (NewNode->next[pos] == NULL)
		{
			NewWord->word[m] = SayInp[n];
			OldNode = NewNode;
			NewNode = (LETTER*) calloc(1, sizeof(LETTER));
			NewNode->letter = SayInp[n];
			OldNode->next[pos] = NewNode;
			m++;
		}
		else
		{
			NewWord->word[m] = SayInp[n];
			NewNode = NewNode->next[pos];
			m++;
		}
	}
	
	printf("\n");
	return;
}


/* In Progress */
void Wrong ()
{
	return;
}


/* In Progress */
void Reverse ()
{
	return;
}


/* Key function. Listens to anything that the user types.
 * It is the learning key of the program. It learns everything 
 * that it hears and then uses that knowledge to talk like a human.
 * It also gives access to /functions */
void Hear ()
{
	int fun, pos, length, n, m, l;
	LETTER* NewNode, *OldNode;
	WORDS* LastWord, *NewWord;
	EXTENSION* NewExt = NULL;

	inp = GetInput();
	
	length = strlen(inp);
	
	/* Error Checking*/
	if (*inp == '\0')				// If there is no input
	{
		fun = RandBetween (1, 5);  // Print randomly the following strings
		if (fun == 1)
			printf("Stop\n\n");
		if (fun == 2)
			printf("Say something\n\n");
			
		return;
	}
	
	FuncCheck = 0;				// If /function() is not accesed, then we can allow the program to use talk() later on
	
	/* Access SpecialFunctions */ 
	if (*inp == '/')			// They can be accesed by typing '/' in front of the command
	{
		inp = inp + 1;
		
		FuncCheck = 1;			// Check to avoid the function talk() later on
		
		SFunctions(inp);

		return;
	}
		
	/* Main Hear() function */
	for (n = 0, m = 0, l = 0; n <= length; n++)
	{
		pos = PosFunction(inp[n]);		// Find the position of the selected letter inp[n]
		
		/* Excite rand and create new nodes based on input */
		
		if (m == 0)			// As stated above: Used to differentiate between Alpha and NewNode->next
		{
			NewWord = (WORDS*) calloc(1, sizeof(WORDS));			// NewWord is used to create the word web that is the heart of the program
			NewWord->word[m] = inp[n];								// Fill the NewWord one by one
			NewWord->repeat = 100;									// Repeat value for a better memory algorithm
			
			if (Alpha[pos] == NULL)
			{
				NewNode = (LETTER*) calloc(1, sizeof(LETTER));
				NewNode->letter = inp[n];
				Alpha[pos] = NewNode;
			}
			else
			{
				NewNode = Alpha[pos];									// NewNode will be used from now on
			}
			
			m++;													// Signals that Alpha will not be used anymore
		}
		else if (pos == -1 || inp[n] == '\0')			// When the end of a word is reached, continue
		{		
			NewWord->word[m] = '\0';
			
			if (NewNode->group == NULL)							
				NewNode->group = NewWord;					// If the NewWord is not yet in the word web, add it
			else
			{	
				free(NewWord);
				NewWord = NewNode->group;					// If it is, just use the original NewWord address
			}
			
			Increase(NewWord);
			ExcitePrep(NewWord);
			RepeatPrep(NewWord);
			
			if (l == 0)				// For the first word of inp[n]:
			{
				PhrasePhi = RandBetween(1,INT_MAX);		// Just initialize PhrasePhi to create strong bonds between the words in inp[n]
				LastWord = NewWord;					// Store the address of NewWord to create the first bond
			}
			else
			{
				CreateBond(LastWord->next, NewExt, LastWord, NewWord);
	
				LastWord = NewWord;			// Store address to create next bond
			}
			
			if (n == length)			// When the end of inp[n] is reached
			{
				CreateEndBond(NewWord->next, NewExt, NewWord);
				
				root = NewWord;				// Store in root to use it in /functions or in answer()
			}
			
			m = 0;			// Allows for Alpha to be used again
			l++;			// l is used to signal the #word we are working on
		}
		else if (NewNode->next[pos] == NULL)			
		{																
			NewWord->word[m] = inp[n];
			OldNode = NewNode;
			NewNode = (LETTER*) calloc(1, sizeof(LETTER));
			NewNode->letter = inp[n];
			OldNode->next[pos] = NewNode;
			m++;
		}
		else
		{
			NewWord->word[m] = inp[n];
			NewNode = NewNode->next[pos];
			m++;
		}
	}
}


/* Key function 2. Uses the knowledge that hear() just acquired to respond appropiately */
void Talk ()
{	
	if (FuncCheck == 1 || *inp == '\0')
		return;
	
	if (Answer(root, PhrasePhi) != 0)
	{
		printf("\n\n");
		return;
	}
	
	printf("\n\n");	
	return;
}


/* Preparation for ExciteThread work on ExciteWord */
void ExcitePrep (WORDS* ExciteWord)
{
	int EmotionalState, n;		
	STATE* emotion = &(ExciteWord->emotion);
	
	EmotionalState = emotion->HAPPINESS + 
					 emotion->SADNESS   + 
					 emotion->ANGER     +
					 emotion->FEAR		+
					 emotion->TRUST 	+
					 emotion->DISGUST   + 
					 emotion->SURPRISE  + 
					 emotion->ANTICIPATION;
					 
	
	for (n = 0; n < ARRAY_MAX; n++)
	{
		if (ExciteArray[n] != NULL) 
		{
			if (!strcmp(ExciteWord->word, ExciteArray[n]->word))
			{
				ExciteArray[n]->rand += EXCITE_BASE + EmotionalState;
				break;
			}
		}
		else
		{	
			ExciteWord->rand += EXCITE_BASE + EmotionalState;
			ExciteArray[n] = ExciteWord;
			break;
		}
	}
}


/* Preparation for RepeatThread work on RepeatWord */
void RepeatPrep (WORDS* RepeatWord)
{
	int n;
	
	for(n = 0; n < ARRAY_MAX; n++)
	{
		if (RepeatArray[n] != NULL) 
		{
			if (!strcmp(RepeatWord->word, RepeatArray[n]->word))
			{
				RepeatArray[n]->repeat = REPEAT_BASE;
				break;
			}
		}
		else
		{	
			RepeatWord->repeat = REPEAT_BASE;
			RepeatArray[n] = RepeatWord;
			break;
		}
	}
}


/* Constantly decreases the memory value of words */
DWORD WINAPI Forget (LETTER* array[])
{
	int n, m, *MemVal;
	static int level = 0;
	WORDS* ForgetWord;
	DWORD lpExitCode;
	
	if (WaitForSingleObject(ExitEvent, 0) == WAIT_OBJECT_0)
	{
		GetExitCodeThread(GlobalThreads[FORGET_THREAD], &lpExitCode);
		ExitThread(lpExitCode);
	}
	
	do
	{
		for (n = 0; n < SIZE_ALPHA; n++)
		{
			Sleep(1);
			if (array[n] != NULL)
			{
				if (array[n]->group != NULL)
				{
					ForgetWord = array[n]->group;
					MemVal = &(ForgetWord->memory);
					
					Decrease(MemVal);
					
					if (*MemVal == SHORT_TERM)
					{
						ForgetWord->dormant = TRUE;
					}
					else if (*MemVal == DORMANT)
					{
						for (m = 0; m < CONNECTIONS; free(ForgetWord->next[m]), m++);
						free(ForgetWord);
						array[n]->group = NULL;
					}
				}
				
				level++;
				Forget(array[n]->next);
				level--;
			}
		}	
	}
	while(level == 0);

	return 0;
}


/* Increases the repeat value for a proper increase in memory */
DWORD WINAPI Repeat (LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);

	int n, m;
	DWORD lpExitCode;
	
	WaitForSingleObject(HoldEvent, INFINITE);
	
	for (;;)
	{	
		if (WaitForSingleObject(ExitEvent, 0) == WAIT_OBJECT_0)
		{
			GetExitCodeThread(GlobalThreads[REPEAT_THREAD], &lpExitCode);
			ExitThread(lpExitCode);
		}
		
		Sleep(REPEAT_TIME);
		
		for (n = 0; n < ARRAY_MAX; n++)
		{
			if (RepeatArray[n] == NULL)
				break;
			
			if (++(RepeatArray[n]->repeat) == 100)
			{			
				for (m = n; m < ARRAY_MAX; m++)
				{
					RepeatArray[m] = RepeatArray[m+1];
					
					if (RepeatArray[m+1] == NULL)
						break;
				}

				n--;
			}
		}
	}

	return 0;
}


/* Excites the rand argument exponentially */
DWORD WINAPI Excite (LPVOID lpParam)											
{												
	UNREFERENCED_PARAMETER(lpParam);
	
	int n, m;
	DWORD lpExitCode;

	WaitForSingleObject(HoldEvent, INFINITE);
	
	for (;;)
	{	
		if (WaitForSingleObject(ExitEvent, 0) == WAIT_OBJECT_0)
		{
			GetExitCodeThread(GlobalThreads[EXCITE_THREAD], &lpExitCode);
			ExitThread(lpExitCode);	
		}
		
		Sleep(EXCITE_TIME);
		
		for (n = 0; n < ARRAY_MAX; n++)
		{
			if (ExciteArray[n] == NULL)
				break;
			
			if (--(ExciteArray[n]->rand) == 0)
			{			
				for (m = n; m < ARRAY_MAX; m++)
				{
					ExciteArray[m] = ExciteArray[m+1];
					
					if (ExciteArray[m+1] == NULL)
						break;
				}

				n--;
			}
		}
	}
		
	return 0;
}


/* Creates spontaneous input */
DWORD WINAPI Think (LPVOID lpParam)
{
	// Check Emotions
	
	// Get a word randomly based on emotions and excited words
	
	// Now that we have a word, we can think of a phrase or an event related to the word
	
	// Talk() might be triggered
	
	// Selected words affect memory, emotions, etc.
	
	// Start over
	return 0;
}
