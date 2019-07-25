#include "ntris.h"

extern int numShapes;
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
void normalize (nomino * nominormal) { normalizeOffset (nominormal, 0, 0); }
void normalizeOffset (nomino * nominormal, int baseX, int baseY) {
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
    if (minX == baseX && minY == baseY)
        return;

    // subtract offsets
    for (ii = 0; ii < nominormal->size; ii++) {
        nominormal->blocks[ii]->x -= minX - baseX;
        nominormal->blocks[ii]->y -= minY - baseY;
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
    int allowDif;

    if (nominone->size < nomintwo->size)
        return ERROR;
    
    allowDif = nominone->size - nomintwo->size;

    for (ii = nominone->size - 1; ii >= 0; ii--) {
        found = 0;
        for (jj = 0; jj < nomintwo->size; jj++) {
            if (nominone->blocks[ii]->x == nomintwo->blocks[jj]->x &&
                nominone->blocks[ii]->y == nomintwo->blocks[jj]->y) {
                found = 1;
                break;
            }
        }
        if (!found) {
            // this only works for merge algorithm, since the last block must
            // be contained in the root
            if (ii == nominone->size - 1 || --allowDif < 0 )
                return ERROR;
        }
    }

    return OK;
}

/*
 * int compareSize (nomino *, nomino *)
 *
 * Compares the bounding size of the two nomino structures, the first argument
 * is expected to be one size larger on one side than the 2nd
 *
 * Returns: 0 on match, -1 otherwise
 *
 */
int compareSize (nomino * one, nomino * two) {
    INT8 maxOneX = -1;
    INT8 maxOneY = -1;
    INT8 maxTwoX = -1;
    INT8 maxTwoY = -1;
    int ii;

    // parse shape one
    for (ii = 0; ii < one->size; ii++) {
        if (one->blocks[ii]->x > maxOneX)
            maxOneX = one->blocks[ii]->x;
        if (one->blocks[ii]->y > maxOneY)
            maxOneY = one->blocks[ii]->y;
    }

    // parse shape two
    for (ii = 0; ii < two->size; ii++) {
        if (two->blocks[ii]->x > maxTwoX)
            maxTwoX = two->blocks[ii]->x;
        if (two->blocks[ii]->y > maxTwoY)
            maxTwoY = two->blocks[ii]->y;
    }

    // Check for needing rotation matches first
    // X=Y, Y=X
    if (maxOneX == maxTwoY && maxOneY == maxTwoX)
        return 0;
    // X=Y+1, Y=X
    if (maxOneX == maxTwoY + 1 && maxOneY == maxTwoX)
        return 0;
    // X=Y, Y=X+1;
    if (maxOneX == maxTwoY && maxOneY == maxTwoX + 1)
        return 0;

    // Check for unrotated matches
    // X=X, Y=Y
    if (maxOneX == maxTwoX && maxOneY == maxTwoY)
        return 0;
    // X=X+1, Y=Y
    if (maxOneX == maxTwoX + 1 && maxOneY == maxTwoY)
        return 0;
    // X=X, Y=Y+1
    if (maxOneX == maxTwoX && maxOneY == maxTwoY + 1)
        return 0;

    return -1;
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
    int rot = 0;
    int offset = 0;

    while (collectCurr != NULL) {   // for each item in collection
        rCurr = rStart;
        found = -1;
        while (rCurr != rEnd) { // for each root in range
            if (compareSize (collectCurr, rCurr) != -1) {
                for (rot = 0; rot < 4; rot++) {
                    // rotate
                    rotate (collectCurr, 1);

                    for (offset = 0; offset < 5; offset++) {
                        // normalize with offsets
                        switch (offset) {
                            case 0:
                                normalizeOffset (collectCurr, 0, 1);
                                break;

                            case 1:
                                normalizeOffset (collectCurr, 1, 0);
                                break;

                            case 2:
                                normalizeOffset (collectCurr, 0, -1);
                                break;

                            case 3:
                                normalizeOffset (collectCurr, -1, 0);
                                break;

                            case 4:
                                // other orders are arbitrary, but 0,0 MUST be last to retain orig shape
                                normalizeOffset (collectCurr, 0, 0);
                                break;

                            default:
                                break;
                        }
                        
                        // check if root exists in current transformation
                        // if x or y < 0, root cannot exist because it is normalized
                        if (collectCurr->blocks[collectCurr->size - 1]->x >= 0 &&
                            collectCurr->blocks[collectCurr->size - 1]->y >= 0 &&
                            (found = compare (collectCurr, rCurr)) == OK) { 
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
                    }
                    if (found == OK)
                        break;
                }
            }
            if (found == OK)
                break;
            rCurr = rCurr->next;
        }
        
        if (found == -1) {
            // not found, ignore and step through list
            collectPrev = collectCurr;
            collectCurr = collectCurr->next;
        }
    }
}
