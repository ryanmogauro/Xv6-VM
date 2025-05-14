#ifndef _SWAP_H_
#define _SWAP_H_

#include "types.h"
#include "param.h"

extern int swap_ready;

#define SWAP_START_BLOCK   600
#define SWAP_NBLOCKS       2048
#define SWAP_PAGES         (SWAP_NBLOCKS/8)

#ifndef PTE_SWAPPED
#define PTE_SWAPPED 0x200
#endif

void  swap_init(void);
int   swap_alloc(void);
void  swap_free(int slot);
void  write_page_to_disk(char *kva, int slot);
void  read_page_from_disk(char *kva, int slot);
int   swap_out_global(void);

#endif
