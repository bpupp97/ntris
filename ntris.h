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
    block ** blocks;    // 2d array, length <size>, each sub array length 2 as coords
    struct _nomino * next;
};

typedef struct _nomino nomino;

#define OK 0
#define ERROR -1

#define OUTFILE "ntris.out"

#define N   0x01
#define E   0x02
#define NE  0x03
#define S   0x04
#define NS  0x05
#define SE  0x06
#define NES 0x07
#define W   0x08
#define NW  0x09
#define EW  0x0A
#define NEW 0x0B
#define SW  0x0C
#define NSW 0x0D
#define SEW 0x0E
#define NESW    0x0F

#endif // INC_NTRIS_H
