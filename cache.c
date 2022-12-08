#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


// variables that keep track of what the evicted block from L1 is
unsigned long address_evicted;

struct node{
    unsigned long tagValue;
    unsigned long origAddress;
    struct node *next;
}*head_L1[1000], *head_L2[1000]; // create 2 heads

int hitOrMissL1(unsigned long set_index_L1, unsigned long tag_L1){
    // return 0 for miss, 1 for hit
    // check if hit or miss
    struct node *tmp = head_L1[set_index_L1];

    // if chain is null, miss
    if(head_L1[set_index_L1] == NULL){
        return 0;

    // if chain is not null
    }else{
        // loop through specified set, check for hit
        while(tmp != NULL){
            if(tmp->tagValue == tag_L1){
                return 1; // stop searching if tag match is in the cache and return 1 - hit
            }
            tmp = tmp->next;
        }
        // if no hits found, return miss
        return 0;
    }
}
int hitOrMissL2(unsigned long set_index_L2, unsigned long tag_L2){
    struct node *tmp = head_L2[set_index_L2];
    if(head_L2[set_index_L2] == NULL){
        return 0;
    }else{
        while(tmp != NULL){
            if(tmp->tagValue == tag_L2){
                return 1;
            }
            tmp = tmp->next;
        }
        return 0;
    }
}


void insertCacheL1(unsigned long set_index_L1, unsigned long tag_L1, unsigned long address){
    // chaining to keys (set index)
    struct node *new = head_L1[set_index_L1];
    struct node *tmp = head_L1[set_index_L1];
    new = (struct node*)malloc(sizeof(struct node));
    new -> tagValue = tag_L1;
    new -> origAddress = address;
    new -> next = NULL;
    // if nothing in chain, set head as new node
    if(head_L1[set_index_L1] == NULL){ 
        head_L1[set_index_L1] = new;
        head_L1[set_index_L1] -> next = tmp;
    // if chain is not empty, add to end of list
    }else{
        while(tmp -> next!= NULL){
            tmp = tmp -> next;
        }tmp -> next = new;
    }
}

void insertCacheL2(unsigned long set_index_L2, unsigned long tag_L2, unsigned long address){
    // chaining to keys (set index)
    struct node *new = head_L2[set_index_L2];
    struct node *tmp = head_L2[set_index_L2];
    new = (struct node*)malloc(sizeof(struct node));
    new -> tagValue = tag_L2;
    new -> origAddress = address;
    new -> next = NULL;
    // if nothing in chain, set head as new node
    if(head_L2[set_index_L2] == NULL){ 
        head_L2[set_index_L2] = new;
        head_L2[set_index_L2] -> next = tmp;
    // if chain is not empty, add to end of list
    }else{
        while(tmp -> next!= NULL){
            tmp = tmp -> next;
        }tmp -> next = new;
    }
}

void removeCacheL2(unsigned long set_index_L2, unsigned long tag_L2, unsigned long assoc_L2){
    struct node *tmp = head_L2[set_index_L2];
    struct node *ptr = NULL;
    if(assoc_L2 == 1){ // if associativity is one, remove the head
        free(tmp);
        tmp = NULL;
        head_L2[set_index_L2] = NULL;
    }else{
        if(tmp -> tagValue == tag_L2){ // if head is the tag, remove the head and move the rest up
            head_L2[set_index_L2] = tmp -> next;
            free(tmp);
        }else{
            while(tmp != NULL){ // once tag is found, remove and reconnect list
                if (tmp->tagValue == tag_L2){
                    ptr -> next = tmp -> next;
                    tmp -> next = head_L2[set_index_L2];
                    free(tmp);
                    return;
                }
                ptr = tmp;
                tmp = tmp -> next;
            }
        }
    }
}

void fifoOrLru(unsigned long set_index_L2){
    // level 2 is a victim cache, each one left is not hit so just remove from the head
    struct node *tmp = head_L2[set_index_L2];
    head_L2[set_index_L2] = head_L2[set_index_L2] -> next;
    free(tmp);
    tmp = head_L2[set_index_L2];
}  

void fifo(unsigned long set_index_L1){
    struct node *tmp = head_L1[set_index_L1];
    address_evicted = tmp -> origAddress; // maintain address that gets evicted
    head_L1[set_index_L1] = head_L1[set_index_L1] -> next;
    free(tmp);
    tmp = head_L1[set_index_L1];
}

void reorder(unsigned long set_index_L1, unsigned long tag_L1, unsigned long assoc_L1){
    // reorders so that the most recently used is in the front
    // if hit or accessed, move to the front
    struct node *tmp = head_L1[set_index_L1];
    struct node *ptr = NULL;
    if(assoc_L1 == 1){ // if associativity is 1, do not need to reorder
        return;
    }else{
        while(tmp != NULL){
            if(ptr!= NULL){
                if (tmp->tagValue == tag_L1){
                    ptr -> next = tmp -> next;
                    tmp -> next = head_L1[set_index_L1];
                    head_L1[set_index_L1] = tmp;
                }
            }
            ptr = tmp;
            tmp = tmp -> next;
        }
    }
}
void lru(unsigned long set_index_L1, unsigned long assoc_L1){
    // remove from end
    struct node *tmp = head_L1[set_index_L1];
    struct node *ptr = NULL;
    if(assoc_L1 == 1){
        address_evicted = tmp -> origAddress; // maintain the address that gets evicted
        free(tmp);
        tmp = NULL;
        head_L1[set_index_L1] = NULL;
    }else{ // else remove from end
        while(tmp -> next != NULL){
            ptr = tmp;
            tmp = tmp -> next;
        };
        address_evicted = tmp -> origAddress; // maintain address
        free(tmp -> next);
        free(tmp);
        ptr -> next = NULL;
    }
}

/* printing caches / hash tables functions for testing
void printCacheL1(unsigned long num_sets_L1){
    printf("CACHE L1:\n");
    for(int i = 0; i < num_sets_L1; i++){
        struct node *ptr = head_L1[i];
        printf("chain %d :", i);
        while(ptr != NULL){
            printf("TAG:%lu ADDRESS:%lu -> ", ptr->tagValue, ptr -> origAddress);
            ptr = ptr ->next;
        }
        printf("\n");
    }
}
void printCacheL2(unsigned long num_sets_L2){
    printf("CACHE L2:\n");
    for(int i = 0; i < num_sets_L2; i++){
        struct node *ptr = head_L2[i];
        printf("chain %d :", i);
        while(ptr != NULL){
            printf("TAG:%lu ADDRESS:%lu -> ", ptr->tagValue, ptr -> origAddress);
            ptr = ptr ->next;
        }
        printf("\n");
    }
}*/

// free memory
void freeCacheL1(unsigned long num_sets_L1){
    struct node *tmp;
    struct node *ptr;
    for(int i = 0; i < num_sets_L1; i++){
        tmp = head_L1[i];
        ptr = head_L1[i];
        while(ptr){
            tmp = ptr;
            ptr = ptr -> next;
            free(tmp);
        }
    }
}
void freeCacheL2(unsigned long num_sets_L2){
    struct node *tmp;
    struct node *ptr;
    for(int i = 0; i < num_sets_L2; i++){
        tmp = head_L2[i];
        ptr = head_L2[i];
        while(ptr){
            tmp = ptr;
            ptr = ptr -> next;
            free(tmp);
        }
    }
}


int main(int argc, char* argv[argc+1]){
    if(argc < 9){
        printf("error");
        return EXIT_SUCCESS;
    }

    // get arguments and convert as necessary to unsigned longs
    char *inp1 = argv[1];

    char *inp2 = argv[2];
    char inp2_arr[80];
    strcpy(inp2_arr, inp2);
    char *inp2_first = strtok(inp2_arr, ":"); // split second argument into 2 parts
    char *inp2_second = strtok(NULL, ":");

    char *inp3 = argv[3];
    char *inp4 = argv[4];
    char *inp5 = argv[5];

    char *inp6 = argv[6];
    char inp6_arr[80];
    strcpy(inp6_arr, inp6);
    char *inp6_first = strtok(inp6_arr, ":"); // split sixth argument into 2 parts
    char *inp6_second = strtok(NULL, ":");

    char *inp7 = argv[7];
    FILE *fp = fopen(argv[8], "r");
    
    // convert to unsigned longs
    char *end;
    unsigned long cache_size_L1 = strtoul(inp1, &end, 0);
    unsigned long assoc_L1;
    if(strcmp(inp2_first, "assoc:")){
        assoc_L1 = strtoul(inp2_second, &end, 0);
    }
    char *cache_policy_L1 = inp3;
    unsigned long  block_size = strtoul(inp4, &end, 0); // same block size for L1 and L2
    unsigned long cache_size_L2 = strtoul(inp5, &end, 0);
    unsigned long assoc_L2;
    if(strcmp(inp6_first, "assoc:")){
        assoc_L2 = strtoul(inp6_second, &end, 0);
    }
    char *cache_policy_L2 = inp7;

    // initialize counters
    unsigned long mem_read_counter = 0;
    unsigned long mem_write_counter = 0;
    unsigned long cache_hit_counter_L1 = 0;
    unsigned long cache_miss_counter_L1 = 0;
    unsigned long cache_hit_counter_L2 = 0;
    unsigned long cache_miss_counter_L2 = 0;
    unsigned long block_counter_L1 = 1;
    unsigned long block_counter_L2 = 1;

    unsigned long set_index_evicted;
    unsigned long tag_evicted;

    // find number of sets
    unsigned long num_sets_L1 = cache_size_L1 / (block_size * assoc_L1);
    unsigned long num_sets_L2 = cache_size_L2 / (block_size * assoc_L2);

    // create sets for each level of cache / start the chains, set beginning to null
    for(int i = 0; i < num_sets_L1; i++){
        head_L1[i] = NULL;
    }
    for(int i = 0; i < num_sets_L2; i++){
        head_L2[i] = NULL;
    }

    // reading from trace file
    char instruction;
    char address_arr[80];

    while(fscanf(fp, "%c %s\n", &instruction, address_arr)!=EOF){
        unsigned long address = strtoul(address_arr, &end, 0); // convert address from char array to value
        // find block offset bits, same for both levels
        unsigned long block_offset_bits = log2l(block_size);

        /* don't need to find block_offset because not actually moving data
        unsigned long block_exponent = powl(2, block_offset_bits) - 1;
        unsigned long block_offset = address & block_exponent;*/

        // find set index bits and set index values
        unsigned long set_index_bits_L1 = log2l(num_sets_L1);
        unsigned long set_index_exp_L1 = powl(2, set_index_bits_L1) - 1;
        unsigned long set_index_L1 = (address >> block_offset_bits) & set_index_exp_L1;

        unsigned long set_index_bits_L2 = log2l(num_sets_L2);
        unsigned long set_index_exp_L2 = powl(2, set_index_bits_L2) - 1;
        unsigned long set_index_L2 = (address >> block_offset_bits) & set_index_exp_L2;

        // find tags
        unsigned long tag_L1 = address >> (block_offset_bits + set_index_bits_L1);
        unsigned long tag_L2 = address >> (block_offset_bits + set_index_bits_L2);

        // READ if instruction is equal to R (82)
        if(instruction == 82){ 

            // MISS L1: increment L1 cache miss
            if(hitOrMissL1(set_index_L1, tag_L1) == 0){
                cache_miss_counter_L1++;
        
                struct node *ptr1 = head_L1[set_index_L1]; // keep track of L1 block counter
                while(ptr1 != NULL){
                    block_counter_L1++;
                    ptr1 = ptr1 -> next;
                }
                // HIT L2: delete from L2, insert into L1
                if(hitOrMissL2(set_index_L2, tag_L2) == 1){
                    cache_hit_counter_L2++; // increment L2 cache hit
                    removeCacheL2(set_index_L2, tag_L2, assoc_L2);

                    // L1 not full: insert normally
                    if(block_counter_L1 <= assoc_L1){
                        insertCacheL1(set_index_L1, tag_L1, address);
                        if(strcmp(cache_policy_L1, "lru")==0){
                            reorder(set_index_L1, tag_L1, assoc_L1);
                        }
                    // L1 full: remove through fifo or lru, insert evicted into L2
                    }else{
                        if(strcmp(cache_policy_L1, "fifo") == 0){
                            fifo(set_index_L1);
                            // recalculate evicted index, tag, address
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);

                            struct node *ptr2 = head_L2[set_index_evicted];
                            while(ptr2 != NULL){
                                block_counter_L2++;
                                ptr2 = ptr2 -> next;
                            }
                            insertCacheL1(set_index_L1, tag_L1, address);
                            // check if L2 in set for evicted address from L1 is full
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted); // fifo or lru do the same thing for L2
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            }
                        }else{
                            lru(set_index_L1, assoc_L1);
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);
                            struct node *ptr3 = head_L2[set_index_evicted];
                            while(ptr3 != NULL){
                            block_counter_L2++;
                            ptr3 = ptr3 -> next;
                            }
                            insertCacheL1(set_index_L1, tag_L1,address);
                            reorder(set_index_L1, tag_L1, assoc_L1);
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted);
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            }
                        }
                    }
                }else{ // MISS L2 (MISS BOTH):
                    mem_read_counter++; // if miss both, read from memory
                    cache_miss_counter_L2++; // increment L2 cache miss

                    // L1 not full: insert normally
                    if(block_counter_L1 <= assoc_L1){
                        insertCacheL1(set_index_L1, tag_L1, address);
                        if(strcmp(cache_policy_L1, "lru") == 0){
                            reorder(set_index_L1, tag_L1, assoc_L1);
                        }
                    }
                    // L1 full: remove through fifo or lru, insert, insert evicted block into L2
                    else{
                        if(strcmp(cache_policy_L1, "fifo") == 0){
                            fifo(set_index_L1);
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);
                            struct node *ptr4 = head_L2[set_index_evicted]; // keep track of L2 block counter
                            while(ptr4 != NULL){
                            block_counter_L2++;
                            ptr4 = ptr4 -> next;
                            }
                            insertCacheL1(set_index_L1, tag_L1, address);

                            // L2 not full: insert evicted tag from L1 normally
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }
                            // L2 full: remove from L2 through fifoOrLru
                            else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted);
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            }
                        }else{
                            lru(set_index_L1, assoc_L1);
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);
                            struct node *ptr5 = head_L2[set_index_evicted]; // keep track of L2 block counter
                            while(ptr5 != NULL){
                            block_counter_L2++;
                            ptr5 = ptr5 -> next;
                            }
                            insertCacheL1(set_index_L1, tag_L1, address);
                            reorder(set_index_L1, tag_L1, assoc_L1);
                            // L2 not full:
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }
                            // L2 full:
                            else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted);
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            }
                        }
                    }
                }
            // HIT L1: increment L1 cache hit
            }else{
                if(strcmp(cache_policy_L1, "lru")==0){
                    reorder(set_index_L1, tag_L1, assoc_L1);
                }
                cache_hit_counter_L1++;
            }

        // WRITE if instruction is equal to W (87)
        }else{
            // if miss, increment cache miss, mem read, and mem write
            if(hitOrMissL1(set_index_L1, tag_L1) == 0){
                cache_miss_counter_L1++;
        
                struct node *ptr6 = head_L1[set_index_L1]; // keep track of L1 block counter
                while(ptr6 != NULL){
                    block_counter_L1++;
                    ptr6 = ptr6 -> next;
                }
                // HIT L2: delete from L2, insert into L1
                if(hitOrMissL2(set_index_L2, tag_L2) == 1){
                    cache_hit_counter_L2++; // increment L2 cache hit
                    removeCacheL2(set_index_L2, tag_L2, assoc_L2);

                    // L1 not full: insert normally
                    if(block_counter_L1 <= assoc_L1){
                        insertCacheL1(set_index_L1, tag_L1, address);
                        if(strcmp(cache_policy_L1, "lru")==0){
                            reorder(set_index_L1, tag_L1, assoc_L1);
                        }
                    // L1 full: remove through fifo or lru, insert evicted into L2
                    }else{
                        if(strcmp(cache_policy_L1, "fifo") == 0){
                            fifo(set_index_L1);
                            // recalculate evicted index, tag, address
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);
                            struct node *ptr7 = head_L2[set_index_evicted]; // keep track of L2 block counter
                            while(ptr7 != NULL){
                                block_counter_L2++;
                                ptr7 = ptr7 -> next;
                            }
                            insertCacheL1(set_index_L1, tag_L1, address);
                            // check if L2 is full
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted);
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            }
                        }else{
                            lru(set_index_L1, assoc_L1);
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);
                            struct node *ptr8 = head_L2[set_index_evicted]; // keep track of L2 block counter
                            while(ptr8 != NULL){
                                block_counter_L2++;
                                ptr8 = ptr8 -> next;
                            }
                            insertCacheL1(set_index_L1, tag_L1,address);
                            reorder(set_index_L1, tag_L1, assoc_L1);
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted);
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            }
                        }
                    }
                    mem_write_counter++;
                }else{ // MISS L2 (MISS BOTH):
                    mem_read_counter++; // if miss both, read and write
                    mem_write_counter++;
                    cache_miss_counter_L2++; // increment L2 cache miss

                    // L1 not full: insert normally
                    if(block_counter_L1 <= assoc_L1){
                        insertCacheL1(set_index_L1, tag_L1, address);
                        if(strcmp(cache_policy_L1, "lru") == 0){
                            reorder(set_index_L1, tag_L1, assoc_L1);
                        }
                    }
                    // L1 full: remove through fifo or lru, insert, insert evicted block into L2
                    else{
                        if(strcmp(cache_policy_L1, "fifo") == 0){
                            fifo(set_index_L1);
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);

                            struct node *ptr7 = head_L2[set_index_evicted]; // keep track of L2 block counter
                            while(ptr7 != NULL){
                            block_counter_L2++;
                            ptr7 = ptr7 -> next;
                            }
                            insertCacheL1(set_index_L1, tag_L1, address);

                            // L2 not full: insert evicted tag from L1 normally
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }
                            // L2 full: remove from L2 through fifoOrLru
                            else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted);
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            } 
                        }else{
                            lru(set_index_L1, assoc_L1);
                            set_index_evicted = (address_evicted >> block_offset_bits) & set_index_exp_L2;
                            tag_evicted = address_evicted >> (block_offset_bits + set_index_bits_L2);

                            struct node *ptr8 = head_L2[set_index_evicted]; // keep track of L2 block counter
                            while(ptr8 != NULL){
                            block_counter_L2++;
                            ptr8 = ptr8 -> next;
                            }

                            insertCacheL1(set_index_L1, tag_L1, address);
                            reorder(set_index_L1, tag_L1, assoc_L1);
                            // L2 not full:
                            if(block_counter_L2 <= assoc_L2){
                                insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                            }
                            // L2 full:
                            else{
                                if(strcmp(cache_policy_L2, "fifo") == 0 || strcmp(cache_policy_L2, "lru") == 0){
                                    fifoOrLru(set_index_evicted);
                                    insertCacheL2(set_index_evicted, tag_evicted, address_evicted);
                                }
                            }
                        }
                    }
                }
            // HIT L1: increment L1 cache hit and mem write
            }else{
                if(strcmp(cache_policy_L1, "lru")==0){
                    reorder(set_index_L1, tag_L1, assoc_L1);
                }
                cache_hit_counter_L1++;
                mem_write_counter++;
            }
        }
        block_counter_L1 = 1; // block counters reset to 1 for each iteration
        block_counter_L2 = 1;
        address_evicted = 0; //reset to 0 for each for each iteration
    }
    printf("memread:%lu\n", mem_read_counter);
    printf("memwrite:%lu\n", mem_write_counter);
    printf("l1cachehit:%lu\n", cache_hit_counter_L1);
    printf("l1cachemiss:%lu\n", cache_miss_counter_L1);
    printf("l2cachehit:%lu\n", cache_hit_counter_L2);
    printf("l2cachemiss:%lu\n", cache_miss_counter_L2);
    freeCacheL1(num_sets_L1);
    freeCacheL2(num_sets_L2);
    fclose(fp);
}