CFLAGS=-Wall -Werror -ggdb
SRC=./src/ntris.c \
    ./src/ntris_io.c \
    ./src/ntris_memory.c \
    ./src/ntris_geometry.c
OBJ=$(SRC:.c=.o)
OUTFILE=./ntris

ntris: $(OBJ)
	gcc $(OBJ) $(CFLAGS) -o $(OUTFILE)

.PHONY: clean

clean:
	rm -rf $(OUTFILE) $(OBJ)
