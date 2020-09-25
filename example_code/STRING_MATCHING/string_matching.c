
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include <mram.h>
#include <perfcounter.h>
#define QUERY_SIZE 8
#define DPU_NUM 64
#define CORPUS_SIZE 4096

__mram_noinit uint8_t QUERY[QUERY_SIZE];
__mram_noinit uint8_t CORPUS[CORPUS_SIZE];
__mram_noinit struct1 s;
__host uint32_t count;
__host uint32_t marker[64];

int main()  
{
    perfcounter_config(COUNT_CYCLES, true);
    count = 0;
    // for(size_t i = 0; i < QUERY_SIZE; i++)
    // {
    //     printf("Hello\n");
    //     printf("%d\n", QUERY[i]);
    // }
    uint8_t match = 0;
    // uint8_t marker_ptr = 0;
    uint8_t marker_ptr = 0;
    for(size_t i = 0; i < 64; i++){
      marker[i] = -1;
    }

    // printf("Printing corpus...\n");
    // for(size_t i = 0; i < CORPUS_SIZE; i++)
    // {
    //     printf("%c", CORPUS[i]);
    // }
    // printf("\nPrinting query...\n");
    // for(size_t i = 0; i < QUERY_SIZE; i++)
    // {
    //     printf("%c", QUERY[i]);
    // }
    // printf("\n");

    uint32_t corpus_itr = 0;
    while (corpus_itr < CORPUS_SIZE)
    {
        if(QUERY[match] == CORPUS[corpus_itr])
        {
            match ++;
        }
        else
        {
            corpus_itr -= match;
            match = 0;
        }

        if(match == QUERY_SIZE)
            {
                if(count == 0) printf("got a match with mram loading!\n");
                count++;
                match = 0;
                //printf("marker ptr: %d\n", marker_ptr);
                //printf("corpus itr: %d\n\n", corpus_itr);

                marker[marker_ptr] = corpus_itr - QUERY_SIZE + 1;
                marker_ptr ++;
            }
        corpus_itr ++;
    }



    // for(size_t i = 0; i < CORPUS_SIZE; i++)
    //     {
    //         match = QUERY[match] == CORPUS[i] ? match + 1 : 0;
    //         if(match == QUERY_SIZE)
    //         {
    //             // if(count == 0) printf("got a match with mram loading!\n");
    //             count++;
    //             match = 0;
    //             marker[marker_ptr] = i - QUERY_SIZE + 1;
    //             marker_ptr ++;
    //         }
    //     }
    printf("Got total %d matches\n", count);
    perfcounter_t runtime = perfcounter_get();
    printf("DPU runtime = %lu\n", runtime);

    return 0;
}