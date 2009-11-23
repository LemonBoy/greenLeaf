OBJECTS = src/binary.o src/dasm.o src/emulator.o src/greenLeaf.o src/instructions.o src/memory.o src/uart.o
OUTPUT = greenLeaf
DEFINES = -DDEBUG -DHYPERDEBUG
# DEFINES = -DDEBUG
# DEFINES = 
CFLAGS = -g -std=c99 -Wall -pedantic -O2
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
