#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>

#include "ntris.h"

int shapes = 0;

// connected thin lines
wint_t nomChars[] = {0x2588,        // 0, no flags
    0x2579, 0x257A, 0x2514, 0x257B, // N, E, NE, S
    0x2502, 0x250C, 0x251C, 0x2578, // NS, SE, NSE, W
    0x2518, 0x2500, 0x2534, 0x2510, // NW, EW, NEW, SW
    0x2524, 0x252C, 0x253C};        // NSW, SEW, NSEW

/*
// all blocks
wint_t nomChars[] = {0x2588,        // 0, no flags
    0x2588, 0x2588, 0x2588, 0x2588, // N, E, NE, S
    0x2588, 0x2588, 0x2588, 0x2588, // NS, SE, NSE, W
    0x2588, 0x2588, 0x2588, 0x2588, // NW, EW, NEW, SW
    0x2588, 0x2588, 0x2588};        // NSW, SEW, NSEW
*/

void saveNominos (nomino * nominout, FILE * fd) {
// save in form <size>/<rotation>/[<x>,<y>,<bmap>],[],[]\n
    int ii;
    shapes = 0;

    while (nominout != NULL) {
        for (ii = 0; ii < nominout->size; ii++) {
            if (ii != 0)
                fprintf (fd, ",[%d,%d,%d]", nominout->blocks[ii]->x,
                        nominout->blocks[ii]->y, nominout->blocks[ii]->bmap);
            else
                fprintf (fd, "{[%d,%d,%d]", nominout->blocks[ii]->x,
                        nominout->blocks[ii]->y, nominout->blocks[ii]->bmap);
        }
        fprintf (fd, "}\n");
        nominout = nominout->next;
        shapes++;
    }
}

// print all nominos starting with the input shape, to file descriptor fd
void printNominos (nomino * nominout, FILE * fd) {
    wint_t ** screen;
    int size;
    int numcols;
    int ii;
    int jj;

    size = nominout->size;
    numcols = MAXPRINTWIDTH / (size + PRINTSPACING);
    shapes = 0;

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
            shapes++;

            nominout = nominout->next;
            if (nominout == NULL)
                break;
        }
    
        // print
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

// free any alloc'ed memory in the nomino
void freeNomino (nomino ** nullmino) {
    int ii;

    for (ii = 0; ii < (*nullmino)->size; ii++) {
        free ((*nullmino)->blocks[ii]);
    }
    free ((*nullmino)->blocks);
    free ((*nullmino));
    *nullmino = NULL;
}

// return a copy of the parent nomino, with size = parent size + 1 and a larger array, next is NULLed
nomino * duplicateGrow (nomino * parentNomino) {
    nomino * childNomino;
    int ii;

    childNomino = malloc (sizeof (nomino));
    childNomino->size = parentNomino->size + 1;
    childNomino->blocks = malloc (sizeof (block *) * childNomino->size);
    childNomino->rot = parentNomino->rot;
    childNomino->next = NULL;

    // copy the parents blocks in
    for (ii = 0; ii < parentNomino->size; ii++) {
        childNomino->blocks[ii] = malloc (sizeof (block));
        childNomino->blocks[ii]->x = parentNomino->blocks[ii]->x;
        childNomino->blocks[ii]->y = parentNomino->blocks[ii]->y;
        childNomino->blocks[ii]->bmap = parentNomino->blocks[ii]->bmap;
    }

    // add the new block
    childNomino->blocks[childNomino->size - 1] = malloc (sizeof (block));
    childNomino->blocks[childNomino->size - 1]->x = 255;
    childNomino->blocks[childNomino->size - 1]->y = 255;
    childNomino->blocks[childNomino->size - 1]->bmap = 0x00;

    return childNomino;
}

// return a copy of the parent nomino, with size = parent size, next is NULLed
nomino * duplicate (nomino * parentNomino) {
    nomino * childNomino;
    int ii;

    childNomino = malloc (sizeof (nomino));
    childNomino->size = parentNomino->size;
    childNomino->blocks = malloc (sizeof (block *) * childNomino->size);
    childNomino->rot = parentNomino->rot;
    childNomino->next = NULL;

    // copy the parents blocks in
    for (ii = 0; ii < parentNomino->size; ii++) {
        childNomino->blocks[ii] = malloc (sizeof (block));
        childNomino->blocks[ii]->x = parentNomino->blocks[ii]->x;
        childNomino->blocks[ii]->y = parentNomino->blocks[ii]->y;
        childNomino->blocks[ii]->bmap = parentNomino->blocks[ii]->bmap;
    }

    return childNomino;
}


// check if the position being added exists, else add it
int addBlock (nomino * frankenomino, int xPos, int yPos) {
    int ii;
    int status = OK;

    for (ii = 0; ii < frankenomino->size; ii++) {
        if (frankenomino->blocks[ii]->x == xPos &&
            frankenomino->blocks[ii]->y == yPos) {
            
            status = ERROR;
            break;
        }
    }

    if (status == OK) {
        frankenomino->blocks[frankenomino->size-1]->x = xPos;
        frankenomino->blocks[frankenomino->size-1]->y = yPos;
    }
    
    return status;
}

// normalize the nomino so all locations are positive and closest to 0
void normalize (nomino * nominormal) {
    int ii = 0;
    int minX = 255;
    int minY = 255;

    // calculate offset
    for (ii = 0; ii < nominormal->size; ii++) {
        if (nominormal->blocks[ii]->x < minX)
            minX = nominormal->blocks[ii]->x;
        if (nominormal->blocks[ii]->y < minY)
            minY = nominormal->blocks[ii]->y;
    }

    // nothing to do if offsets are 0
    if (minX == 0 && minY == 0)
        return;

    // subtract offsets
    for (ii = 0; ii < nominormal->size; ii++) {
        nominormal->blocks[ii]->x -= minX;
        nominormal->blocks[ii]->y -= minY;
    }
}

// rotate the nomino 90 deg clockwise
void rotate (nomino * nomspino) {
    int buffer = 0;
    int ii;
    for (ii = 0; ii < nomspino->size; ii++) {
        buffer = nomspino->blocks[ii]->x;
        nomspino->blocks[ii]->x = nomspino->blocks[ii]->y * -1;
        nomspino->blocks[ii]->y = buffer;
    }
    nomspino->rot++;
}

// compare 2 nominos, returns ERROR if not matching, OK if matching
int compare (nomino * nominone, nomino * nomintwo) {
    int ii;
    int jj;
    int found;

    if (nominone->size != nomintwo->size)
        return ERROR;

    found = 0;
    for (ii = 0; ii < nominone->size; ii++) {
        for (jj = 0; jj < nomintwo->size; jj++) {
            if (nominone->blocks[ii]->x == nomintwo->blocks[jj]->x &&
                nominone->blocks[ii]->y == nomintwo->blocks[jj]->y) {
                found++;
                
                break;
            }
        }
    }

    if (found == nominone->size)
        return OK;
    
    return ERROR;
}

// generate a list of all unique nominos for a size, given a list 1 size smaller of nominos
nomino * genNominos (nomino * headParent) {
    int size = -1;
    int ii = 0;
    int jj = 0;
    int kk = 0;
    int dup = 0;
    int status = OK;
    nomino * headChild = NULL; // head of list to return
    nomino * currChild = NULL; // position in list
    nomino * compChild = NULL; // pointer to compare for duplicates
    nomino * buffChild = NULL; // nomino being worked on / checked
    nomino * lastParent = NULL;// pointer to parent nomino to free

    if (headParent == NULL)
        return NULL;

    size = headParent->size;
    
    // for each nomino
    while (headParent != NULL) {
        buffChild = duplicateGrow (headParent);
        // for each block
        for (ii = 0; ii < size; ii++) {
            // for each direction
            for (jj = 0; jj < 4; jj++) {
                // if bmap has a 1 at this direction, skip
                if ((buffChild->blocks[ii]->bmap & (0x01 << jj)) != 0)
                    continue;

                // add block in direction of jj
                switch (jj) {
                    case 0: // north
                        status = addBlock (buffChild,
                                    buffChild->blocks[ii]->x,
                                    buffChild->blocks[ii]->y - 1);
                        break;

                    case 1: // east
                        status = addBlock (buffChild,
                                    buffChild->blocks[ii]->x + 1,
                                    buffChild->blocks[ii]->y);
                        break;

                    case 2: // south
                        status = addBlock (buffChild,
                                    buffChild->blocks[ii]->x,
                                    buffChild->blocks[ii]->y + 1);
                        break;

                    case 3: // west
                        status = addBlock (buffChild,
                                    buffChild->blocks[ii]->x - 1,
                                    buffChild->blocks[ii]->y);
                        break;

                    default:
                        status = ERROR;
                        break;
                }
                
                if (status == ERROR)
                    continue;

                // if head is NULL, list is empty, so obviously not a duplicate, add now
                if (headChild == NULL) {
                    normalize (buffChild);
                    headChild = duplicate (buffChild);

                    // update bitmap
                    headChild->blocks[ii]->bmap |= (0x01 << (jj % 4));
                    headChild->blocks[size]->bmap = 0x01 << ((jj + 2) % 4);

                    currChild = headChild;
                    continue;
                }

                // loop through rotations
                dup = 0;
                for (kk = 0; kk < 4; kk++) {
                    rotate (buffChild);
                    
                    // normalize coords
                    normalize (buffChild);
                   
                    // check if unique
                    compChild = headChild;
                    while (compChild != NULL) {
                        if (compare (buffChild, compChild) == OK) {
                            dup = 1;
                            break;
                        }
                        compChild = compChild->next;
                    }
                    
                    if (dup)
                        break;
                }

                while (buffChild->rot !=0)
                    rotate (buffChild);

                normalize(buffChild);

                // Add if not a duplicate 
                if (!dup) {
                    currChild->next = duplicate (buffChild);

                    // update block bitmaps
                    currChild->next->blocks[ii]->bmap |= (0x01 << (jj % 4));
                    currChild->next->blocks[size]->bmap = 0x01 << ((jj + 2) % 4);

                    currChild = currChild->next;
                }
            }
        }
        freeNomino (&buffChild);
        lastParent = headParent;
        headParent = headParent->next;
        freeNomino (&lastParent);
    }
    return headChild;
}

int main (int argc, char * argv[]) {
    int size = 0;
    int ii = 0;
    nomino * noms = NULL;
    nomino * newNoms = NULL;
    clock_t clkStart;
    clock_t clkEnd;
    float diffTime;
    FILE * fd;

    // Check arguments were entered properly
    if (argc != 2 || (size = atoi (argv[1])) <= 0) {
        printf ("USAGE: ntris <size>\n");
        return ERROR;
    }
    
    // start timer
    clkStart = clock();

    // set up root nomino
    noms = malloc (sizeof (nomino));
    noms->size = 1;
    noms->rot = 0;
    noms->blocks = malloc (sizeof (block *));
    noms->blocks[0] = malloc (sizeof (block));
    noms->blocks[0]->x = 0;
    noms->blocks[0]->y = 0;
    noms->blocks[0]->bmap = 0x00;
    noms->next = NULL;

    // begin
    for (ii = 1; ii < size; ii++) {
        newNoms = genNominos (noms);
        noms = newNoms;
    }

    clkEnd = clock();
    diffTime = (float) (clkEnd - clkStart) / CLOCKS_PER_SEC;
    
    // open file
    fd = fopen (OUTFILE, "w+");
    if (fd == NULL)
        printf ("Failed to open file for writing!\n");

    // do printing
    setlocale (LC_ALL, "");
    // printNominos (noms, fd);
    saveNominos (noms, fd);

    if (fd != NULL)
        fclose (fd);

    printf ("Generated %d unique %sominos in:\n"
            "    %.2f seconds (%.0f / sec)\n",
                shapes,
                size == 1 ? "mon" :
                size == 2 ? "d" :
                size == 3 ? "tr" :
                size == 4 ? "tetr" :
                size == 5 ? "pent" :
                size == 6 ? "hex" :
                size == 7 ? "hept" :
                size == 8 ? "oct" :
                size == 9 ? "non" :
                size == 10 ? "dec" :
                "poly",
                diffTime,
               (float)shapes / diffTime);

    return OK;
}
