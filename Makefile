# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Target executable
TARGET = main

# Source files
SRCS = main.c chunk.c memory.c debug.c value.c line.c vm.c compiler.c scanner.c object.c

# Object files directory
OBJDIR = obj

# Object files
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

# Header files
HDRS = common.h chunk.h memory.h debug.h value.h line.h vm.h compiler.h scanner.h token.h object.h

# Default target
all: $(TARGET)

# Create the object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Link the object files to create the executable
$(TARGET): $(OBJDIR) $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compile source files into object files
$(OBJDIR)/%.o: %.c $(HDRS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Phony targets
.PHONY: all clean
