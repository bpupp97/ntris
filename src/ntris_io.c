#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "ntris.h"

/*
// connected lines
wint_t nomChars[] = {0x2588,        // 0, no flags
    0x2579, 0x257A, 0x2514, 0x257B, // N, E, NE, S
    0x2502, 0x250C, 0x251C, 0x2578, // NS, SE, NSE, W
    0x2518, 0x2500, 0x2534, 0x2510, // NW, EW, NEW, SW
    0x2524, 0x252C, 0x253C};        // NSW, SEW, NSEW
*/
// all blocks
wint_t nomChars[] = {0x2588,        // 0, no flags
    0x2588, 0x2588, 0x2588, 0x2588, // N, E, NE, S
    0x2588, 0x2588, 0x2588, 0x2588, // NS, SE, NSE, W
    0x2588, 0x2588, 0x2588, 0x2588, // NW, EW, NEW, SW
    0x2588, 0x2588, 0x2588};        // NSW, SEW, NSEW

extern int estimates[];
extern int numShapes;

/*
 * void printSts (int, int)
 *
 * Prints a status message for amount of nominos that have been generated
 * out of the theoretical maximum
 *
 */
void printSts (int size) {
    printf ("\rGenerating %sominos: %d / %d     ",
                size == 1 ? "mon" :
                size == 2 ? "d" :
                size == 3 ? "tri" :
                size == 4 ? "tetr" :
                size == 5 ? "pent" :
                size == 6 ? "hex" :
                size == 7 ? "hept" :
                size == 8 ? "oct" :
                size == 9 ? "non" :
                size == 10 ? "dec" :
                size == 11 ? "undec" :
                size == 12 ? "dodec" :
                "poly", numShapes, estimates[size]);
}

/*
 * void saveNominos (nomino *, FILE *)
 *
 * Writes the nomino data, given the input list head, to the specified
 * file pointer. File must be opened previous to writing and closed after.
 * Write is in the format:
 * <START OF FILE>
 * $size
 * $x,$y,$bmap;$x,$y,$bmap...
 * ...
 * $ENDMARK
 * <EOF>
 *
 */
void saveNominos (nomino * nominout, FILE * fd) {
    int ii;
    // store header info, just size for now
    fprintf (fd, "%d\n", nominout->size);

    // store blocks
    while (nominout != NULL) {
        for (ii = 0; ii < nominout->size; ii++) {
            if (ii != 0)
                fprintf (fd, ";%d,%d,%d", nominout->blocks[ii]->x,
                        nominout->blocks[ii]->y, nominout->blocks[ii]->bmap);
            else
                fprintf (fd, "%d,%d,%d", nominout->blocks[ii]->x,
                        nominout->blocks[ii]->y, nominout->blocks[ii]->bmap);
        }
        fprintf (fd, "\n");
        nominout = nominout->next;
    }
    fprintf (fd, ENDMARK);
}

/*
 * nomino * loadNominos (FILE *)
 *
 * Parse the file passed in and return a pointer to the head of a linked
 * list of nomino* representing the data
 *
 * Returns: nomino * to head of list, or NULL on error
 *
 */
nomino * loadNominos (FILE * fd, int * numNoms) {
    nomino * head = NULL;
    nomino * curr = NULL;
    nomino * buff = NULL;
    char * linebuf = NULL;
    char * blkbuf = NULL;
    size_t linelen = 0;
    int blkNum = 0;
    int size = 0;

    // process header info
    getline (&linebuf, &linelen, fd);
    size = (int) strtoul (linebuf, NULL, 10);
    linebuf = NULL;


    // process block info
    *numNoms = 0;
    while (getline (&linebuf, &linelen, fd) != -1) {
        if (strcmp (linebuf, ENDMARK) == 0) {
            // End of blocks found, this is a complete set
            break;
        }
        (*numNoms)++;
        buff = malloc (sizeof (nomino));
        buff->blocks = malloc (sizeof (block*) * size);
        
        // allocate and fill blocks
        blkNum = 0;
        while (linebuf != NULL) {
            blkbuf = strsep (&linebuf, ";");
            buff->blocks[blkNum] = malloc (sizeof (block));
            // x pos
            buff->blocks[blkNum]->x = (INT8) strtoul (strsep (&blkbuf, ","), NULL, 10);
            
            // y pos
            buff->blocks[blkNum]->y = (INT8) strtoul (strsep (&blkbuf, ","), NULL, 10);

            // bmap
            buff->blocks[blkNum]->bmap = (INT8) strtoul (strsep (&blkbuf, ","), NULL, 10);
            blkNum++;
        }
       
        // set the rest of the fields
        buff->size = size;
        buff->rot = 0;
        buff->next = NULL;

        // append or create the list
        if (head == NULL) {
            head = buff;
            curr = head;
        } else {
            curr->next = buff;
            curr = buff;
        }

        free (linebuf);
        linebuf = NULL;
        linelen = 0;
    }

    return head;
}

/*
 * void printNominos (nomino*, FILE*)
 *
 * Output the nominos in the given list to the file specified in the parameters.
 * Options for spacing and characters used can be found in ntris.h. Supports
 * unicode.
 *
 */
void printNominos (nomino * nominout, FILE * fd) {
    wint_t ** screen;
    int size;
    int numcols;
    int ii;
    int jj;

    if (nominout == NULL)
        return;

    size = nominout->size;
    numcols = MAXPRINTWIDTH / (size + PRINTSPACING);

    // set up screen
    screen = malloc (sizeof (wint_t*) * size);
    for (ii = 0; ii < size; ii++) {
        screen[ii] = malloc (sizeof(wint_t) * (size + PRINTSPACING) * numcols);
    }

    // loop through list
    while (nominout != NULL) {
        // clear screen
        for (ii = 0; ii < size; ii++) {
            for (jj = 0; jj < (size + PRINTSPACING) * numcols; jj++) {
                screen[ii][jj] = ' ';
            }
        }

        // insert row of shapes
        for (ii = 0; ii < numcols; ii++) {
            for (jj = 0; jj < size; jj++) {
                screen  [nominout->blocks[jj]->y]
                        [nominout->blocks[jj]->x + (ii * (size + PRINTSPACING))] = 
                        nomChars[nominout->blocks[jj]->bmap];
            }

            nominout = nominout->next;
            if (nominout == NULL)
                break;
        }
    
        // print
        setlocale (LC_ALL, "");
        for (ii = 0; ii < size; ii++) {
            for (jj = 0; jj < (size + PRINTSPACING) * numcols; jj++) {
                fprintf (fd, "%lc", screen[ii][jj]);
            }
            fprintf (fd, "\n");
        }
        fprintf (fd, "\n");
    } 

    // free mem
    for (ii = 0; ii < size; ii++) {
        free (screen[ii]);
    }
    free (screen);
}

#ifdef DEBUG
void printNomStruct (nomino * nom) {
    int ii = 0;
    printf ("Struct Nomino @ %p {\n"
            "    size : %d\n"
            "     rot : %d\n"
            "    next : %p\n"
            "  blocks : \n", nom, nom->size, nom->rot, nom->next);
    for (ii = 0; ii < nom->size; ii++) {
        printf ("    {%d, %d, %d}\n", nom->blocks[ii]->x, nom->blocks[ii]->y,
                                    nom->blocks[ii]->bmap);
    }
}
#endif
