

CC = gcc
CFLAGS = -I.


main: main.o
	$(CC) -o main main.o $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

clean: rm -f *.o main
