COREOBJECTS = src/binary.o src/dasm.o src/emulator.o src/instructions.o src/memory.o
MACHOBJECTS = machdep/src/mach_uart.o machdep/src/mach_main.o machdep/src/mach_memory.o machdep/src/mach_emulator.o
OBJECTS = $(COREOBJECTS) $(MACHOBJECTS)
OUTPUT = greenLeaf
# DEFINES = -DDEBUG -DHYPERDEBUG
DEFINES = -DDEBUG
# DEFINES = 
INCLUDES = -Iinclude -Imachdep/include
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
