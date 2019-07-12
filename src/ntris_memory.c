#include <stdlib.h>
#include "ntris.h"


/*
 * void freeNomino (nomino **)
 *
 * Free all memory associated with the nomino structure passed in, and
 * update its pointer to NULL.
 *
 */
void freeNomino (nomino ** nullmino) {
    int ii;

    for (ii = 0; ii < (*nullmino)->size; ii++) {
        free ((*nullmino)->blocks[ii]);
    }
    free ((*nullmino)->blocks);
    free ((*nullmino));
    *nullmino = NULL;
}

/*
 * void freeNominoList (nomino * nullmino)
 *
 * recursively free all nomino structures starting at the linked list provided
 *
 */
void freeNominoList (nomino * nullmino) {
    if (nullmino == NULL)
        return;
    freeNominoList (nullmino->next);
    freeNomino (&nullmino);
}

/*
 * nomino * duplicateGrow (nomino *)
 *
 * Allocates memory for a clone of the nomino given in the parameter,
 * increments the size field by 1, and allocates memory for one additional
 * block structure. The old blocks are copied, and the new block is set up
 * with positions at x=255, y=255 with a bitmap of 0x00. The rotation is
 * copied, and the next field is set to NULL.
 *
 * Returns: larger, duplicated nomino on success, NULL on error
 *
 */
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

/*
 * nomino * duplicate (nomino *)
 *
 * Returns a copy of the nomino passed in with a similar size. Returns a base
 * monomino if given NULL. Next field is always NULL'ed
 *
 * Returns: duplicated nomino
 *
 */
nomino * duplicate (nomino * parentNomino) {
    nomino * childNomino;
    int ii;

    // allow creating a root monomino if given null
    if (parentNomino == NULL) {
        childNomino = malloc (sizeof (nomino));
        childNomino->blocks = malloc (sizeof (block *));
        childNomino->blocks[0] = malloc (sizeof (block));
        childNomino->blocks[0]->x = 0;
        childNomino->blocks[0]->y = 0;
        childNomino->blocks[0]->bmap = 0;
        childNomino->size = 1;
        childNomino->rot = 0;
        childNomino->next = NULL;

        return childNomino;
    }

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

