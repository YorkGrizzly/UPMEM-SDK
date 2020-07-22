#include<stdio.h>
#include<stdint.h>
#include <mram.h>
#include <perfcounter.h>
#define QUERY_SIZE 8
#define CORPUS_SIZE 262144

__mram_noinit uint8_t QUERY[QUERY_SIZE];
__mram_noinit uint8_t CORPUS[CORPUS_SIZE];
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
    size_t marker_ptr = 0;
    for(size_t i = 0; i < 64; i++){
      marker[i] = -1;
    }
    for(size_t i = 0; i < CORPUS_SIZE; i++)
        {
            match = QUERY[match] == CORPUS[i] ? match + 1 : 0;
            if(match == QUERY_SIZE)
            {
                if(count == 0) printf("got a match with mram loading!\n");
                count++;
                match = 0;
                marker[marker_ptr] = i - QUERY_SIZE + 1;
                marker_ptr ++;
            }
        }
    perfcounter_t runtime = perfcounter_get();
    printf("DPU runtime = %lu\n", runtime);

    /*
    //char QUERY[3] = {'A', 'A', 'C'};
    //uint8_t buffer[BUFFER_SIZE] = "ACAAATACGATCGGGATAGGTTCATTTGACGCGCTGCCATAGCAAACGAGTTGAATCATTCGTGGCATAACCCCCGGCGTCGAGTATCACAGCAGTGTCCGGGACGGCGTGCGCAGGTTTTTATTCTGCAGGCGTACCTATCATGGTACTCCATGCATTGTCAATGATCTTCAACCCGTGCGAGAGTGCACGCCGTTCGACTAAAAGCGCTAACTTATAATATCTACCCATGTTTGTAACTTCCTGCCAGAATAGAGGGCTATGCCTTACTTCCGGCTGATCGACTACCCTCCTGACTTAATCAGCTGGCCTCACGCGTGCAAGTCACCGGCTGCTATGTGTGTCTATCTGACTACACTCAAATCGCGACGGCCTGCAGTCACCAGGTCCTAGCCGAGAGTATAGTTTGAGTTGACTAAATGTGGGAACGCGTAGACATAGTATCACACCGCTGGGTATGCCGTATCTTACGCTGGAAAAGAAAGTTGGAGTTCAAGACCTGAACATCCGGGTGTAAGGGTTTGTAAGAGCGCCTTCCTATCCCTGATGGCACCCCTGGTAAGGGGGTAGGGCATGGGAATATAGTCTATACCGTACGTGAGCCCCGA";
    count = 0;
    uint8_t match = 0;
    
    printf("Found a total of: %d matches", count);
*/
    return 0;
}