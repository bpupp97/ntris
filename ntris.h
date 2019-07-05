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

#define OK          0
#define ERROR       -1

#define OUTFILE     "ntris.out"

#endif // INC_NTRIS_H
