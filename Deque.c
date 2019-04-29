#include <stdlib.h> // for malloc

#include "/c/cs223/Hwk4/Deque.h"

// Deque.h                                      Stan Eisenstat (03/03/19)
//
// Define the abstract data type for a Deque of string pointers.  The strings
// themselves are NOT stored.
//
//   (*) A deque (or double-ended queue, pronounced "deck") is a data structure
//   that combines the attributes of a queue and a stack:  items can be added
//   at the tail and the head but can be removed only from the head.
      

// Define true and false

#include <stdbool.h>


// A variable of type Deque is a pointer to the struct used to implement the
// deque operations declared below.  Note that the declaration of the fields of
// that struct appears only in the file that implements these operations.  Thus
// the calling program does not know what fields have been defined and cannot
// access them; and the variable must be a pointer since the size of the struct
// is unknown.

struct deque {
    struct deque *next;
    char * value;
};


// The following functions are the only means of accessing a Deque data
// structure.  Each requires a pointer to a Deque variable in case it needs
// to modify the value of that variable (or more generally for consistency).
// Each returns the status of the operation, either true (= success) or false
// (= failure; e.g., an invalid argument, an inconsistent Deque object, or a
// NULL return from malloc() / realloc()).


// Set *D to a new object of type Deque.
// Returns true if successful, false otherwise (e.g., malloc() fails).
bool createD (Deque *d)
{
    /* malloc space for D and test if it was successful */
    if (((*d) = malloc(sizeof(struct deque))) == NULL)
        return false;

    // else, initialize Deque
    (*d)->next = *d;

    // return true;
    return true;
}

// Return true if the Deque *D is empty, false otherwise.  The value of *D may
//   change as a result.
bool isEmptyD (Deque *d)
{
    return ((*d) != NULL) && ((*d)->next == (*d));
}

// Add the string pointer S to the tail of the Deque *D; the string itself is
//   not duplicated.  The value of *D may change as a result.
// Return true if successful, false otherwise (e.g., malloc() fails).
bool addD (Deque *d, char *s)
{
    Deque e;  /* new element */

    /* malloc space for e and test if it was successful */
    if (((e = malloc(sizeof(struct deque))) == NULL) || ((*d) == NULL))
        return false;

    /* if D is empty, treat e as adding a head */
    if (isEmptyD(d))
    {
        /* have the new pushed value point to itself */
        e->next = e;
        e->value = s;

        (*d)->next = e;
    }
    else /* if D is not empty */
    {
        /* add e to the head */
        e->next = (*d)->next->next;
        e->value = s;

        (*d)->next->next = e;

        /* point (*d)->next to new head of the Deque to make e the tail */
        (*d)->next = (*d)->next->next;
    }
    return true;
}

// Remove the string pointer at the head of the Deque *D and store that value
//   (or NULL if the Deque is empty) in *S.  The value of *D may change as a
//   result.
// Return true if successful, false otherwise (e.g., *D is empty).
bool remD (Deque *d, char **s)
{
    if((*d) == NULL || isEmptyD(d))
    {
        *s = NULL;
        return false;
    }

    /* if there is only one element in the Deque */
    if ((*d)->next->next == (*d)->next)
    {
        /* store the value at the head of the Deque *D in *s */
        *s = (*d)->next->value;

        /* Change the value of *D as a result */
        Deque temp = (*d)->next;
        (*d)->next = (*d);
        free(temp);
    }
    else /* if there is more than one element in the Deque */
    {
        /* store the value at the head of the Deque *D in *s */
        *s = (*d)->next->next->value;

        /* Change the value of *D as a result */
        Deque temp = (*d)->next->next;
        (*d)->next->next = temp->next;
        free(temp);
    }
    return true;
}


// Store in *S the value of the string pointer at the head of the Deque *D (or
//   NULL if the Deque is empty).  The value of *D may change as a result.
// Return true if successful, false otherwise (e.g., *D is empty).
bool headD (Deque *d, char **s)
{
    if((*d) == NULL || isEmptyD(d))
    {
        *s = NULL;
        return false;
    }
    /* if there is only one element in the Deque */
    if ((*d)->next->next == (*d))
        *s = (*d)->next->value;
    else /* if there is more than one element in the Deque */
        *s = (*d)->next->next->value;

    return true;
}


// Add the string pointer S to the head of the Deque *D; the string itself is
//   not duplicated.  The value of *D may change as a result.
// Return true if successful, false otherwise (e.g., malloc() fails).
bool pushD (Deque *d, char *s)
{
    Deque e;  /* new element */

    /* malloc space for e and test if it was successful */
    if (((e = malloc(sizeof(struct deque))) == NULL) || ((*d) == NULL))
        return false;

    /* if D is empty */
    if (isEmptyD(d))
    {
        /* have the new pushed value point to itself */
        e->next = e;
        e->value = s;

        (*d)->next = e;
    }
    else /* if D is not empty */
    {
        e->next = (*d)->next->next;
        e->value = s;

        (*d)->next->next = e;
    }
    return true;
}


// An alternate name for remD() when the Deque is used as a stack.
#define popD(d,s)  remD(d,s)


// An alternate name for headD() when the Deque is used as a stack.
#define topD(d,s)  headD(d,s)


// Destroy the Deque *D by freeing any storage that it uses (but not the blocks
//   of storage to which the string pointers point) and set the value of *D to
//   NULL.
// Return true if successful, false otherwise (e.g., D is an invalid argument).
bool destroyD (Deque *d)
{
    if ((*d) == NULL) /* return false if D is an invalid argument */
        return false;

    /* dummy string variable so that I can use my remD function */
    while(!isEmptyD(d)) {
        /* if there is only one element in the Deque */
        if ((*d)->next->next == (*d)->next)
        {
            /* Change the value of *D as a result */
            Deque temp = (*d)->next;
            (*d)->next = (*d);
            free(temp);
        }
        else /* if there is more than one element in the Deque */
        {
            /* Change the value of *D as a result */
            Deque temp = (*d)->next->next;
            (*d)->next->next = temp->next;
            free(temp);
        }
    }
    free(*d);
    (*d) = NULL;
    return true;
}


