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

#define OK              0
#define ERROR           -1
#define MAXFNAME        50
#define MAXPRINTWIDTH   80
#define PRINTSPACING    4
#define ENDMARK         "X"

#define USAGE           "USAGE: ntris (-options) <size>\n" \
                        "\n" \
                        "    OPTIONS: -o <file>     output shapes to file <file>\n" \
                        "             -l <file>     load shapes from file <file>\n" \
                        "             -s <file>     save shapes to file <file>\n" \
                        "             -q            quiet mode, no output\n" \
                        "\n"
#define FAILOPEN        "Failed to open file: %s\n"
#define FAILLOAD        "Failed to load nominos: Invalid Data\n"

extern wint_t nomchars[];

// I/O functions
void printSts (int size);
void saveNominos (nomino * nominout, FILE * fd);
nomino * loadNominos (FILE * fd, int * numNoms);
void printNominos (nomino * nominout, FILE * fd);

// memory management functions
void freeNomino (nomino ** nullmino);
void freeNominoList (nomino * nullmino);
nomino * duplicateGrow (nomino * parentNomino);
nomino * duplicate (nomino * parentNomino);

// geometric functions
int addBlock (nomino * frankenomino, int xPos, int yPos);
void normalize (nomino * nominormal);
void normalizeOffset (nomino * nominormal, int baseX, int baseY);
void rotate (nomino * nomspino, int amount);
int compare (nomino * nominone, nomino * nomintwo);
void removeSubRoots (nomino ** collection, nomino * rStart, nomino * rEnd);

// 'main' function
nomino * genNominos (nomino * root);

#endif // INC_NTRIS_H
