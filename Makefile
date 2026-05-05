CC = gcc
CFLAGS = -Wall -Werror -O2
TARGET = jsoncpy

SRCDIR = src
OBJDIR = build
OBJS = $(addprefix $(OBJDIR)/,debug.o arena.o main.o parser.o scanner.o utils.o)
DEPS = $(addprefix $(SRCDIR)/include/,debug.h arena.h parser.h scanner.h utils.h types.h)

all: $(TARGET)

# build executable from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# build object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build jsoncpy

.PHONY: clean all
