#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/mman.h>
#include <inttypes.h>

#include "os.h"
# define LEVELS 5
# define VPN_BLOCK_SIZE 9

int get_curr(uint64_t vpn, int j);
void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn);
uint64_t page_table_query(uint64_t pt, uint64_t vpn);

int get_curr(uint64_t vpn, int j){
    uint64_t curr, shift;
    int mask;
    mask = ((1 << VPN_BLOCK_SIZE) - 1);
    shift = (vpn >> (45-9*j));
    curr = shift & mask;
    return curr; 
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    int i, curr_index, last_index;
    uint64_t pte;
    
    uint64_t* level = phys_to_virt(pt << 12);
    for(i = 0; i < LEVELS; i++){
        curr_index = get_curr(vpn,i);
        pte = level[curr_index];
        if((pte & 1) != 1){
            if(ppn == NO_MAPPING){
                return;
            }
            else{
                level[curr_index] = (alloc_page_frame() << 12) | 1;
                pte = level[curr_index];
            }  
        }
        level = phys_to_virt(pte & 0xFFFFFFFFFFFFF000);
    }
    last_index = get_curr(vpn, LEVELS);
    if(ppn == NO_MAPPING){
        level[last_index] = 0;
    }
    else{
        level[last_index]= (ppn << 12) | 1;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    int i, curr_index, last_index;
    uint64_t pte, ppn;

    uint64_t* level = phys_to_virt(pt << 12);

    for(i = 0; i < LEVELS; i++){
        curr_index = get_curr(vpn,i);
        pte = level[curr_index];
        if((pte & 1) != 1){
            return NO_MAPPING;
        }
        level = phys_to_virt(pte & 0xFFFFFFFFFFFFF000);
    }
    last_index = get_curr(vpn, LEVELS);
    ppn = level[last_index];
    printf("valid is:\n");
    printf("%" PRIu64 "\n",(ppn & 1));
    if((ppn & 1) != 1){
        return NO_MAPPING;
    }
    return (ppn >> 12);
}