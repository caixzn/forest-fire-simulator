# Compilation flags
CFLAGS = -Wall -g -pthread
CC = gcc

# Create directories to red the files
SRCDIR = src
OBJDIR = obj
BINDIR = bin
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

# Avoid files with the same name
.PHONY: all clean

# Execute compilation
all : binfolder objfolder $(BINDIR)/main

# Build main file
$(BINDIR)/main: $(OBJ)
	@ echo "Compiling the object files into the executable"
	$(CC) $(CFLAGS) $^ -o $@

# Build binary folder
binfolder:
	@ echo "Creating binary folder"
	mkdir -p $(BINDIR)

# Build object folder
objfolder:
	@ echo "Creating object folder"
	mkdir -p $(OBJDIR)

# Compile files
$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean files created
clean:
	@ echo "Removing directories and files"
	rm bin/* obj/* 
	rmdir bin obj

