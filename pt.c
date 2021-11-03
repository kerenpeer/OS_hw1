# include "os.h"
# define LEVELS 5
# define VPN_BLOCK_SIZE 9



uint64_t get_curr(uint64_t vpn, int j);
void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn);
uint64_t page_table_query(uint64_t pt, uint64_t vpn);

uint64_t get_curr(uint64_t vpn, int j){
    uint64_t curr;
    int mask;
    mask = ((1 << VPN_BLOCK_SIZE) - 1);
    curr = (vpn >> (45-9*j)) & mask;
    return curr; 
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    int i;
    uint64_t curr_index, pte, last_index;
    
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
        level = phys_to_virt(pte & (~1));
    }
    last_index = get_curr(vpn,5);
    if(ppn == NO_MAPPING){
        level[last_index] = 0;
    }
    else{
        level[last_index]= (ppn << 12) | 1;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    int i;
    uint64_t curr_index, pte, ppn, last_index;

    uint64_t* level = phys_to_virt(pt << 12);

    for(i = 0; i < LEVELS; i++){
        curr_index = get_curr(vpn,i);
        pte = level[curr_index];
        if((pte & 1) != 1){
            return NO_MAPPING;
        }
        level = phys_to_virt(pte & (~1));
    }
    last_index = get_curr(vpn,5);
    ppn = level[last_index];
    if((ppn & 1) != 1){
        return NO_MAPPING;
    }
    return (ppn >> 12);
}