# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target Executable
TARGET = pwordcount

# Source and Object Files
SRCS = main.c pwordcount_f.c
OBJS = main.o pwordcount_f.o

# Default Rule (Compiles the program)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile main.c
main.o: main.c pwordcount.h
	$(CC) $(CFLAGS) -c main.c

# Compile pwordcount_f.c
pwordcount_f.o: pwordcount_f.c pwordcount.h
	$(CC) $(CFLAGS) -c pwordcount_f.c

# Clean: Removes compiled files
clean:
	rm -f $(TARGET) $(OBJS)
