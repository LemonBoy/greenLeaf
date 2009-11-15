OBJECTS = binary.o dasm.o emulator.o greenLeaf.o instructions.o memory.o uart.o
OUTPUT = greenLeaf
DEFINES = -DDEBUG
CFLAGS = -std=c99 -Wall -pedantic -O2
LDFLAGS = 

all: $(OUTPUT)
%.o: %.c
	gcc $(CFLAGS) $(DEFINES) -c -o $@ $<
$(OUTPUT): $(OBJECTS)
	gcc $(LDFLAGS) -o $(OUTPUT) $(OBJECTS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
