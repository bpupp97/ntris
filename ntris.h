#ifndef INC_NTRIS_H
#define INC_NTRIS_H

#include <wchar.h>
#include <stdio.h>

typedef unsigned char UINT8;
typedef signed char INT8;

struct _block {
    INT8 x;
    INT8 y;
    INT8 bmap;  // bit map of adjacent block directions
};

typedef struct _block block;

struct _nomino {
    INT8 size;
    INT8 rot;
    block ** blocks;    // 2d array, length <size>, each sub array length 2 as coords
    struct _nomino * next;
};

typedef struct _nomino nomino;

#define OK      0
#define ERROR   -1

#define MAXPRINTWIDTH   80
#define PRINTSPACING    4

extern wint_t nomchars[];

// I/O functions
void saveNominos (nomino * nominout, FILE * fd);
nomino * loadNominos (FILE * fd);
void printNominos (nomino * nominout, FILE * fd);

// memory management functions
void freeNomino (nomino ** nullmino);
nomino * duplicateGrow (nomino * parentNomino);
nomino * duplicate (nomino * parentNomino);

// geometric functions
int addBlock (nomino * frankenomino, int xPos, int yPos);
void normalize (nomino * nominormal);
void rotate (nomino * nomspino);
int compare (nomino * nominone, nomino * nomintwo);

// 'main' function
nomino * genNominos (nomino * headParent);

#endif // INC_NTRIS_H
