#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include "ntris.h"

int numShapes;

int main (int argc, char * argv[]) {
    int opt = 0;
    int size = 0;
    nomino * noms = NULL;
    nomino * newNoms = NULL;
    clock_t clkStart;
    clock_t clkEnd;
    float diffTime;
    char outFile[MAXFNAME] = "";
    char loadFile[MAXFNAME] = "";
    char saveFile[MAXFNAME] = "";
    FILE * fd = NULL;
    int quiet = 0;

    // Check arguments
    while ((opt = getopt (argc, argv, ":o:s:l:q")) != -1) {
        switch (opt) {
            case 'o':
                // set output file
                strcpy (outFile, optarg);
                break;

            case 's':
                // set save file
                strcpy (saveFile, optarg);
                break;

            case 'l':
                // set load file
                strcpy (loadFile, optarg);
                break;

            case 'q':
                // quiet
                quiet = 1;
                break;

            default:
                // unknown option, print usage and exit
                printf (USAGE);
                return ERROR;
                break;
        }
    }

    // get size from first unhandled arg
    if (optind < argc) {
        size = strtoul (argv[optind], NULL, 10);
        // if more than 1 unhandled arg, print usage and exit
        if ((optind + 1 ) < argc) {
            printf (USAGE);
            return ERROR;
        }
    }

    // if size conversion failed, was not entered, or < 1: error
    if (size < 1) {
        printf (USAGE);
        return ERROR;
    }

    // attempt to load from file if applicable
    if (strcmp (loadFile, "") != 0) {
        if ((fd = fopen (loadFile, "r")) == NULL) {
            printf (FAILOPEN, loadFile);
            return ERROR;
        }
        noms = loadNominos (fd);
        fclose (fd);
        fd = NULL;
        if (noms == NULL) {
            printf (FAILLOAD);
            return ERROR;
        }
    }

    // start timer
    clkStart = clock();

    // set up root nomino
    if (noms == NULL) {
        noms = malloc (sizeof (nomino));
        noms->size = 1;
        noms->rot = 0;
        noms->blocks = malloc (sizeof (block *));
        noms->blocks[0] = malloc (sizeof (block));
        noms->blocks[0]->x = 0;
        noms->blocks[0]->y = 0;
        noms->blocks[0]->bmap = 0x00;
        noms->next = NULL;
    }

    // begin
    while (noms->size < size) {
        numShapes = 0;
        newNoms = genNominos (noms);
        noms = newNoms;
        printSts (noms->size);
        printf ("\n");
    }

    clkEnd = clock();
    diffTime = (float) (clkEnd - clkStart) / CLOCKS_PER_SEC;
    
    // open file for printing
    if (!quiet) {
        if (strcmp (outFile, "") != 0) {
            fd = fopen (outFile, "w+");
            if (fd == NULL) {
                printf (FAILOPEN, outFile); // not fatal, just print to stdout
                fd = stdout;
            }
        } else {
            fd = stdout;
        }

        // do printing
        printNominos (noms, fd);

        if (fd != NULL && fd != stdout) {
            fclose (fd);
            fd = NULL;
        }
    }

    // open file for saving
    if (strcmp (saveFile, "") != 0) {
        fd = fopen (saveFile, "w+");
        if (fd == NULL) {
            printf (FAILOPEN, outFile); // not fatal, just print to stdout
            fd = stdout;
        } 
        saveNominos (noms, fd);
        if (fd != stdout)
            fclose (fd);
    }

    printf ("Generated %d unique %sominos in:\n"
            "    %.2f seconds (%.0f / sec)\n",
                numShapes,
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
                size == 11 ? "undec" :
                size == 12 ? "dodec" :
                "poly",
                diffTime,
               (float)numShapes / diffTime);

    return OK;
}

/*
 * nomino * genNominos (nomino *)
 *
 * Generates a linked list of nominos one size larger than the one passed in
 * as parameter. Uses this parameter list as the root shapes, and frees all
 * memory from the input list as it traverses
 *
 * Returns: head of linked list of larger set of unique shapes
 *
 */
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
                    numShapes++;
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
                    numShapes++;
                    if ((numShapes % 100) == 0) {
                        printSts (size + 1);
                        fflush (stdout);
                    }
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


