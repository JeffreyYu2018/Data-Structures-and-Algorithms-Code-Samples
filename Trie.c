/* Referenced Aspnes notes for help with implementation */
#define _GNU_SOURCE
#include <stdlib.h> /* for malloc () */
#include "Trie.h"
#include <stdbool.h> /* define true and false */
#include <string.h>
#include <stdio.h> /* for printF */

#define NULLBYTE '\0'
#define NCHILD 26 /* defines the number of child pointers in each node */
#define STORAGEBOUND 65536 /* defines the maximum number of words that may be added */

// long dictWords = 0;

static int dictWords = 0;
static int searchWords = 0;

struct trie { /* trie will be headed by an empty node */
	char * word; /* a word that has been reached */
	char * reachedfrom; /* the word from which it was first reached by a one-letter change (or NULL for the INITIAL word) */
	int nsteps;	/* the number of transitions required to get to WORD from the INITIAL word */
	Trie * children;
};

// Set *T to a new object of type Trie.
// Returns true if successful, false otherwise (e.g., malloc() fails).
void createT (Trie *t)
{
	/* malloc space for T */
    *t = malloc(sizeof(struct trie));

    (*t)->word = NULL;
    (*t)->reachedfrom = NULL;
    (*t)->nsteps = 0;
    (*t)->children = malloc(NCHILD * sizeof(Trie *)); /* allocate space for 26 child pointers */

    /* initialize list of children */
    for (int index = 0; index < NCHILD; index++)
    	(*t)->children[index] = NULL;
}

// Return true if the Trie *T is empty, false otherwise.  The value of *T may
//   change as a result.
bool isEmptyT (Trie *t)
{
	return ((*t)->children == NULL) && ((*t)->word == NULL) && ((*t)->reachedfrom == NULL);
}

/* returns true if trie contains word. 
   If word is found, 
	   N is updated with the value of nsteps
	   reachedfrom is updated with the value of reachedfrom
   Otherwise, they are left alone. */
bool searchT(Trie *t, char * word, char ** reachedfrom, int *N)
{
	if (isEmptyT(t))
		return false;

	int len = strlen(word);
	int current = 0; // keeps track of the current letter being checked in s
	Trie * l = t;

	int letterindex;

	Trie currentChild = NULL;

	while (current < len) 
	{
		// Follow link specified by next letter, if possible
		letterindex = word[current]-'a';
		currentChild = (*l)->children[letterindex];
		if (currentChild == NULL) // if there is no link, then key not in trie
			return false;
		else
			l = &currentChild;
		current++;
	}
	// If final node is reached, return if its word is equal to the desired word 
	/* NULL means that this field doesn't matter */
	if (N != NULL)
		(*N) = (*l)->nsteps;
	if (reachedfrom != NULL) 
		(*reachedfrom) = (*l)->reachedfrom;

	return ((*l)->word != NULL) && (strcmp((*l)->word, word) == 0);
}

/* use the search table to print the sequence of words from goal to initial */
void printSequence(Trie *Searchtable, char * reachedfrom) /* reachedfrom's default value is the reachedfrom of goal */
{
	char * currentword;

	if (reachedfrom == NULL)
		return;

	currentword = strdup(reachedfrom);
	searchT(Searchtable, currentword, &reachedfrom, NULL);
	printSequence(Searchtable, reachedfrom);
	printf("%s\n", currentword);
	free(currentword);
	currentword = NULL;
}

// Add the string pointer word to the trie T; the string itself is
//   not duplicated.  The value of *T may change as a result.
//
// To ensure that the storage used remains bounded, the trie module monitors
// the number of words added, returning an error indicator instead of inserting
// a new entry when the number exceeds 65536.  ladder issues a one-line error
// message and exits on receiving this indicator.
//
// Return false if storage bound is exceeded, true otherwise
// dictionary = true means the Trie *t is the dictionary, dictionary = false means the Trie *t is the search Table
bool insertT(Trie *t, char * word, char * reachedfrom, int nsteps, bool dictionary) 
{
	// Check for bounded storage
	if ((dictionary && (dictWords > STORAGEBOUND)) || (!dictionary && (searchWords > STORAGEBOUND)))
		return false;

	int len = strlen(word);
	int current = 0; // keeps track of the current letter being checked in s
	Trie * l = t;

	int letterindex;

	while (current < len) 
	{
		letterindex = word[current]-'a';
		
		if ((*l)->children[letterindex] == NULL) // If there is no link, then create new path to key
			(*l)->children = addChild(l, letterindex);

		// Follow link specified by next letter
		l = &((*l)->children[letterindex]);
		current++;
	}
	/* if the word already exists within the dictionary, do not add the string */
	if (!(((*l)->word != NULL) && (strcmp((*l)->word, word) == 0)))
	{
		// update the last node with word, reachedfrom, and nsteps, strduping
		if (word != NULL)
			(*l)->word = strdup(word);
		if (reachedfrom != NULL)
			(*l)->reachedfrom = strdup(reachedfrom);
		(*l)->nsteps = nsteps;

		// update the static int word counters
		if (dictionary)
			dictWords++;
		else
			searchWords++;
	}
	return true;
}	

// Adds a letter child pointer to a Trie's children
Trie * addChild(Trie * t, int letterindex) 
{
	Trie * children = (*t)->children;

	/* create new child */
	Trie new;
	createT(&new);

	// Add new pointer 
	children[letterindex] = new;

	return children;
}

// Destroy the Trie *T by freeing any storage that it uses and set the value of *T to NULL.
// Return true if successful, false otherwise (e.g., T is an invalid argument).
// destroyT also frees the strings passed to it.
void destroyT(Trie *t)
{
	Trie currentChild;
	for (int index = 0; index < NCHILD; index++)
	{
		currentChild = (*t)->children[index]; 
		if (currentChild != NULL)
			destroyT(&currentChild);
	}
	// Free the children, the word, reachedfrom, and Trie itself
	if ((*t)->children != NULL)
	{
		free((*t)->children);
		(*t)->children = NULL;
	}
	if ((*t)->word != NULL)
	{
		free((*t)->word);
		(*t)->word = NULL;
	}
	if ((*t)->reachedfrom != NULL)
	{
		free((*t)->reachedfrom);
		(*t)->reachedfrom = NULL;
	}
	if ((*t) != NULL)
	{
		free(*t);
		(*t) = NULL;
	}
}

/* debugging utility: print all keys in trie */
void printT(Trie *t)
{
	Trie currentChild;
	char * currentWord;
	char * currentrf;
	for (int index = 0; index < NCHILD; index++)
	{
		currentChild = (*t)->children[index]; 
		if (currentChild != NULL)
		{
			printf("currentChild[%d]'s and word = ", index);
			currentWord = currentChild->word;
			if (currentWord != NULL)
				printf("%s", currentWord);
			else
				printf("NULL");

			printf(" and reached from = ");
			currentrf = currentChild->reachedfrom;
			if (currentrf != NULL)
				printf("%s\n", currentrf);
			else
				printf("NULL\n");

			printT(&currentChild);
		}
		
	}
}











