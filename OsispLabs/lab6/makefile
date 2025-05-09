COMPILER := g++
FLAGS := -Wall -Wextra -std=c++17
SOURCES := main.cpp functions.cpp
OBJECTS := $(SOURCES:.cpp=.o)
EXEC := traceroute

build: $(EXEC)

$(EXEC): $(OBJECTS)
	$(COMPILER) $(FLAGS) -o $(EXEC) $(OBJECTS)

%.o: %.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

remove:
	rm -f $(OBJECTS) $(EXEC)

.PHONY: build remove