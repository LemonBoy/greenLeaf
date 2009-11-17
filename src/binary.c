#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "elf_eabi.h"

inline int isValidMipsElf(Elf32_Ehdr *elfHdr)
{
	if(memcmp((char *)elfHdr->e_ident, ELFMAG, SELFMAG) == 0) /* Is a valid elf ? */
		if(elfHdr->e_machine == EM_MIPS) /* A mips one ? */
			return 1;
	
	return 0;
}

u32 openElf(char *path, u32 baseaddr)
{
	int section;
	
	Elf32_Ehdr elfHdr;
	Elf32_Shdr sectionHdr;
	
	u32 entryPoint = 0;
	
	FILE* fd = fopen(path, "rb");
	if(fd == NULL)
		return -1;
	
	fread(&elfHdr, 1, sizeof(Elf32_Ehdr), fd);
	
	if(!isValidMipsElf(&elfHdr))
		return -1;

	for(section = 0; section < elfHdr.e_shnum; section++) {
		fseek(fd, elfHdr.e_shoff + (section * sizeof(Elf32_Shdr)), SEEK_SET);
		fread(&sectionHdr, 1, sizeof(Elf32_Shdr), fd);
		
#ifdef DEBUG
		printf("Section addr 0x%08X\n", sectionHdr.sh_addr);
#endif
		
		if(sectionHdr.sh_flags & SHT_NOBITS) {
			memoryset(sectionHdr.sh_addr, 0, sectionHdr.sh_size);
		}
		
		if(sectionHdr.sh_flags & SHF_EXECINSTR) {
#ifdef DEBUG
			printf("Executable section %d (Size: 0x%08X)\n", section, sectionHdr.sh_size);
			printf("offset 0x%08X\n", sectionHdr.sh_offset);
#endif
			entryPoint |= (sectionHdr.sh_addr & 0x3FFFFFFF);
			//~ entryPoint |= baseaddr;
			
			u8 *sectionBuffer = malloc(sectionHdr.sh_size);
			
			fseek(fd, sectionHdr.sh_offset, SEEK_SET);
			fread(sectionBuffer, 1, sectionHdr.sh_size, fd);
			
			memcopy(sectionBuffer, baseaddr, sectionHdr.sh_size);
			
			free(sectionBuffer);				
		}	
			
	}
		
	fclose(fd);
		
	return entryPoint;
}
