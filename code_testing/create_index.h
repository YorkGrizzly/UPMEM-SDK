//#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include "fm_index.h"

//using namespace std;

void generate_all_suffixes(char **suffixes, int *suffix_count, char str[], int str_length)
{ // suffixes are all the rotated strings
    int n = str_length;

    char temp[2 * n + 1];
    strcpy(temp, str); // copies string from str to temp
    strcat(temp, str); // appends str to the end of temp
    int k = 0;

    for (int i = 0; i < n; i++)
    {
        k = 0;
        for (int j = 0; j != n; j++)
        {
            suffixes[*suffix_count][k++] = temp[i + j];
        }
        (*suffix_count) = (*suffix_count) + 1;
    }
}

// Defining comparator function as per the requirement
static int myCompare(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

// Function to sort the array
void sort(const char *arr[], int n)
{
    // calling qsort function to sort the array
    // with the help of Comparator
    qsort(arr, n, sizeof(const char *), myCompare);
}
