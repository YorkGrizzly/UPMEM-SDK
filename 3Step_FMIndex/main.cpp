#include "create_index.h"

#define DEBUG 1

int main(int argc, char *args[]) 
{ 
    //char *read[] = {"AATCGCAT$", "GCATAAAG$", "AAAGCCTA$"};
    char *read[] = {"GATCGGAATCGGAGAATCGT$"};
    //Reads to be used to generate suffixes
    //char *read[] = {"AAAGCCTA$", "AAAGCCTA$", "AAAGCCTA$", "TTTGCCAA$", "TTTGCCTTT", "$GCCTATAG", "AAAAAA$TG"};
    //Number of reads
    //int read_count = 3;
    int read_count = 1;
    //Number of suffixes encountered so far
    int suffix_count = 0;
    //Length of each suffix
    int read_length = strlen(read[0]);
    //Total number of suffixes possible
    int num_suffixes = read_length*read_count;
    //Array of all suffixes, before as well as after sorting
    char *suffixes[num_suffixes];
    
    for(int i=0;i<num_suffixes;i++)
        suffixes[i] = (char*)malloc((read_length)*sizeof(char));

    //Generate suffix for each suffix and add to the array
    for(int i=0;i<read_count;i++)
        generate_all_suffixes(suffixes, &suffix_count, read[i], strlen(read[i]));


    //sort the suffixes using quicksort (C++ default implementation)
    sort((const char**)suffixes, num_suffixes);

#ifdef DEBUG
    printf("Printing sorted suffixes:\n");
    for(int i=0;i<num_suffixes;i++){
        cout<<i<<": ";
        for(int j=0;j<read_length;j++)
            cout<<suffixes[i][j];
        cout<<endl;
    }
    printf("-----------------------------------------------------------------\n");
#endif

    ThreeStepFMIndex fm_index;

    //Integer value of global counts encountered from 0 to (d_major*i-1)th stored in occ_major[i]
    int d_major = 4;
    //Byte value of localal counts encountered from d_minor*ith to (d_minor*i + d_minor - 1)th stored in occ_minor[i]
    int d_minor = 1;

    //Initializes fm_index class variables
    init_three_step_fm_index(&fm_index, num_suffixes, d_major, d_minor);

    //Creates fm_index from the sorted suffixes
    calculate_three_step_fm_index_from_sorted_suffixes(&fm_index, suffixes, read_length);

    //kmer to be searched in the fm_index
    char *kmer = "GAATCG";
    //Length of the kmer to be searched
    int kmer_length = 6;
    //result_start_interval'th to result_end_result'th suffixes in sorted suffixes list are match results of kmer in fm_index
    int result_start_interval = 0, result_end_result = 0;

#ifdef DEBUG
    printf("Querying for ");
    for(int i=0;i<kmer_length;i++)
        printf("%c",kmer[i]);
    printf("\n");
#endif

    //Perform query for kmer in fm_index
    query_3step_fm_index(&fm_index, kmer, kmer_length, &result_start_interval, &result_end_result);

    printf("Result limits : %d to %d\n", result_start_interval, result_end_result);
    return 0; 
} 
