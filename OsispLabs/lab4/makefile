CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJS = main.o config.o

daemon_logger: $(OBJS)
	$(CC) $(CFLAGS) -o daemon_logger $(OBJS)

main.o: main.c config.h
	$(CC) $(CFLAGS) -c main.c

config.o: config.c config.h
	$(CC) $(CFLAGS) -c config.c

clean:
	rm -f $(OBJS) daemon_logger