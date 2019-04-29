#define _GNU_SOURCE
#include "Trie.h"
#include "/c/cs223/Hwk4/Deque.h"
#include <string.h>
#include <stdio.h> /* for printf debugging */
#include <ctype.h> /* for isalpha() functions */
#include <stdlib.h> /* for malloc() */

#define NULLBYTE '\0'	// to end strings
#define NCHILD 26 /* defines the number of child pointers in each node */

char * parseString(char * original) 
{
	int len = strlen(original);

	char * parsed = strdup(original);
	strcpy(parsed, "");
	int pindex = 0; // current index of parsed

	char currentletter; // current letter in original being analyzed
	char lower;

	for (int oindex = 0; oindex < len; oindex++)
	{
		currentletter = original[oindex];
		/* All non-alphabetic characters are ignored */
		if (!isalpha(currentletter) == 0)
		{
			/* and all alphabetic characters are converted to lower-case. */
			lower = tolower(currentletter);
			parsed[pindex] = lower;
			pindex++;
		}
	}
	parsed[pindex] = NULLBYTE; /* possibly causing the rest of the memory after the end of the string to not be freed? */
	return parsed;
}

int main(int argc, char * argv[])
{	
	/* Input parsing */
	if (!(argc == 4 || argc == 5))
	{
		printf("ladder: ladder initial goal dictionary [maxSteps]\n");
		return 0;
	}

	/*  check INITIAL and GOAL */
	char * initial = parseString(argv[1]);
	char * goal = parseString(argv[2]);

	/* INITIAL and GOAL have nonzero numbers of alphabetic characters */
	if ((strcmp(initial, "") == 0) || (strcmp(goal, "") == 0) || (strlen(initial) != strlen(goal)))
	{
		free(initial);
		free(goal);
		fprintf(stderr, "ladder: invalid initial/goal\n"); 
		return 0;
	}

	/* Parse MAXSTEPS, if it exists */
	long maxsteps = 10; 
	if (argc == 5)
	{
		/* MAXSTEPS (if present) is a sequence of decimal digits (but need not be representable using an int) */
		char * ptr;
		maxsteps = strtol(argv[4], &ptr, 10);

		/* check if maxsteps is NOT a valid integer */
		if (!((maxsteps >= 0) && (strcmp(ptr, "") == 0)))
		{
			fprintf(stderr, "ladder: invalid maxSteps\n");
			free(initial);
			free(goal);
			return 0;
		}
	}
	
	/* Parse DICTIONARY */
	// If DICTIONARY is -, then the words are read from the standard input.
	FILE * fp;
	char * line = NULL;
	size_t len = 0;

	Trie dictionary;
	Trie * Dictionary = &dictionary;

	// Extract [filename]*
	// Following code is taken and modifed from K&R reading and man getline
	fp = fopen(argv[3], "r");
	if (strcmp(argv[3], "-") == 0)
	{
		fp = stdin;
	}
	if (fp == NULL) 
	{
		fprintf(stderr, "ERROR: Null file.\n");
		free(initial);
		free(goal);
		return 0;
	}
	// use getLine() to fetch value of next file value
	else 
	{
		createT(Dictionary);
		char * modified = NULL;
		int initiallen = strlen(initial);
		while (getline(&line, &len, fp) != -1)
		{
			modified = parseString(line);
			/* Words that are too short or too long to be used in the ladder are ignored. */
			if (strlen(modified) == initiallen)
			{
				/* insert new parsed copy of original string */
				if(!insertT(Dictionary, modified, NULL, 0, true)) /* If storage bound is exceeded, return an error. Otherwise, do nothing */
				{
					free(initial);
					free(goal);
					destroyT(Dictionary);
					free(modified);
					free(line);
					fclose(fp);
					fprintf(stderr, "ladder: dictionary overflowed\n");
					return 0;
				}
			}
			free(modified);
			modified = NULL;
			free(line); /* free the original string */
			line = NULL;
		}
	}
	len = 0;
	fclose(fp);
	free(line);
	line = NULL;

	/* Check if INITIAL GOAL are even in the dictionary */
	if(!searchT(Dictionary, initial, NULL, NULL) || !searchT(Dictionary, goal, NULL, NULL))
	{
		free(initial);
		free(goal);
		destroyT(Dictionary);
		fprintf(stderr, "ladder: invalid initial/goal\n"); 
		return 0;
	}
	Trie searchtable;
	Trie * Searchtable = &searchtable;
	createT(Searchtable);

	Deque queue;
	Deque * Queue = &queue;
	createD(Queue);

	char * word = NULL;
	char * wordprime = NULL;
	int N = 0; // Arbitrary integer value

	/* Edge case: what if initial = goal and initial is in the dictionary? */
	if ((strcmp(initial, goal) == 0) && searchT(Dictionary, initial, NULL, NULL))
	{
		printf("%s\n", initial);
		destroyD(Queue);
		destroyT(Dictionary);
		destroyT(Searchtable);
		free(initial);
		free(goal);
		return 0;
	}

	// add the word INITIAL to the queue
	addD(Queue, initial);
	// add the triple (INITIAL, NULL, 0) to the search table
	insertT(Searchtable, initial, NULL, 0, false);
	// while the queue is not empty
	while (!isEmptyD(Queue))
	{
	// remove the word W at the head of the queue
		remD(Queue, &word);

	// let N be the number of steps associated with W in the search table
		searchT(Searchtable, word, NULL, &N); // get the value of N
	// if (N < MAXSTEPS)
		if (N < maxsteps)
		{
			int wordlen = strlen(word);
	// for each word W' reachable from W by a one-letter change
			for (int index = 0; index < wordlen; index++) /* for each letter in word */
			{
				char originalletter = word[index];
				for (char start = 'a'; start < ('z'+1); start++) /* create a new word by replacing the letter with one of the 26 possible characters */
				{
					wordprime = strdup(word);
					wordprime[index] = start;
					if (searchT(Dictionary, wordprime, NULL, NULL) && (start != originalletter)) /* If the word exists in the dictionary */
					{
						// if W' is the GOAL word
						if (strcmp(wordprime, goal) == 0)
						{
							// use the search table to print the sequence of words and exit
							printSequence(Searchtable, word);
							printf("%s\n", wordprime);

							/* Free storage on the queue */
							char * temp = NULL;
							while (!isEmptyD(Queue))
							{
								remD(Queue, &temp);
								free(temp);
							}

							destroyD(Queue);
							destroyT(Dictionary);
							destroyT(Searchtable);

							free(word);
							free(wordprime);
							free(goal);
							return 0;
						}
						// else if W' is not in the search table
						else if (!searchT(Searchtable, wordprime, NULL, NULL))
						{
							// add the word W' to the queue
							addD(Queue, wordprime);
							// add the triple (W', W, N+1) to the search table
							insertT(Searchtable, wordprime, word, N+1, false);
						}
						else
						{
							free(wordprime);
							wordprime = NULL;
						}
					}
					else /* skip word */
					{
						free(wordprime);
						wordprime = NULL;
					}
				}
			}
		}
		free(word);
		word = NULL;
	}
 	// <GOAL IS NOT REACHABLE FROM INITIAL>
	free(goal);
	free(wordprime);
	destroyT(Dictionary);
	destroyT(Searchtable);

	/* Free storage on the queue */
	char * temp;
	while (!isEmptyD(Queue))
	{
		remD(Queue, &temp);
		free(temp);
	}
	destroyD(Queue);
	return 0;
}








