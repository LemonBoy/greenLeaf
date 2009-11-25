#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "elf32.h"
#include "elf64.h"
#include "emulator.h"

#include "mach_memory.h"

static int isValidMipsElf32(Elf32_Ehdr *elfHdr)
{
	if(memcmp((char *)elfHdr->e_ident, ELFMAG, SELFMAG) == 0) {	/* Is a valid elf ? */
		if(elfHdr->e_machine == EM_MIPS) {			/* A mips one ? */
			return 1;
		}else if(elfHdr->e_machine == EM_NONE) {		/* A none is fine too */
			return 1;
		}else{
#ifdef DEBUG
			printf("Not a MIPS ELF. Is %d when should be %d.\n", elfHdr->e_machine, EM_MIPS);
#endif
		}
	}else{
#ifdef DEBUG
		printf("Not an ELF.\n");
#endif
	}
	
	return 0;
}

u32 openElf32(mipsCpu* cpu, char *path)
{
	int section;
	u8* sectionBuffer = NULL;
	
	Elf32_Ehdr elfHdr;
	Elf32_Shdr sectionHdr;

	FILE* fd = fopen(path, "rb");
	
	if(fd == NULL) {
#ifdef DEBUG
		printf("Couldn't open %s\n", path);
#endif
		return -1;
	}
	
	fread(&elfHdr, 1, sizeof(Elf32_Ehdr), fd);
	
	if(!isValidMipsElf32(&elfHdr)) {
#ifdef DEBUG
		printf("%s is not a valid MIPS ELF.\n", path);
#endif
		fclose(fd);
		return -1;
	}
		
#ifdef DEBUG
	printf("Section#:         %d\n", elfHdr.e_shnum);
#endif
	for(section = 0; section < elfHdr.e_shnum; section++) {
		fseek(fd, elfHdr.e_shoff + (section * sizeof(Elf32_Shdr)), SEEK_SET);
		fread(&sectionHdr, 1, sizeof(Elf32_Shdr), fd);
		
#ifdef DEBUG
		printf("Section :         %d\n", section);
		printf("Size    :         0x%08X\n", sectionHdr.sh_size);
		printf("Offset  :         0x%08X\n", sectionHdr.sh_offset);
		printf("Address :         0x%08X\n", sectionHdr.sh_addr);
		printf("Flags   :         0x%08X\n", sectionHdr.sh_flags);
#endif
		if ((!(sectionHdr.sh_flags & SHF_ALLOC)) ||
		    (!(sectionHdr.sh_addr))              ||
		    (!(sectionHdr.sh_size)))
		{
			continue;
		}
		
		if(sectionHdr.sh_flags & SHT_NOBITS) {
#ifdef DEBUG
			printf("No bits\n");
#endif
			memoryset_load(cpu, sectionHdr.sh_addr, 0, sectionHdr.sh_size);
		} else {
			sectionBuffer = malloc(sectionHdr.sh_size);
			
			fseek(fd, sectionHdr.sh_offset, SEEK_SET);
			fread(sectionBuffer, 1, sectionHdr.sh_size, fd);
			
			memcopy_load(cpu, sectionBuffer, sectionHdr.sh_addr, sectionHdr.sh_size);
			
			free(sectionBuffer);				
		}	
			
	}
		
	fclose(fd);
		
	return elfHdr.e_entry;
}

static int isValidMipsElf64(Elf64_Ehdr *elfHdr)
{
	if(memcmp((char *)elfHdr->e_ident, ELFMAG, SELFMAG) == 0) {	/* Is a valid elf ? */
		if(elfHdr->e_machine == EM_MIPS) {			/* A mips one ? */
			return 1;
		}else if(elfHdr->e_machine == EM_NONE) {		/* A none is fine too */
			return 1;
		}else{
#ifdef DEBUG
			printf("Not a MIPS ELF. Is %d when should be %d.\n", elfHdr->e_machine, EM_MIPS);
#endif
		}
	}else{
#ifdef DEBUG
		printf("Not an ELF.\n");
#endif
	}
	
	return 0;
}

u64 openElf64(mipsCpu* cpu, char *path)
{
	int section;
	u8* sectionBuffer = NULL;
	
	Elf64_Ehdr elfHdr;
	Elf64_Shdr sectionHdr;
	
	FILE* fd = fopen(path, "rb");
	
	if(fd == NULL) {
#ifdef DEBUG
		printf("Couldn't open %s\n", path);
#endif
		return -1;
	}
	
	fread(&elfHdr, 1, sizeof(Elf64_Ehdr), fd);
	
	if(!isValidMipsElf64(&elfHdr)) {
#ifdef DEBUG
		printf("%s is not a valid MIPS ELF.\n", path);
#endif
		fclose(fd);
		return -1;
	}
	
#ifdef DEBUG
	printf("Section#:         %d\n", elfHdr.e_shnum);
#endif
	for(section = 0; section < elfHdr.e_shnum; section++) {
		fseek(fd, elfHdr.e_shoff + (section * sizeof(Elf64_Shdr)), SEEK_SET);
		fread(&sectionHdr, 1, sizeof(Elf64_Shdr), fd);
		
#ifdef DEBUG
		printf("Section :         %d\n", section);
		printf("Size    :         0x%016llX\n", sectionHdr.sh_size);
		printf("Offset  :         0x%016llX\n", sectionHdr.sh_offset);
		printf("Address :         0x%016llX\n", sectionHdr.sh_addr);
		printf("Flags   :         0x%016llX\n", sectionHdr.sh_flags);
#endif
		if ((!(sectionHdr.sh_flags & SHF_ALLOC)) ||
		    (!(sectionHdr.sh_addr))              ||
		    (!(sectionHdr.sh_size)))
		{
			continue;
		}
		
		if(sectionHdr.sh_flags & SHT_NOBITS) {
#ifdef DEBUG
			printf("No bits\n");
#endif
			memoryset_load(cpu, sectionHdr.sh_addr, 0, sectionHdr.sh_size);
		} else {
			sectionBuffer = malloc(sectionHdr.sh_size);
			
			fseek(fd, sectionHdr.sh_offset, SEEK_SET);
			fread(sectionBuffer, 1, sectionHdr.sh_size, fd);
			
			memcopy_load(cpu, sectionBuffer, sectionHdr.sh_addr, sectionHdr.sh_size);
			
			free(sectionBuffer);				
		}	
		
	}
	
	fclose(fd);
	
	return elfHdr.e_entry;
}


u32 openRaw(mipsCpu* cpu, char *path, u32 addr)
{
	u8* buf = NULL;
	u32 sz;
	FILE* fd = fopen(path, "rb");
	if(fd == NULL) {
		return -1;
	}
	fseek(fd, 0, SEEK_END);
	sz = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	buf = malloc(sz);
	fread(buf, 1, sz, fd);
	fclose(fd);
	printf("Size    :         0x%08X\n", sz);
	printf("Address :         0x%08X\n", addr);
	memcopy_load(cpu, buf, addr, sz);
	free(buf);
	return addr;
}
