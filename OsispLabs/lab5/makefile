CC = gcc
CFLAGS = -O2 -pthread
TARGETS = generator sorter

build: $(TARGETS)

generator: generate.c
	$(CC) $(CFLAGS) -o generator generate.c

sorter: sort.c
	$(CC) $(CFLAGS) -o sorter sort.c

remove:
	rm -f $(TARGETS) *.o data.txt sorted.txt sorted2.txt

.PHONY: build remove