#ifndef _MIPSDEFS_H_
#define _MIPSDEFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "inttypes.h"

#define MAX_STR 		1024
#define MAX_BANK 		8
#define ROM_BANKS		16

typedef struct mips_mem_bank_s
{
	UInt32 (*read_byte) (UInt32 addr);
	void (*write_byte) (UInt32 addr, UInt32 data);
	UInt32 (*read_halfword) (UInt32 addr);
	void (*write_halfword) (UInt32 addr, UInt32 data);
	UInt32 (*read_word) (UInt32 addr);
	void (*write_word) (UInt32 addr, UInt32 data);
	
	UInt64 (*read_doubleword) (UInt32 addr); //Maybe unusable for MIPS
	void (*write_doubleword) (UInt32 addr, UInt64 data);
	
	unsigned long addr, len;
	char filename[MAX_STR];
	unsigned type;		//chy 2003-09-21: maybe io,ram,rom
}mips_mem_bank_t;


typedef struct mips_mem_config_s
{
	int bank_num;
	int current_num;	//current num of bank
	mips_mem_bank_t mem_banks[MAX_BANK];
}mips_mem_config_t;


typedef struct mips_mem_state_s
{
	UInt32 *dram;
	UInt32 *rom[ROM_BANKS]; //Shi yang 2006-08-24
	UInt32 rom_size[ROM_BANKS];
//teawater add for arm2x86 2004.12.04-------------------------------------------
	UInt8 *tbp[ROM_BANKS];	//translate block pointer
	struct tb_s *tbt[ROM_BANKS];	//translate block structure pointer
//AJ2D--------------------------------------------------------------------------

}mips_mem_state_t;

void mips_mem_read(UInt32 pa, UInt32 *data, int len);
void mips_mem_write(UInt32 pa, const UInt32* data, int len);
void load(MIPS_State* mstate, VA va, PA pa, UInt32* x, int size);
void store(MIPS_State* mstate, UInt32 data, VA va, PA pa, int size);
#endif //end of _MIPSDEF_H_
