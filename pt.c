# include "os.h"
# define LEVELS 5
# define VPN_BLOCK_SIZE 9

uint64_t get_curr(uint64_t vpn, int j){
    uint64_t curr, mask;
    mask = ((1 << VPN_BLOCK_SIZE) - 1);
    curr = (vpn >> (45-9*j)) & mask;
    return curr; 
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    int i;
    uint64_t curr_index, pte;
    uint64_t * level = phys_to_virt(pt << 12);

    for(i = 0; i < LEVELS; i++){
        curr_index = get_curr(vpn,i);
        pte = level[curr_index];
        if(pte == 0){
            if(ppn == NO_MAPPING){
                return;
            }
            else{
                level[curr_index] = (alloc_page_frame() << 12) | 1;
                pte = level[curr_index];
            }  
        }
        level = phys_to_virt(pte & (~1));
    }
    if(ppn == NO_MAPPING){
        level[5] = 0;
    }
    else{
        level[5] = (ppn << 12)+1;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    int i;
    uint64_t curr_index, pte, ppn;

    uint64_t * level = phys_to_virt(pt << 12);

    for(i = 0; i < LEVELS; i++){
        curr_index = get_curr(vpn,i);
        pte = level[curr_index];
        if((pte & 1) != 1){
            return NO_MAPPING;
        }
        level = phys_to_virt(pte & (~1));
    }
    ppn = *level;
    if((ppn & 1) != 1){
            return NO_MAPPING;
    }
    return (ppn >>12);
}