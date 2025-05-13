// Define 1MB swap region on emulated disk
#define SWAP_START_BLOCK 600
#define SWAP_NBLOCKS     2048
#define SWAP_PAGES       (SWAP_NBLOCKS / 8)   // 256 swap slots

extern void  swap_init(void);
extern int   swap_alloc(void);
extern void  swap_free(int slot);
extern int   swap_out_global(void);
extern void  swap_in(char *kva, int slot);
