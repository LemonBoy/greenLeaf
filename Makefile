OBJECTS = binary.o dasm.o emulator.o greenLeaf.o instructions.o memory.o uart.o
OUTPUT = greenLeaf

all: $(OUTPUT)
%.o: %.c
	gcc -Wall -c -o $@ $<
$(OUTPUT): $(OBJECTS)
	gcc  -o $(OUTPUT) $(OBJECTS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
