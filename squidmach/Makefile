BASE = ..
COREOBJECTS = $(BASE)/src/binary.o $(BASE)/src/dasm.o $(BASE)/src/emulator.o $(BASE)/src/instructions.o $(BASE)/src/memory.o
MACHOBJECTS = $(BASE)/squidmach/src/mach_main.o $(BASE)/squidmach/src/mach_hw.o $(BASE)/squidmach/src/mach_memory.o $(BASE)/squidmach/src/mach_emulator.o
OBJECTS = $(COREOBJECTS) $(MACHOBJECTS)
OUTPUT = squidemu
# DEFINES = -DDEBUG -DHYPERDEBUG
DEFINES = -DDEBUG
# DEFINES = 
INCLUDES = -I$(BASE)/include -I$(BASE)/squidmach/include
CFLAGS = -g -std=c99 -Wall -pedantic -O2 $(INCLUDES)
LDFLAGS =
CC = gcc
RM = rm

MIPSAS = mips-as
MIPSLD = mips-ld
MIPSASFLAGS = -mabi=o64 -EL -mips3
MIPSLDFLAGSGENERIC = -nostdlib -EL -mips3 --oformat elf64-little -s
MIPSLDFLAGSBOOT = $(MIPSLDFLAGSGENERIC) --section-start .code=0xBFC00000 --section-start .info=0xBFC10000 -e 0xBFC00000
MIPSLDFLAGS = $(MIPSLDFLAGSGENERIC) --section-start .code=0xC0000000 --section-start .data=0xC0010000 --section-start .bootsupport=0x10000000 -e 0xC0000000

all: $(OUTPUT)
%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $<
$(OUTPUT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS)
clean:
	$(RM) -f $(OUTPUT) $(OBJECTS)
run:
	./$(OUTPUT)
bootstrap:
	$(MIPSAS) $(MIPSASFLAGS) bootstrap.s -o bootstrap.o 
	$(MIPSLD) $(MIPSLDFLAGSBOOT) bootstrap.o -o bootstrap.elf
cleanbootstrap:
	$(RM) -f bootstrap.o bootstrap.elf
testrom:
	$(MIPSAS) $(MIPSASFLAGS) test.s -o test.o
	$(MIPSLD) $(MIPSLDFLAGS) test.o -o test.elf
cleantestrom:
	$(RM) -f test.o test.elf




