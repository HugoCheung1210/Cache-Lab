#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int hit_count, miss_count, eviction_count;    // global variable for printSummary()

int s,E,b,S=0;
char t[1000];

typedef struct{
    int valid_bit;
    int tag;
    int LRU_counter;
}cache_line,*cache_set,**cache;

cache cache_sim= NULL;  // initialize a NULL cache

void empty_cache() {
    int i, j;
    cache_sim = (cache) malloc(sizeof(cache_set) * S);  // Initializing array cache[S]
    for (i = 0; i < S; i++) {
        cache_sim[i] = (cache_set) malloc(sizeof(cache_line) * E); // Initializing 2D array cache[S][E]
        for (j = 0; j < E; j++) {    // Initializing empty cache line
            cache_sim[i][j].valid_bit = 0;
            cache_sim[i][j].tag = -1;
            cache_sim[i][j].LRU_counter = 0;
        }
    }
}

void LRU_update(){      // update LRU counter
    int i,j;
    for (i=0; i<S; i++){
        for (j=0; j<E; j++){
            if (cache_sim[i][j].valid_bit ==1)
                cache_sim[i][j].LRU_counter++;
        }
    }
}

void update(unsigned address){
    int input_tag = address >> (s+b);     // get address's tag
    int i;
    int input_set = (address >> b) & ((-1U)>>(64-s));    // get set index
    for (i=0; i<E; i++){  // hit
        if (cache_sim[input_set][i].tag == input_tag){
            cache_sim[input_set][i].valid_bit = 1;  // the cache line is now occupied
            cache_sim[input_set][i].LRU_counter = 0; // update the LPU counter to zero
            LRU_update();
            hit_count++;
            return;
        }
    }
    for (i=0; i<E; i++){  // find empty cache line
        if (cache_sim[input_set][i].valid_bit == 0){
            cache_sim[input_set][i].tag = input_tag;  // update the tag to input tag
            cache_sim[input_set][i].LRU_counter = 0; // update the LPU counter to zero
            cache_sim[input_set][i].valid_bit = 1;
            LRU_update();
            miss_count++;  // cold miss
            return;
        }
    }

    miss_count++;           // eviction if there is no hit and empty cache in set
    eviction_count++;

    int max_LRU_counter = cache_sim[input_set][0].LRU_counter;
    int max_LRU_line = 0;
    for (i=0; i<E; i++){   // find the cache line which LRU
        if (cache_sim[input_set][i].LRU_counter> max_LRU_counter){
            max_LRU_counter = cache_sim[input_set][i].LRU_counter;
            max_LRU_line = i;
        }
    }
    cache_sim[input_set][max_LRU_line].tag = input_tag; // eviction using LRU
    cache_sim[input_set][max_LRU_line].LRU_counter =0;
    LRU_update();

}

void read_trace(){
    FILE* fp = fopen(t, "r");
    if (fp == NULL){
        printf("Error in opening the trace file!");
        exit(-1);
    }

    char identifier;
    unsigned address;
    int size;
    while(fscanf(fp,"%c %x,%d", &identifier, &address, &size)>0){   // read line from trace file

        switch (identifier){
            case 'I': continue;
            case 'L':
                update(address);
                break;
            case 'M':   // modify
                update(address);
            case 'S':   // store again
                update(address);
            default: break;
        }

    }
    fclose(fp);
    for(int i = 0; i < S; i++){
        free(cache_sim[i]);         // free memory of cache
    }
    free(cache_sim);
}

int main(int argc, char** argv)
{
    int opt;
    while(-1 != (opt = getopt(argc, argv, "hvs:E:b:t:"))){      // parsing elements on unix command line
    switch(opt) {
        case 'h':  // not implementing helper function
            break;
        case 'v':  // verbose function
            break;
        case 's':   // number of set index bits
            s = atoi(optarg);
            break;
        case 'E':   // number of cache line in a set
            E = atoi(optarg);
            break;
        case 'b':   // number of block bit
            b = atoi(optarg);
            break;
        case 't':   // trace file name
            strcpy(t,optarg);
            break;
        default:
            printf("wrong argument\n");
            break;
    }
}
    S = 1<<s;
    empty_cache();
    read_trace();

    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
