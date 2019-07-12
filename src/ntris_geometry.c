#include "ntris.h"

/* 
 * int addBlock (nomino *, int, int)
 *
 * Adds a block to the nomino passed in, and checks if a block already
 * exists at that position. No block is added if one already exists.
 *
 * Returns: OK on no match, ERROR if block already exists
 */
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

/*
 * void normalize (nomino *)
 *
 * Normalizes all of the block coordinates so that they are the minimum
 * integers >=0 and retain the same shape
 *
 */
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

/* 
 * void rotate (nomino *)
 *
 * Rotates the given nomino's blocks by 90 degrees clockwise * amount, and updates the
 * rotation attribute
 *
 */
void rotate (nomino * nomspino, int amount) {
    int buffer = 0;
    int ii;

    if (amount == 1) {
        // rot90:  x = -y, y = x
        for (ii = 0; ii < nomspino->size; ii++) {
            buffer = nomspino->blocks[ii]->x;
            nomspino->blocks[ii]->x = nomspino->blocks[ii]->y * -1;
            nomspino->blocks[ii]->y = buffer;
        }
    } else if (amount == 2) {
        // rot180: x = -x, y = -y
        for (ii = 0; ii < nomspino->size; ii++) {
            nomspino->blocks[ii]->x *= -1;
            nomspino->blocks[ii]->y *= -1;
        }
    } else if (amount == 3) {
        // rot270: x = y, y = -x
        for (ii = 0; ii < nomspino->size; ii++) {
            buffer = nomspino->blocks[ii]->x;
            nomspino->blocks[ii]->x = nomspino->blocks[ii]->y;
            nomspino->blocks[ii]->y = buffer * -1;
        }
    } else {
        return;
    }
    nomspino->rot = (nomspino->rot + amount) % 4;
}

/*
 * int compare (nomino *, nomino *)
 *
 * Compares the blocks in the two nomino structures to check for equality,
 * unequal sizes are allowed so long as the larger nomino is the first
 * parameter
 *
 * Returns: OK if equal, ERROR if not
 *
 */
int compare (nomino * nominone, nomino * nomintwo) {
    int ii;
    int jj;
    int found;

    if (nominone->size < nomintwo->size)
        return ERROR;

    found = 0;
    for (ii = 0; ii < nomintwo->size; ii++) {
        for (jj = 0; jj < nomintwo->size; jj++) {
            if (nominone->blocks[ii]->x == nomintwo->blocks[jj]->x &&
                nominone->blocks[ii]->y == nomintwo->blocks[jj]->y) {
                found++;
                
                break;
            }
        }
    }

    if (found == nomintwo->size)
        return OK;
    
    return ERROR;
}

/*
 * void removeSubRoots (nomino ** collection, nomino * rootsStart, nomino * rootsEnd)
 *
 * Modifies the collection to remove any nominos containing the roots between
 * rStart and rEnd, only left inclusive
 *
 * RETURNS: none
 */
 
void removeSubRoots (nomino ** collection, nomino * rStart, nomino * rEnd) {
    nomino * rCurr = NULL;
    nomino * collectCurr = *collection;
    nomino * collectPrev = NULL;
    int found = -1; // using -1 for not found, OK (0) for found

    while (collectCurr != NULL) {   // for each item in collection
        rCurr = rStart;
        found = -1;
        while (rCurr != NULL && rCurr != rEnd) { // for each root in range
            if ((found = compare (collectCurr, rCurr)) == OK) { 
                // sub-root found, remove
                if (collectPrev != NULL) {
                    // middle of list, can just do unlink
                    collectPrev->next = collectCurr->next;
                    freeNomino (&collectCurr);
                    collectCurr = collectPrev->next;
                } else {
                    // start of list, need to also update pointer given
                    *collection = (*collection)->next;
                    freeNomino (&collectCurr);
                    collectCurr = *collection;
                }
                break;
            }

            rCurr = rCurr->next;
        }
        
        if (found == -1) {
            // not found, ignore and step through list
            collectPrev = collectCurr;
            collectCurr = collectCurr->next;
        }
    }
}
