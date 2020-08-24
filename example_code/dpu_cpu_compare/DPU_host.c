#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#ifndef DPU_BINARY
#define DPU_BINARY "DPU"
#endif

/* Size of the buffer for which we compute the checksum: 64KBytes. */
#define SIZE 10000
#define DPU_NUM 1

int main() {
  struct dpu_set_t set, dpu;
  
  uint32_t A[SIZE];
  unsigned long long int part_sum;
  unsigned long long int sum;
  sum = 0;
  uint8_t j = 0;
  for(int i = 0; i < SIZE; i++){
    A[i] = (uint32_t)i;
  }
  
  DPU_ASSERT(dpu_alloc(DPU_NUM, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  
  /*DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_copy_to(dpu, "A", 0, &A[SIZE / DPU_NUM * j], SIZE / DPU_NUM * 4));
    j = j + 1;
  }*/
  DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_copy_to(dpu, "A", 0, A, SIZE * 4));
    j = j + 1;
  }
  
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  DPU_FOREACH(set, dpu) {
    //DPU_ASSERT(dpu_log_read(dpu, stdout));
    DPU_ASSERT(dpu_copy_from(dpu, "sum", 0, &sum, sizeof(sum)));
    //sum = sum + part_sum;
  }
  DPU_ASSERT(dpu_free(set));
  printf("sum = %llu\n",sum);
  return 0;
}