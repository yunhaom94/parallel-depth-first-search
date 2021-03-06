CC = gcc
FLAGS = `pkg-config --cflags --libs glib-2.0` 

all: dfs_test

dfs.o: dfs.c dfs.h tree.o
	$(CC) -g -o dfs.o -c dfs.c $(FLAGS) -pthread

tree.o: tree.c tree.h
	$(CC) -g -o tree.o -c tree.c $(FLAGS) -pthread

dfs_test: dfs_test.c tree.o dfs.o
	$(CC) $(FLAGS) -g $< tree.o dfs.o -o $@ -lglib-2.0 -lm -pthread

clean:
	rm -f *.o dfs_test

