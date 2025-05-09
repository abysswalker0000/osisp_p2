CC = g++
CFLAGS = -Wall -O2 -std=c++11
PROGRAM = self_recovering_process

default: $(PROGRAM)

$(PROGRAM): lab4.cpp
	$(CC) $(CFLAGS) -o $(PROGRAM) lab4.cpp

clear:
	rm -f $(PROGRAM) counter.txt

.PHONY: default clear