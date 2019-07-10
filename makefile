CCFLAGS=-Wall -Werror -ggdb
SRC=ntris.c ntris_io.c ntris_memory.c ntris_geometry.c
OBJ=$(SRC:.c=.o)

ntris: $(OBJ)
	gcc $(SRC) $(CCFLAGS) -o ntris.out

.PHONY: clean

clean:
	rm -rf ntris ntris.out *.o
