# *  Define the name of the makefile.                                        *

MAKNAM = makefile

# *  Define the directories in which to search for library files.            *

LIBDRS =

# *  Define the directories in which to search for include files.            *

INCDRS =

# *  Define the library files.                                               *

LIBFLS =

# *  Define the source files.                                                *

SRCFLS = src/mutator.c \
         src/collector.c \
	     src/heap.c \
         src/bistree.c \
         src/ascii_tree.c \
		 src/list.c

# *  Define the object files.                                                *

OBJFLS = bin/mutator.o \
		 bin/collector.o \
         bin/heap.o \
         bin/bistree.o \
         bin/ascii_tree.o \
	     bin/list.o

# *  Define the executable.                                                  *

EXE    = bin/mutator

# *  Define the compile and link options.                                    *

CC     = gcc
LL     = gcc
CFLAGS = -Wall -g
LFLAGS =

# *  Define the rules.                                                       *
init:
	mkdir -p bin
	

$(EXE): $(OBJFLS)
	$(LL) $(LFLAGS) $(OBJFLS) -o $@ $(LIBDRS) $(LIBFLS)

# $(OBJFLS): bin/%.o : src/%.c
#
bin/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCDRS) -c $< -o $@ 


all:
	make -f $(MAKNAM) clean
	make -f $(MAKNAM) init
	make -f $(MAKNAM) $(EXE)

clean:
	-rm $(EXE)
	-rm $(OBJFLS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

bin/mutator.o: src/bool.h src/globals.h src/list.h src/bistree.h src/bistree.c
bin/heap.o: src/heap.h src/globals.h src/heap.c
bin/bistree.o: src/bool.h src/bistree.h src/ascii_tree.h src/bistree.c
bin/list.o: src/bool.h src/list.h src/list.c
bin/collector.o: src/collector.h src/collector.c src/list.h src/list.c
bin/ascii_tree.o: src/bistree.h src/ascii_tree.h src/ascii_tree.c
