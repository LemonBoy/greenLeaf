COREOBJECTS = src/binary.o src/dasm.o src/emulator.o src/instructions.o src/memory.o
MACHOBJECTS = src/machdep/mach_uart.o src/machdep/mach_main.o src/machdep/mach_memory.o src/machdep/mach_emulator.o
OBJECTS = $(COREOBJECTS) $(MACHOBJECTS)
OUTPUT = greenLeaf
# DEFINES = -DDEBUG -DHYPERDEBUG
DEFINES = -DDEBUG
# DEFINES = 
INCLUDES = -Iinclude
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
