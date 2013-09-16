
/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* The code to read a memory trace and simulate it on the various
   caches. */

#include "global.h"
#include "utils.h"        // utility functions


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* n is a power of two, which one? */
/* 1 -> 0; 2 -> 1; ... */
int which_power(int n)
{
    int i = 0;
    while (n > 1)
        {
            n = n>>1;
            i += 1;
        }
    return(i);
}

/* generate a mask of n low order bits */
/* if we want a mask of two bits, shift a 1 two
   bits over (100), and then subtract one (011). */
unsigned int mask_of(int n)
{
    return ((1 << n) - 1);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

Boolean ishex(int c)
{
    /*switch(c){
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7': case '9':
        return TRUE;
    }
    switch(c){
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
        return TRUE;
    }
    switch(c){
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
        return TRUE;
    }
    return FALSE;*/
    if (('0' <= c) && (c <= '9')) return(TRUE);
    if (('a' <= c) && (c <= 'f')) return(TRUE);
    if (('A' <= c) && (c <= 'F')) return(TRUE);
    return(FALSE);
}

int hexvalue(int c)
{
    /*switch(c){
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7': case '9':
        return (c - '0');
    }
    switch(c){
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
        return (c - 'a' + 10);
    }
    switch(c){
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
        return (c - 'A' + 10);
    }
    return -1;*/
    if (('0' <= c) && (c <= '9')) return(c - '0');
    if (('a' <= c) && (c <= 'f')) return(c - 'a' + 10);
    if (('A' <= c) && (c <= 'F')) return(c - 'A' + 10);
    return(-1);
}

int decvalue(int c)
{

    if (('0' <= c) && (c <= '9')) return(c - '0');
    return(-1);
}


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* MALLOC space for a string and copy it */

String remember_string(const String name)
{
    size_t n;
    String p;

    if (name == NULL) return(NULL);

    /* get memory to remember file name */
    n = strlen(name) + 1;
    p = CAST(String , malloc(n));
    strcpy(p, name);
    return(p);
}

/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

int skip_blanks(FILE *file)
{
    int c;
    while (((c = getc(file)) != EOF) && isspace(c)) /* keep reading */;
    return(c);
}


int skip_line(FILE *file)
{
    int c;
    while (((c = getc(file)) != EOF) && (c != '\n')) /* keep reading */;
    c = skip_blanks(file);
    return(c);
}


