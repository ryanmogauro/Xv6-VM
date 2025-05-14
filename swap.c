// swap.c
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "fs.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"
#include "mmu.h"
#include "proc.h"
#include "swap.h"

int swap_ready = 0;

extern struct {
  struct spinlock lock;
  struct proc     proc[NPROC];
} ptable;

static uchar swapmap[SWAP_PAGES/8]; 
static struct spinlock swaplock; 

//initializes swapmap on disk
void swap_init(void){
    initlock(&swaplock, "swap");
    memset(swapmap, 0, sizeof swapmap);
    swap_ready = 1;
}

static int bit_is_set(int i){ 
    return swapmap[i>>3] &  (1<<(i&7)); 
}

static void bit_set(int i){
    swapmap[i>>3] |= (1<<(i&7)); 
}

static void bit_clear(int i){
  swapmap[i>>3] &= ~(1<<(i&7)); 
}

int swap_alloc(void){
    acquire(&swaplock);

    for(int i = 0; i < SWAP_PAGES; i++){
        if(!(bit_is_set(i))){
            bit_set(i); 
            release(&swaplock); 
            return i;
        }
    }
    release(&swaplock);
    return -1;
}

void swap_free(int slot){
    acquire(&swaplock); 
    bit_clear(slot);
    release(&swaplock); 
}

void write_page_to_disk(char *kva, int slot){
    for(int i = 0; i < 8; i++){
        struct buf *b = bread(ROOTDEV, SWAP_START_BLOCK + slot*8 + i);
        memmove(b->data, kva+i*BSIZE, BSIZE);
        bwrite(b); 
        brelse(b); 
    }
}

void read_page_from_disk(char *kva, int slot){
    for (int i = 0; i < 8; i++){
        struct buf *b = bread(ROOTDEV, SWAP_START_BLOCK + slot*8 + i);
        memmove(kva + i*BSIZE, b->data, BSIZE); 
        brelse(b); 
    }
}

//Proof of concept
//O(# proc x # pt), optimize later if time
int swap_out_global(void){

    for(struct proc *p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->state == UNUSED || p->pgdir == 0){
            continue;
        }
        
        for(uint pdx = 0; pdx < NPDENTRIES; pdx++){
            pde_t pde = p->pgdir[pdx]; 
            if(!(pde & PTE_P)){
                continue;
            }

            pte_t *pt = (pte_t*)P2V(PTE_ADDR(pde));
            for(uint ptx=0; ptx<NPTENTRIES; ptx++){
                pte_t *pte = &pt[ptx];

                if((*pte & (PTE_P | PTE_U)) != (PTE_P|PTE_U)){
                    continue; 
                }

                char *kva = (char*)P2V(PTE_ADDR(*pte)); 
                int slot = swap_alloc(); 
                if(slot < 0){
                    return 0; 
                }

                write_page_to_disk(kva, slot); 
                *pte = (slot << 12) | PTE_SWAPPED | (PTE_FLAGS(*pte)&0x1FF); 
                invlpg((void*)PGADDR(pdx, ptx, 0)); 
                kfree(kva); 
                return 1;
            }
        }
    }
    return 0; //nothing swappable
}

