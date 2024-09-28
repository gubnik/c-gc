CC=gcc
CFLAGS=-O2 -Wall -fsanitize=address
CLIBFLAGS=-O2 -Wall
LDFLAGS=gc.o
LDLIBFLAGS=

.PHONY: main %.o

%.o: src/%.c incl/%.h 
	$(CC) $(CLIBFLAGS) -o $@ -c $< $(LDLIBFLAGS)

main: main.c gc.o 
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
