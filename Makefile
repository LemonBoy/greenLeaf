BASE = .
COREOBJECTS = $(BASE)/src/binary.o $(BASE)/src/dasm.o $(BASE)/src/emulator.o $(BASE)/src/instructions.o $(BASE)/src/memory.o
MACHOBJECTS = $(BASE)/machdep/src/mach_uart.o $(BASE)/machdep/src/mach_main.o $(BASE)/machdep/src/mach_memory.o $(BASE)/machdep/src/mach_emulator.o
OBJECTS = $(COREOBJECTS) $(MACHOBJECTS)
OUTPUT = greenLeaf
# DEFINES = -DDEBUG -DHYPERDEBUG
DEFINES = -DDEBUG
# DEFINES = 
INCLUDES = -I$(BASE)/include -I$(BASE)/machdep/include
CFLAGS = -g -std=c99 -Wall -pedantic -O2 $(INCLUDES)
LDFLAGS =

all: $(OUTPUT)
%.o: %.c
	gcc $(CFLAGS) $(DEFINES) -c -o $@ $<
$(OUTPUT): $(OBJECTS)
	gcc $(LDFLAGS) -o $(OUTPUT) $(OBJECTS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
run:
	./$(OUTPUT)
