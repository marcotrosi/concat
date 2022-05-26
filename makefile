.SILENT:
.PHONY: all clean dirs

DIRS=bin

all: clean dirs
	gcc -Wall -std=c89 -o bin/concat src/concat.c

clean:
	rm -rf ${DIRS}

dirs:
	mkdir -p ${DIRS}

