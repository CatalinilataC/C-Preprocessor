CFLAGS = -Wall -std=c89
CC = gcc

build:
	$(CC) $(CFLAGS) tema1.c -o so-cpp
clean:
	rm -rf tema1.o so-cpp