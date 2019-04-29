#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>	/* Needed for free() function */
#include <string.h>
#include <limits.h> /* Needed for INT_MAX */
#include "/c/cs223/Hwk4/Deque.h"
#include <stdbool.h>

#define NULLBYTE '\0'

long min(long a, long b);
void mergeSort(Deque * queue, Deque * stack, int pos, int len, int size, bool left);
void merge(Deque * queue, Deque * stack, int pos, int len, int qsize, int ssize);
int extractKey(char * qval, char * sval, int pos, int len);

int main(int argc, char * argv[]){
	/* ask: how can *D be an invalid argument for function destroyD? */
	/* check for valid argument list */
	if (argc == 1)
		return 0;

	int count = 1; /* Index of current argument being parsed */

	/* variables needed for the parsing of -POS[,LEN] */
	char * poslen = argv[count]; /* poslen represents the value being parsed */
	/* Negative values are sentinel values */
	long pos = -1;
	long len = -1;

	/* check to see if the second argument is a valid -POS[,LEN] */
	if (poslen[0] == '-') /* treat second argument as -POS[,LEN] */
	{
		/* parse POS, if it exists */
		/* ignore the starting '-' in poslen from now on */
		poslen += 1; 
		/* check that the length of -POS[,LEN] is greater than 1 */
		if (!(strlen(poslen) > 0))
		{
			fprintf(stderr, "Invalid -POS[,LEN]\n");
			return 0;
		}
		
		char *posptr;

	  	pos = strtol(poslen, &posptr, 10);	// omit the first '-' character

	  	/* check if pos is a valid integer */
	  	if ((pos >= 0) && (strcmp(posptr, poslen) != 0))	/* check if the first character in poslen is a number */
	  	{
	  		/* check if ptr starts with a , */
	  		if (strcmp(posptr, "") == 0) /* if posptr is empty, begin file parsing */
	  			;
	  		else if	(posptr[0] != ',') /* if posptr doesn't begin with a comma, return an error */
	  		{
	  			fprintf(stderr, "Invalid -POS[,LEN]\n");
	  			return 0;
	  		}
	  		else /* else, rescan the rest of -POS[,LEN] for another integer */
	  		{
	  			poslen = posptr+1;	/* skip the comma in poslen */

	  			/* check that the length of [,LEN] is greater than 1 */
	  			if (!(strlen(poslen)>0))
	  			{
	  				fprintf(stderr, "Invalid -POS[,LEN]\n");
	  				return 0;
	  			}

	  			char *lenptr;

	  			len = strtol(poslen, &lenptr, 10);

	  			/* check if len is a valid integer */
	  			if ((len >= 0) && (strcmp(lenptr, poslen) != 0))
	  			{
	  				if (strcmp(lenptr, "") != 0) /* if lenptr is empty, return an error */
	  				{
		  				fprintf(stderr, "Invalid -POS[,LEN]\n");
		  				return 0;
	  				}
		  			/* else, begin parsing files */
		  			// printf("Valid input, where pos = %li and len = %li\n", pos, len);
		  		}
		  		else
		  		{
		  			fprintf(stderr, "Invalid -POS[,LEN]\n");
		  			return 0;
		  		}
	  		}
	  	}
	  	else
	  	{
	  		fprintf(stderr, "Invalid -POS[,LEN]\n");
	  		return 0;
	  	}
	   	count++;
	}
	
	FILE * fp;
	char * line = NULL;
	size_t length = 0;

	if (len == -1) /* If LEN is not specified, then its default value is INT_MAX, the largest integer representable with an int, which is defined in <limits.h>. */
		len = INT_MAX;

	Deque queue = NULL;
	Deque stack = NULL;
	if(!createD(&queue) || !createD(&stack))
	{
		destroyD(&queue);
		destroyD(&stack);
		fprintf(stderr, "Malloc() failed.\n");
		return 0;
	}

	/* variable to keep track of the number of elements */
	int size = 0;
	/* treat the rest of the arguments as files */
	while (count < argc)
	{
		fp = fopen(argv[count], "r");
		if (fp == NULL) 
		{
			destroyD(&queue);
			destroyD(&stack);
			fprintf(stderr, "file does not exist\n");
			return 0;
		}
		// use getLine() to fetch value of next file value
		else 
		{
			while (getline(&line, &length, fp) != -1)
			{
				/* extract the substring key from each line */
				/* strip off the trailing newline, if there is one */
				int newlen = strlen(line);
				if (line[newlen-1] == '\n') /* if the line ends with a newline, strip it off */
				{
					newlen--;
					line = memmove(line, line, newlen);
					line[newlen] = NULLBYTE;
				}
				/* add each line to the Deque */
				addD(&queue, line);
				size++;
				line = NULL;
			}
		}
		count++;
		length = 0;
		free(line);
		fclose(fp);
		fp = NULL;
	}
	/* sort the entire output stream */
	if (size != 0) /* In case the only character in the file is a newline */
		mergeSort(&queue, &stack, pos, len, size, true);
	/* print and free unsorted values */
	char * current;
	while(!isEmptyD(&queue))
	{
		remD(&queue, &current);
		printf("%s\n", current);
		free(current);
	}
	destroyD(&queue);
	destroyD(&stack);
}

long min(long a, long b){
	if (a < b)
		return a;
	else
		return b;
}

void mergeSort(Deque * queue, Deque * stack, int pos, int len, int size, bool left){	/* sort an unsorted Deque of keys */
	/* BASE CASE */
	/* Base case: once the size of the list is 1, return and do nothing */
	if (size == 1)
		return;
	
	char * current; // current value
	/* DIVIDE: Divide the list in half by moving the first (n/2) elements in the queue into the stack */
	for (int index = 1; index <= size-(size/2); index++)
	{
		remD(queue, &current);
		pushD(stack, current);
	}
	/* RECURSION: Recursively mergeSort the remaining elements in the queue (the left half of the list) */
	mergeSort(queue, stack, pos, len, size/2, left);

	/* MERGE: If the size of the list is 2 (base case), DO NOT reverse the list in the queue; instead, simply store and add the stack value to the queue */
	if (size == 2)
	{
		char * qval;
		headD(queue, &qval);

		char * sval;
		remD(stack, &sval);
		addD(queue, sval);

		int keyCompare = extractKey(qval, sval, pos, len);
		if ((keyCompare > 0) || (keyCompare == 0 && left)) /* for the left half, if qval and sval are equal, keep the STACK value at the head */
		{
			remD(queue, &qval);
			addD(queue, qval);
		}
		/* else, for the right half, keep the QUEUE value at the head */
	}
	else
	{
		/* ACCESS RIGHT LIST: If the size of the list is NOT 2, reverse the list in the queue and put in the stack, and store the right half of the list in the queue (2n pushes total) */
			/* Place all the values in the queue in the stack (n/2 pushes)*/
		while(!isEmptyD(queue))
		{
			remD(queue, &current);
			pushD(stack, current);
		}
			/* Place all the values in the stack in the queue (n pushes) */
		for (int index = 1; index <= size; index++)
		{
			remD(stack, &current);
			addD(queue, current);
		}
			/* Place the original reversed values in the stack (n/2 pushes) */
		for (int index = 1; index <= (size/2); index++)
		{
			remD(queue, &current);
			pushD(stack, current);
		}
		/* RECURSION: Recursively mergeSort the remaining elements in the queue (the right half of the list) */
		mergeSort(queue, stack, pos, len, size-(size/2), !left);

		/* MERGE: Compare the Qhead and the Shead; place the smaller value in the tail */
		merge(queue, stack, pos, len, size-(size/2), size/2);
	}
}

void merge(Deque * queue, Deque * stack, int pos, int len, int qsize, int ssize) /* Merges the sorted left half of the list in the queue and the sorted right half of the list in the stack */
{
	char * qval;
	char * sval;

	headD(queue, &qval);
	headD(stack, &sval);

	while (qsize > 0 && ssize > 0) /* Compare values of the queue and stack and put the smaller value at the tail of merged */
	{
		if (extractKey(qval, sval, pos, len) <= 0) /* if qval < sval, or are equal, add the queue value. */
		{
			remD(queue, &qval);
			addD(queue, qval);
			headD(queue, &qval);
			qsize--;
		}
		else
		{
			remD(stack, &sval);
			addD(queue, sval);
			headD(stack, &sval);
			ssize--;
		}
	}
	/* Add any remaining values into the Deque */
	while (qsize > 0)
	{
		remD(queue, &qval);
		addD(queue, qval);
		headD(queue, &qval);
		qsize--;
	}
	while (ssize > 0)
	{
		remD(stack, &sval);
		addD(queue, sval);
		headD(stack, &sval);
		ssize--;
	}
}

int extractKey(char * qval, char * sval, int pos, int len) /* extracts the keys from leftval and rightval and returns the value when they are compared */
{
	/* adding 1 to the key sizes makes sure comparing empty strings works */
	char qkey[strlen(qval)+1];
	char skey[strlen(sval)+1];

	/* slice the substring using the values of -POS[,LEN], if available */
	/* If val[POS] lies beyond the end of the line, the key is the empty string.*/

	/* extract the value of the left key */
	if (pos != -1)	/* If a POS flag was entered */
	{
		if (pos >= strlen(qval) || len == 0)
			strcpy(qkey, "");
		else
		{
			/* If there are fewer than LEN characters starting with val[POS], the key consists of only those characters (i.e. the rest of the string)
			   Or if there aren't fewer than LEN characters, copy LEN characters
			   Whichever comes first */
			int newlen = min(len, strlen(qval)-pos);
			memmove(qkey, qval+pos, newlen);
			qkey[newlen] = NULLBYTE;
		}
	} /* else, leave val unchanged */
	else
		strcpy(qkey, qval);

	/* extract the value of the right key */
	if (pos != -1)	/* If a POS flag was entered */
	{
		if (pos >= strlen(sval) || len == 0)
			strcpy(skey, "");
		else
		{
			/* If there are fewer than LEN characters starting with val[POS], the key consists of only those characters (i.e. the rest of the string)
			   Or if there aren't fewer than LEN characters, copy LEN characters
			   Whichever comes first */
			int newlen = min(len, strlen(sval)-pos);
			memmove(skey, sval+pos, newlen);
			skey[newlen] = NULLBYTE;
		}
	} /* else, leave val unchanged */
	else
		strcpy(skey, sval);

	return strcmp(qkey, skey);
}

/* Edge cases to fix: (looked through the newsgroup)
- ./Merge19 -0,0 testfile (doesn't make last sort) (change true and false to left and !left)
- What if I pass in NULL deques to each Deque function? I need to add an extra conditional to each one.
- 
*/









