#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DEBUG 1
//#define DEBUG_ENCODE 0
//Mapping used to reduce 0-255 range to 0-111, since only 112 valid values exist for 3-char strings of $,A,C,G and T, with no repetition for $
char OneByteEncoded[] = {-1,-1,-1,-1,-1,-1,0,1,2,3,-1,4,5,6,7,-1,8,9,10,11,-1,12,13,14,15,-1,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,-1,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,-1,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,-1,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111};

//Mapping used to map $->0, A->1, C->2, G->3, T->4
char CharToOneByte[] = {0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,2,-1,-1,-1,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,4};

//Map 3-chars to range 0-111. To be used for occ tables
char encode_3chars_to_1byte(char *str, int start){
    int index = 25*CharToOneByte[str[start]-'$'] + 5*CharToOneByte[str[start+1]-'$'] + CharToOneByte[str[start+2]-'$'];
#ifdef DEBUG_ENCODE
    printf("%d,%d,%d\t%d\n",CharToOneByte[str[start]-'$'], CharToOneByte[str[start+1]-'$'], CharToOneByte[str[start+2]-'$'], index);
#endif
    return OneByteEncoded[index];
}

//Data Structure Fix Required : Need to fix row_map table to 0-111
//Map 3-chars to range 0-124. To be used for row_map table
char encode_3chars_to_1byte_wo_normalization(char *str, int start){
    int index = 25*CharToOneByte[str[start]-'$'] + 5*CharToOneByte[str[start+1]-'$'] + CharToOneByte[str[start+2]-'$'];
#ifdef DEBUG_ENCODE
    printf("%d,%d,%d\t%d\n",CharToOneByte[str[start]-'$'], CharToOneByte[str[start+1]-'$'], CharToOneByte[str[start+2]-'$'], index);
#endif
    return index;
}

//short reads (1<=length<=255) can be represented using 1 char
#define SHORT_READ 0

//long reads(255<length) need more than 1 char
//#define LONG_READ 0


//Suffix Array record (read_id, suffix_array) for each suffix
typedef struct{
#ifdef SHORT_READ
    char *sa;
#endif
#ifdef LONG_READ
    int *sa;
#endif
    int *read_id;
}SuffixArray;

typedef struct{
    int n;//Total number of suffixes represented in fm_index
    int row_map[125];//Mapping to the first record starting with F-value in sorted suffixes

    //Data Structure Fix : Change usage from bwtString to bwtStrings
    char *bwtString;//Combined L-column values for all sorted suffixes
    char **bwtStrings;//F-value wise L-coulmn values for all sorted suffixes

    SuffixArray *suffix_array;//Suffix array for each suffix. Not calculated for time being

    int **occ_major;//Storing occ values every (d_major*i-1)th suffix
    unsigned char **occ_minor;//Storing occ values from (d_minor*i)th (d_minor*i+d_minor-1)th suffix
    int d_major;
    int d_minor;
}ThreeStepFMIndex;

/*----------------------------------Index creation functions start------------------------------------------------*/

//memory allocations for fm_index structure variables
void init_three_step_fm_index(ThreeStepFMIndex *fm_index, int num_suffixes, int d_major, int d_minor){
    fm_index->bwtString = (char*)malloc(num_suffixes);
    fm_index->bwtStrings = (char**)malloc(124*sizeof(char*));
    fm_index->suffix_array = NULL;
    fm_index->n = num_suffixes;
    fm_index->d_major = d_major;
    fm_index->d_minor = d_minor;
    int d_major_count = num_suffixes/d_major;
    int d_minor_count = num_suffixes/d_minor - num_suffixes/d_major;
    fm_index->occ_major = (int**)malloc(d_major_count*sizeof(int*));
    fm_index->occ_minor = (unsigned char**)malloc(d_minor_count*sizeof(unsigned char*));
    for(int i=0;i<d_major_count;i++)
        fm_index->occ_major[i] = (int*)malloc(112*sizeof(int));
    for(int i=0;i<d_minor_count;i++)
        fm_index->occ_minor[i] = (unsigned char*)malloc(112*sizeof(unsigned char));

    for(int i=0;i<125;++i)
        fm_index->row_map[i] = OneByteEncoded[i];
}

//Calculates fm_index values from the list of sorted suffixes
void calculate_three_step_fm_index_from_sorted_suffixes(ThreeStepFMIndex *fm_index, char *sorted_suffixes[], int suffix_length){
    int this_suffix_idx = 0;//Array index of the current suffix currently being examined. Value in range (0, fm_index->n-1)
    int this_F_column_idx = 0;//Array index of the F-value currently being examined. Value in range (0, 124)
    int previous_suffix_idx = 0;//Array index of the F-value previously being examined. Value in range (0, 124)

    //Iterate through all sorted suffixes to map first suffix with this_F_column_idx'th F-value into fm_index->row_map
    while(this_suffix_idx<fm_index->n && this_F_column_idx <= 124){
        //printf("(%d)Comparing %d and %d\n", this_suffix_idx, encode_3chars_to_1byte(sorted_suffixes[this_suffix_idx], 0), OneByteEncoded[this_F_column_idx]);
        //If F-value of this suffix is same as F-value at this_F_column_idx, this is first such record and needs mapping in fm_index->row_map
        if(encode_3chars_to_1byte(sorted_suffixes[this_suffix_idx], 0) == OneByteEncoded[this_F_column_idx]){         
            fm_index->row_map[this_F_column_idx] = this_suffix_idx;//Map the record
            this_suffix_idx++;//Move to examine next suffixes
        }

        //Keep iterating till we encounter a new F-value
        while(this_suffix_idx<fm_index->n && encode_3chars_to_1byte(sorted_suffixes[this_suffix_idx], 0) == OneByteEncoded[this_F_column_idx]) 
            this_suffix_idx++;

        //Iteration paused. Check if new F-value found. Mapping would be performed for this F-value in fm_index->row_map if value found. 
        //Else map wil be -1 for this F-value.
        //Move to examine next suffixes
        if(this_suffix_idx>previous_suffix_idx){
            previous_suffix_idx = this_suffix_idx;
            this_F_column_idx = this_F_column_idx + 1;
        }
        else{
            fm_index->row_map[this_F_column_idx] = -1;
            this_F_column_idx = this_F_column_idx + 1;
            //printf("-1 initialization for row_map at idx %d\n", this_F_column_idx);
        }
        //printf("Done till %d with this_F_column_idx = %d\n", this_suffix_idx, this_F_column_idx);
    }

    //Since no suffixes exist for remaining F-values, store -1 for their fm_index->row_map's
    for(int i=this_F_column_idx;i<125;i++)
        fm_index->row_map[i] = -1;

#ifdef DEBUG
    printf("-----------------------------PRINTING VALID ROW-MAPS--------------------------\n");
    for(int i=0;i<125;i++){
        if(fm_index->row_map[i]>-1)
            printf("%d   %d   %d\n",i,OneByteEncoded[i],fm_index->row_map[i]);
    }
    printf("-------------------------------------------------------------------\n");
#endif
    
    //Encode L-values for each sorted suffix and store in fm_index->bwtString
    for(int i=0;i<fm_index->n;i++)
        fm_index->bwtString[i] = encode_3chars_to_1byte(sorted_suffixes[i], suffix_length-3);

#ifdef DEBUG_L_VALUES
    for(int i=0;i<fm_index->n;i++)
        printf("%d : %d\n",i,fm_index->bwtString[i]);
#endif
#ifdef DEBUG
    printf("--------------------Printing Occ_major:-------------------------\n");
#endif

    
    int d_major_count = fm_index->n/fm_index->d_major;//Count of occ_major records possible with fm_index->n suffixes
    int d_minor_count = fm_index->n/fm_index->d_minor - fm_index->n/fm_index->d_major;//Count of occ_minor records possible with fm_index->n suffixes

    //Calculate occ_major values
    for(int i=0;i<d_major_count;i++){
        for(int j=0;j<112;j++)
            fm_index->occ_major[i][j] = 0;
        for(int j=i*fm_index->d_major;j<i*fm_index->d_major+fm_index->d_major;j++){
            //if(fm_index->bwtString[j] == 111)
            //    printf("Increasing for 111 at %dth L, current value=%d\n", j, fm_index->occ_major[i][fm_index->bwtString[j]]);
            fm_index->occ_major[i][fm_index->bwtString[j]]++;
        }
        if(i>0){
            for(int j=0;j<112;j++)
                fm_index->occ_major[i][j] = fm_index->occ_major[i][j] + fm_index->occ_major[i-1][j];
        }

#ifdef DEBUG        
        printf("[------%dth major Occ-------]\n", i);
        for(int j=0;j<112;j++){
            if(fm_index->occ_major[i][j] != 0)
                printf("(%d)%d\t",j, fm_index->occ_major[i][j]);
        }
        printf("\n\n");
        printf("--------------------Printing Occ_major:-------------------------\n");
#endif
    }

    //Calculate occ_minor values
    for(int i=0;i<d_minor_count;i++){
        for(int j=0;j<112;j++)
            fm_index->occ_minor[i][j] = 0;
        int end_idx = fm_index->d_minor*(i + i/(fm_index->d_major/fm_index->d_minor-1) + 1);
        int start_idx = end_idx - fm_index->d_minor;
        for(int j=start_idx;j<end_idx;j++){
            fm_index->occ_minor[i][fm_index->bwtString[j]]++;
        }
#ifdef DEBUG        
        printf("[------%dth minor Occ-------]\n", i);
        for(int j=0;j<112;j++){
            if(fm_index->occ_minor[i][j] != 0)
                printf("(%d)%d\t",j, fm_index->occ_minor[i][j]);
        }
        printf("\n\n");
#endif
    }
}

/*----------------------------------Index Creation functions end------------------------------------------------*/

/*----------------------------------Query functions start------------------------------------------------*/

//Initializes search range for the first F-value in kmer (last 3-chars of the query kmer). These values will be in terms of counts instead of actual array indexes
void init_search_interval(ThreeStepFMIndex *fm_index, unsigned char F_term, int *search_start, int *search_end){
    *search_start = fm_index->row_map[F_term];//Get the array index of the first suffix with F-value = F_term
    //printf("Initialised value = %d\n", *search_start);
    if(*search_start != -1){
        *search_end = 0;
        int i = F_term + 1;
		while(fm_index->row_map[i]==-1)//Iterate till we find next valid row_map. This would mark start of suffixes with next F-value
            i++;

        //Get the array index of the first suffix with F-value = next F_term
        if(i<124)
            *search_end = fm_index->row_map[i];
        else
            *search_end = fm_index->n;

        //Normalize range in gterms of counts, instead of actual indexes
        *search_end = *search_end - *search_start - 1;
        *search_start = 0;
    }
    else{
        *search_start = -1;
        *search_end = -1;
    }
}

//Find the pre-calculated occ value 'count_at_start_idx' (using occ_major and occ_minor values) nearest to target_idx. Also return the 'search_start_idx' for the search_start_idx'th suffix where values were pre-calculated
void find_nearest_Occ(ThreeStepFMIndex *fm_index, unsigned char L_term, int target_idx, int *search_start_idx, int *count_at_start_idx){
    int d_minor_step = target_idx/fm_index->d_minor;
    int nearest_d_major_start = target_idx/fm_index->d_major - 1;//The array index of first major_occ value at or before the target_idx
    int nearest_d_minor_start = (nearest_d_major_start+1)*3 + (d_minor_step%4 == 3 ? 2 : d_minor_step%4);//The array index of first minor_occ value at or before the target_idx
    int start_idx_record_lower = (nearest_d_minor_start + nearest_d_minor_start/3 + 1)*fm_index->d_minor;

    //Calculate the count value at nearest_d_major_start using major_occ values
    int count_start_record_lower = nearest_d_major_start > -1 ? fm_index->occ_major[nearest_d_major_start][L_term] : 0;
    int max_limit = d_minor_step%4 == 3 ? 2 : d_minor_step%4;

    //Calculate the count value at start_idx_record_lower-1 using major_occ and minor_occ values
    for(int i=0;i<=max_limit;i++){
        //printf("%dth increment\n", i);
        count_start_record_lower += fm_index->occ_minor[(nearest_d_major_start+1)*3 + i][L_term];
    }
    //printf("step = %d for %d. nearest_d_major_start=%d nearest_d_minor_start=%d. count_start_record_lower=%d at start_idx_record_lower=%d\n", d_minor_step, temp_start_result, nearest_d_major_start, nearest_d_minor_start, count_start_record_lower, start_idx_record_lower, start_idx_record_lower);
    *search_start_idx = start_idx_record_lower-1;
    *count_at_start_idx = count_start_record_lower;
}


//Update search intervals search_start and search_end for every new F_term and L_term values from the previous search interval. Also store exact count occ values at those search interval limits
void update_search_interval(ThreeStepFMIndex *fm_index, unsigned char F_term, unsigned char L_term, int *search_start, int *search_end, int *count_lower_limit, int *count_upper_limit){
    int start_idx = fm_index->row_map[F_term] + *search_start;//First suffix for F-value = F_term to be searched
    int end_idx = fm_index->row_map[F_term] + *search_end;//Last suffix for F-value = F_term to be searched
    //printf("Inside : %d to %d\n",start_idx, end_idx);

    //Iterate the L-values in the search interval to find the first suffix (start_idx) with L-value = L_term
    while(start_idx <= end_idx && fm_index->bwtString[start_idx] != L_term){
        //printf("%d No match: %02x %02x\n", start_idx, fm_index->bwtString[start_idx], L_term);
        start_idx++;
    }
    int temp_start_result, temp_end_result;
    if(start_idx <= end_idx){
        //Check if this suffix's L-value is really L_term
        temp_start_result = fm_index->bwtString[start_idx] == L_term ? start_idx : -1;
        temp_end_result = temp_start_result;
    }
    else{
        *search_start = -1;
        *search_end = -1;
        return;
    }

    //Iterate the L-values in the search interval to find the last suffix (end_idx) with L-value = L_term
    while(start_idx <= end_idx && fm_index->bwtString[end_idx] != L_term)
        end_idx--;

    temp_end_result = (start_idx < end_idx && fm_index->bwtString[end_idx] == L_term) ? end_idx : temp_start_result;
    //printf("Before nearest Occ: %d to %d\n", temp_start_result, temp_end_result);

    int nearest_lower_limit_idx = 0, nearest_upper_limit_idx = 0;
    //FInd pre-calculated occ values (count_lower_limit, count_lower_limit) nearest to suffix ith array index temp_start_result and temp_end_result
    find_nearest_Occ(fm_index, L_term, temp_start_result, &nearest_lower_limit_idx, count_lower_limit);
    find_nearest_Occ(fm_index, L_term, temp_end_result, &nearest_upper_limit_idx, count_upper_limit);
    //printf("After nearest Occ count: %d to %d\n", *count_lower_limit, *count_upper_limit);

    //Add the count values for remaining suffixes after pre-calculated values till the target suffix at array index temp_start_result
    for(int i=nearest_lower_limit_idx+1; i<=temp_start_result;i++)
        *count_lower_limit += L_term == fm_index->bwtString[i] ? 1 : 0;

    //Add the count values for remaining suffixes after pre-calculated values till the target suffix at array index temp_end_result
    for(int i=nearest_upper_limit_idx+1; i<=temp_end_result;i++)
        *count_upper_limit += L_term == fm_index->bwtString[i] ? 1 : 0;
    //printf("Final nearest Occ count: %d to %d\n", *count_lower_limit, *count_upper_limit);
    *search_start = temp_start_result;
    *search_end = temp_end_result;
}

void query_3step_fm_index(ThreeStepFMIndex *fm_index, char *kmer, int kmer_length, int *lower_limit, int *upper_limit){
    unsigned char F_term = encode_3chars_to_1byte_wo_normalization(kmer, kmer_length-3);//First F-value. Last 3-chars in kmer
    unsigned char L_term = encode_3chars_to_1byte(kmer, kmer_length-6);//First L-value. Second-to-last 3-chars in kmer
    int search_rounds = (kmer_length+2)/3 - 1;//Number of search rounds required to cover whole kmer
    int search_start = 0;
    int search_end = 0;
    init_search_interval(fm_index, F_term, &search_start, &search_end);//Initialise search interval according to F_term
    int count_lower_limit = 0, count_upper_limit = 0;
    int i = 0;

    //Run 3-char search rounds to cover whole kmer search
    for(i=1;i<=search_rounds;i++){
        //Data Structure Fix : Map values to 0-111
        F_term = encode_3chars_to_1byte_wo_normalization(kmer, kmer_length-3*i);//Since fm_index->row_map values currently map to 0-124 instead of 0-111
        L_term = encode_3chars_to_1byte(kmer, kmer_length-3*i-3);//Since fm_index->bwtString values are by default encoded to 0-111
        if(i>1){
            //printf("Before search limit update : map_result=%d limit : %d to %d\n", fm_index->row_map[F_term], count_lower_limit, count_upper_limit);
            search_start = count_lower_limit -1;
            search_end = count_upper_limit -1;
            //printf("Updated search limits : %d to %d\n", search_start, search_end);
        }
        if(search_start !=-1 && search_end !=-1){
            update_search_interval(fm_index, F_term, L_term, &search_start, &search_end, &count_lower_limit, &count_upper_limit);
            //printf("Result limits : %d to %d with counts %d and %d\n", search_start, search_end, count_lower_limit, count_upper_limit);
        }
        else{
            break;
        }
    }
    if(i>search_rounds && search_start !=-1 && search_end !=-1){
        search_start = fm_index->row_map[encode_3chars_to_1byte_wo_normalization(kmer, 0)];
        search_end = search_start + count_lower_limit -1;
        search_start += count_upper_limit -1;
    }
    *lower_limit = search_start;
    *upper_limit = search_end;
}

/*----------------------------------Query functions end------------------------------------------------*/



//--------------------------------Not used for 3-step FM Index---------------------------------------
void encode_8chars_to_3bytes(char str[], char *result){
    result[0] &= 0x00;
    result[1] &= 0x00;
    result[2] &= 0x00;

    result[0] |= str[0]=='A' ? 0x20 : str[0]=='C' ? 0x40  : str[0]=='G' ? 0x60 : str[0]=='T' ? 0x80 : 0x00;
    result[0] |= str[1]=='A' ? 0x04 : str[1]=='C' ? 0x08  : str[1]=='G' ? 0x0C : str[1]=='T' ? 0x10 : 0x00;

    if(str[2]=='A'){
        result[1] |= 0x80;
    }
    else if(str[2]=='C'){
        result[0] |= 0x01;
    }
    else if(str[2]=='G'){
        result[0] |= 0x01;
        result[1] |= 0x80;
    }
    else if(str[2]=='T'){
        result[0] |= 0x02;
    }

    result[1] |= str[3]=='A' ? 0x10 : str[3]=='C' ? 0x20  : str[3]=='G' ? 0x30 : str[3]=='T' ? 0x40 : 0x00;
    result[1] |= str[4]=='A' ? 0x02 : str[4]=='C' ? 0x04  : str[4]=='G' ? 0x05 : str[4]=='T' ? 0x08 : 0x00;

    if(str[5]=='A'){
        result[2] |= 0x40;
    }
    else if(str[5]=='C'){
        result[2] |= 0x80;
    }
    else if(str[5]=='G'){
        result[2] |= 0xC0;
    }
    else if(str[5]=='T'){
        result[1] |= 0x01;
    }

    result[2] |= str[6]=='A' ? 0x08 : str[6]=='C' ? 0x10  : str[6]=='G' ? 0x18 : str[6]=='T' ? 0x20 : 0x00;
    result[2] |= str[7]=='A' ? 0x01 : str[7]=='C' ? 0x02  : str[7]=='G' ? 0x03 : str[7]=='T' ? 0x04 : 0x00;
}
