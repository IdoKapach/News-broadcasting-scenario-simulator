# Default target
all: ex3.out

# Rule to create the target executable
ex3.out: main.o
	g++ -o ex3.out main.o

# Rule to compile main.cpp into main.o
main.o: main.cpp classes.h
	g++ -c main.cpp

# Rule to compile buffer.cpp into buffer.o
buffer.o: buffer.cpp buffer.h
	g++ -c buffer.cpp

# Rule to compile classes.cpp into classes.o
classes.o: classes.cpp classes.h
	g++ -c classes.cpp

# Clean up build files
clean:
	rm -f *.o ex3.out

# Phony targets
.PHONY: all clean