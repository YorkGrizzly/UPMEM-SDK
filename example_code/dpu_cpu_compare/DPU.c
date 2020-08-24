#include <mram.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define SIZE 10000

__mram_noinit uint32_t A[SIZE];
__host unsigned long long int sum;

int main(){
  sum = 0;

  for(int i = 0; i < SIZE; i++){
    sum += A[i];
  }
  //printf("%llu\n",sum);
  return 0;
}