CCFLAGS=-Wall -Werror -ggdb
SRCDIR=./src/
SRC=./src/ntris.c \
    ./src/ntris_io.c \
    ./src/ntris_memory.c \
    ./src/ntris_geometry.c

OBJDIR=./bin/
OBJ=$(SRC:.c=.o)
OUTFILE=./ntris

ntris: $(OBJ)
	gcc $(OBJ) $(CCFLAGS) -o $(OUTFILE)

.PHONY: clean

clean:
	rm -rf $(OUTFILE) $(OBJ)
