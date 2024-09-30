CC=gcc
CFLAGS=-O2 -Wall -fsanitize=address -pthread
CLIBFLAGS=-O2 -Wall -pthread
LDFLAGS=gc.o -pthread
LDLIBFLAGS=-pthread

.PHONY: main %.o

%.o: src/%.c incl/%.h 
	$(CC) $(CLIBFLAGS) -o $@ -c $< $(LDLIBFLAGS)

main: main.c gc.o 
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
