CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJS = main.o utils.o

reverse_filter: $(OBJS)
	$(CC) $(CFLAGS) -o reverse_filter $(OBJS)

main.o: main.c utils.h
	$(CC) $(CFLAGS) -c main.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(OBJS) reverse_filter