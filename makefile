CC = gcc
FLAGS = `pkg-config --cflags --libs glib-2.0`

all: main

dfs.o: dfs.c dfs.h tree.o
	$(CC) -g -o dfs.o -c dfs.c $(FLAGS) 

tree.o: tree.c tree.h
	$(CC) -g -o tree.o -c tree.c $(FLAGS) 

main: main.c tree.o dfs.o
	$(CC) $(FLAGS) -g $< tree.o dfs.o -o $@ -lglib-2.0

clean:
	rm -f *.o 

