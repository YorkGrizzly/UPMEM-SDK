#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define SIZE 10000

int main(){
  uint32_t A[SIZE];
  long long int sum;
  
  sum = 0;
  for(int i = 0; i < SIZE; i++){
    A[i] = (uint32_t)i;
  }
  for(int i = 0; i < SIZE; i++){
    sum += A[i];
  }
  printf("sum = %llu\n", sum);
}