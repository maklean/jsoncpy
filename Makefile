CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
TARGET = jsoncpy

SRCDIR = src
OBJDIR = build
OBJS = $(addprefix $(OBJDIR)/,debug.o main.o parser.o scanner.o utils.o)

all: $(TARGET)

# build executable from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# build object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build jsoncpy

.PHONY: clean all