ntris: ntris.c ntris.h
	gcc -o ntris ntris.c -Wall -ggdb

.PHONY: clean

clean:
	rm -rf ntris ntris.out
