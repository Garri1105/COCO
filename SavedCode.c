/* Initializes a LEVELS dimmensional structure of letters.
 * This is the main array. The program will store every word in here */
void Init (LETTER* array[])
{
	int n;
	static int size = 0;
	LETTER* NewNode;
	
	for (n = 0; n < SIZE_ARRAY; n++)
	{
		NewNode = (LETTER*) calloc(1, sizeof(LETTER));				// MISSING: deal with special characters. ', ?, !, ,, ., ...
		
		if (n == SIZE_ARRAY - 2)
			NewNode->letter = '\'';
		else if (n == SIZE_ARRAY - 1)
			NewNode->letter = '-';
		else
			NewNode->letter = (char) n + 65;
			
		NewNode->rand = RandBetween(20, 100);
		array[n] = NewNode;
		
		if (size + 1 < LEVELS)
		{
			size++;
			Init(array[n]->next);
			size--;
		}
	}
}

/* Dictionary Init */
void Init (LETTER* array[])
{
	char c;
	int pos, n, m = 0;
	LETTER* NewNode;
	
	WORDS* NewWord;
	NewWord = (WORDS*) calloc(1, sizeof(WORDS));
	
	printf("Initializing...\n");
	
	FILE* dictionary = fopen("dictionary.txt", "r");
	
	if (dictionary == NULL)
	{
		printf("Error while opening file\n\n");
		return;
	}
	
	c = fgetc(dictionary);
		
	for (n = 0; c != EOF; n++)
	{
		if (c != '\n')
		{
			pos = Func(c);
			
			if (m == 0)
			{
				if (Alpha[pos] == NULL)
				{
					NewNode = (LETTER*) calloc(1, sizeof(LETTER));
					NewNode->letter = c;
					NewNode->rand = 1;
					Alpha[pos] = NewNode;			
				}
			
				array = Alpha[pos]->next;
			}
			else
			{
				if (array[pos] == NULL)
				{
					NewNode = (LETTER*) calloc(1, sizeof(LETTER));
					NewNode->letter = c;
					NewNode->rand = 1;
					array[pos] = NewNode;			
				}
				
				array = array[pos]->next;
			}	
		
			NewWord->word[m] = c;
			m++;
		}
		else
		{
			NewNode->group = NewWord;
			NewWord = (WORDS*) calloc(1, sizeof(WORDS));
			m = 0;
		}
		
		c = fgetc(dictionary);
	}
	
	fclose(dictionary);
}


/* Sorts array of letters based 
 * on value of rand */
int SortLetter (LETTER* array[], int pos)
{
	LETTER* temp;
	int n, m;
	if (pos != 0)
	{
		for(n = 0; n < pos; n++)
		{
			if (array[pos]->rand > array[n]->rand)
			{
				temp = array[pos];
				for (m = pos; m != n; array[m] = array[m-1], m--);
				array[n] = temp;
				return n;
			}
		}
	}
	return pos;
}


/* In Hear and Say*/

/* Find the position of the selected letter inp[n] */
		temp = Func(inp[n]);   // Use func to store the true value of inp[n]
		
		if (temp != -1)		   // If inp[n] is not a space then continue
		{
			if (m == 0)		   // m is used to differentiate between array and NewNode->next
			{
				for (pos = 0; pos < SIZE_ARRAY; pos++)
				{
					if (temp == Func(Alpha[pos]->letter))    	 // Compare inp[n] true value with the current letter true value. Func is essential
						break;									 // If there is a match, we now know the position of inp[n] in the array
				}												 // That position is stored in pos
			}
			else
			{
				for (pos = 0; pos < SIZE_ARRAY; pos++)		 // Same process as above but using NewNode->next
				{
					if (NewNode->next[pos] != NULL)		 	 // We have to check if NewNode->next[pos] is not NULL before proceeding
					{										 	// This problem doesn't appear in Alpha because we know it is completely full from init()
						if (temp == Func(NewNode->next[pos]->letter))
						break;
					}
					else 
						break;
				}
			}
		}
		
		
void RandWord (LETTER* array[])
{
	int prob, choose, RandNum, n;
	WORDS* phrase = NULL;
	
	for(;;)
	{
		for (n = 0, prob = 0; n < SIZE_ARRAY; n++)
		{
			if (array[n] == NULL)
				break;
				
			prob = prob + array[n]->rand;
		}
		
		for (n = 0; n < SIZE_ARRAY; n++)
		{
			if (array[0] == NULL)
			{
				printf("\n\n");
				return;
			}
				
			else if (array[n] == NULL)
				n = 0;	
				
			choose = RandBetween(1, prob);
			RandNum = array[n]->rand;
			
			if (choose <= RandNum)
			{
				printf("%c", array[n]->letter);
			
				if (array[n]->group != NULL)
				{
					if (array[n]->next[0] != NULL)
					{
						if (RandBetween(1, RandNum + array[n]->next[0]->rand) <= RandNum)
						{
							phrase = array[n]->group;
							printf(" ");
							break;	
						}
					}
					else
					{
						phrase = array[n]->group;
						printf(" ");
						break;
					}
				}
				
				array = array[n]->next;
				break;
			}
		}
		
		if (phrase != NULL)
		{
			if (phrase->next[0] != NULL)
				Answer(phrase, RandBetween(1,MAX));
			
			printf("\n\n");
			return;
		}
	}	
}