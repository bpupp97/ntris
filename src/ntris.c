#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include "ntris.h"

int numShapes;
int estimates[] = {0, 1, 1, 2, 7, 18, 60, 196, 704, 2500, 9189, 33896, 126759};

int main (int argc, char * argv[]) {
    int size = 0;
    int rootPos = 0;
    int numRoots = 0;
    int numChildren = 0;
    nomino * roots = NULL;
    nomino * lastRoot = NULL;
    nomino * currChild = NULL;
    nomino ** children = NULL;

    int opt = 0;
    int quiet = 0;
    clock_t clkStart;
    clock_t clkEnd;
    float diffTime;
    char outFile[MAXFNAME] = "";
    char loadFile[MAXFNAME] = "";
    char saveFile[MAXFNAME] = "";
    FILE * fd = NULL;

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
        roots = loadNominos (fd, &numRoots);
        fclose (fd);
        fd = NULL;
        if (roots == NULL) {
            printf (FAILLOAD);
            return ERROR;
        }
    }

    // start timer
    clkStart = clock();

    // set up root nomino
    if (roots == NULL) {
        roots = duplicate (NULL);
        numRoots = 1;
    }

    // begin generation
    while (roots->size < size) {
        // reset shape counter
        numShapes = 0;
        
        // init collection pointers
        children = malloc (sizeof (nomino *) * numRoots); 

        // populate children collections
        lastRoot = roots;
        rootPos = 0;
        while (lastRoot != NULL) {
            children[rootPos] = genNominos (lastRoot);
            removeSubRoots (&(children[rootPos]), roots, lastRoot);
            rootPos++;
            lastRoot = lastRoot->next;
        }

        // concat the children
        currChild = children[0];
        numChildren = 1;
        rootPos = 0;
        while (currChild != NULL) {
            if (currChild->next == NULL) { // end of single roots children
                rootPos++;
                if (rootPos >= numRoots) // end of roots
                    break;

                currChild->next = children[rootPos];
            }
            currChild = currChild->next;
            numChildren++;
        }

        // cleanup, setup roots for next iteration
        freeNominoList (roots);
        roots = *children;
        numRoots = numChildren;
        free (children);
        children = NULL;
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
        printNominos (roots, fd);

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
        saveNominos (roots, fd);
        if (fd != stdout)
            fclose (fd);
    }

    // count
    while (roots != NULL) {
        numShapes++;
        roots = roots->next;
    }

    // print stats
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
 * Takes a root shape and generates all unique shapes as children of root
 *
 * Returns: collection of children shapes from root
 *
 */
nomino * genNominos (nomino * root) {
    int size = -1;
    int ii = 0;
    int jj = 0;
    int kk = 0;
    int dup = 0;
    int status = OK;
    nomino * collect = NULL;
    nomino * collectEnd = NULL;
    nomino * comp = NULL; // pointer to compare for duplicates
    nomino * buff = NULL; // nomino being worked on / checked
    
    // dont allow null roots
    if (root == NULL)
        return NULL;

    size = root->size;
    buff = duplicateGrow (root);

    // for each root block
    for (ii = 0; ii < size; ii++) {
        // for each direction
        for (jj = 0; jj < 4; jj++) {
            // if bmap has a 1 at this direction, skip
            if ((buff->blocks[ii]->bmap & (0x01 << jj)) != 0)
                continue;

            // add block in direction of jj
            switch (jj) {
                case 0: // north
                    status = addBlock (buff,
                                buff->blocks[ii]->x,
                                buff->blocks[ii]->y - 1);
                    break;

                case 1: // east
                    status = addBlock (buff,
                                buff->blocks[ii]->x + 1,
                                buff->blocks[ii]->y);
                    break;

                case 2: // south
                    status = addBlock (buff,
                                buff->blocks[ii]->x,
                                buff->blocks[ii]->y + 1);
                    break;

                case 3: // west
                    status = addBlock (buff,
                                buff->blocks[ii]->x - 1,
                                buff->blocks[ii]->y);
                    break;

                default:
                    status = ERROR;
                    break;
            }
            
            if (status == ERROR)
                continue;

            // if list is empty, obviously not a duplicate, add now
            if (collect == NULL) {
                normalize (buff);
                collect = duplicate (buff);

                // update bitmap
                collect->blocks[ii]->bmap |= (0x01 << (jj % 4));
                collect->blocks[size]->bmap = 0x01 << ((jj + 2) % 4);

                collectEnd = collect;
                continue;
            }

            // loop through rotations
            dup = 0;
            for (kk = 0; kk < 4; kk++) {
                rotate (buff, 1);
                
                // normalize coords
                normalize (buff);
               
                // check if unique
                comp = collect;
                while (comp != NULL) {
                    if (compare (buff, comp) == OK) {
                        dup = 1;
                        break;
                    }
                    comp = comp->next;
                }
                
                if (dup)
                    break;
            }

            rotate (buff, 4 - buff->rot);

            normalize(buff);

            // Add if not a duplicate 
            if (!dup) {
                collectEnd->next = duplicate (buff);
                collectEnd = collectEnd->next;

                // update block bitmaps
                collectEnd->blocks[ii]->bmap |= (0x01 << (jj % 4));
                collectEnd->blocks[size]->bmap = 0x01 << ((jj + 2) % 4);
            }
        }
    }
    freeNomino (&buff);

    return collect;
}


