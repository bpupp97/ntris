#ifndef INC_NTRIS_H
#define INC_NTRIS_H

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

#define OUTFILE         "ntris.out"
#define MAXPRINTWIDTH   80
#define PRINTSPACING    4

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
