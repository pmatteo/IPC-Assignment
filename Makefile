# sources
TARGET 	= calcola.x

# config OS
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	CC		= gcc
	CFLAGS	= -std=gnu99 -Wall -c -pthread -ggdb
	LDLIBS	= -pthread
	LD		= gcc
endif
ifeq ($(UNAME), Darwin)
	CC		= clang
	CFLAGS	= -std=c99 -Wall -g -c -pthread
	LD		= clang
endif

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = .

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LD) $(OBJECTS) $(LDLIBS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
