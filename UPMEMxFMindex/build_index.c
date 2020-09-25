#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_all_suffixes(char **suffixes, int *suffix_count, char str[], int str_length) 
{ 
    int n = str_length; 

    char temp[2*n + 1]; 
    strcpy(temp, str); 
    strcat(temp, str); 
    int k = 0;

    for (int i = 0; i < n; i++) 
    {
        k = 0; 
        for (int j=0; j != n; j++){ 
            suffixes[*suffix_count][k++] = temp[i + j];
        }
        (*suffix_count) = (*suffix_count)+1;
    }
}

// comparator function for qsort function
static int myCompare(const void* a, const void* b) 
{ 
    return strcmp(*(const char**)a, *(const char**)b); 
}

// sorting function
void sort(const char* arr[], int n) 
{ 
    qsort(arr, n, sizeof(const char*), myCompare); 
} 



int main()
{


    return 0;
}