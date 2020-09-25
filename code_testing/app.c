#include<stdlib.h>
#include<stdio.h>
int main()
{
    int num = 46;
    int *ptr = &num;
    *ptr = *ptr + 1;
    printf("%d\n", *ptr);
    return 0;
}
