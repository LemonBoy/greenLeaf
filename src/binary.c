#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "elf_eabi.h"
#include "emulator.h"

#include "machdep/mach_memory.h"

inline int isValidMipsElf(Elf32_Ehdr *elfHdr)
{
	if(memcmp((char *)elfHdr->e_ident, ELFMAG, SELFMAG) == 0) /* Is a valid elf ? */
		if(elfHdr->e_machine == EM_MIPS) /* A mips one ? */
			return 1;
	
	return 0;
}

u32 openElf(mipsCpu* cpu, char *path)
{
	int section;
	u8 * sectionBuffer = NULL;
	
	Elf32_Ehdr elfHdr;
	Elf32_Shdr sectionHdr;

	FILE* fd = fopen(path, "rb");
	
	if(fd == NULL) {
		return -1;
	}
	
	fread(&elfHdr, 1, sizeof(Elf32_Ehdr), fd);
	
	if(!isValidMipsElf(&elfHdr)) {
		return -1;
	}
		
	for(section = 0; section < elfHdr.e_shnum; section++) {
		fseek(fd, elfHdr.e_shoff + (section * sizeof(Elf32_Shdr)), SEEK_SET);
		fread(&sectionHdr, 1, sizeof(Elf32_Shdr), fd);
		
		if ((!(sectionHdr.sh_flags & SHF_ALLOC)) ||
		    (!(sectionHdr.sh_addr))              ||
		    (!(sectionHdr.sh_size)))
		{
			continue;
		}
		
#ifdef DEBUG
		printf("Section :         %d\n", section);
		printf("Size    :         0x%08X\n", sectionHdr.sh_size);
		printf("Offset  :         0x%08X\n", sectionHdr.sh_offset);
		printf("Address :         0x%08X\n", sectionHdr.sh_addr);
#endif
		if(sectionHdr.sh_flags & SHT_NOBITS) {
			memoryset(cpu, sectionHdr.sh_addr, 0, sectionHdr.sh_size);
		} else {
			sectionBuffer = malloc(sectionHdr.sh_size);
			
			fseek(fd, sectionHdr.sh_offset, SEEK_SET);
			fread(sectionBuffer, 1, sectionHdr.sh_size, fd);
			
			memcopy(cpu, sectionBuffer, sectionHdr.sh_addr, sectionHdr.sh_size);
			
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
	memcopy(cpu, buf, addr, sz);
	free(buf);
	return addr;
}
