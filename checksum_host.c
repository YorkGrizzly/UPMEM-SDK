/* Communication with a DPU via the MRAM. */
/* Populate the MRAM with a collection of bytes and request the DPU to */
/* compute the checksum. */

#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#ifndef DPU_BINARY
#define DPU_BINARY "checksum"
#endif

/* Size of the buffer for which we compute the checksum: 64KBytes. */
#define BUFFER_SIZE 8

void populate_mram(struct dpu_set_t set) {
  uint32_t buffer[BUFFER_SIZE];

  for (int byte_index = 0; byte_index < BUFFER_SIZE; byte_index++) {
    buffer[byte_index] = (uint32_t)byte_index;
  }
  DPU_ASSERT(dpu_copy_to(set, "local_cache", 0, buffer, BUFFER_SIZE * 4));
}
void populate_mram1(struct dpu_set_t set) {
  uint32_t buffer[BUFFER_SIZE];

  for (int byte_index = 0; byte_index < BUFFER_SIZE; byte_index++) {
    buffer[byte_index] = (uint32_t)byte_index *(uint32_t)2;
  }
  DPU_ASSERT(dpu_copy_to(set, "local_cache", 0, buffer, BUFFER_SIZE * 4));
}

int main() {
  struct dpu_set_t set1, dpu1;
  struct dpu_set_t set2, dpu2;
  uint32_t checksum1;
  uint32_t checksum2;
  DPU_ASSERT(dpu_alloc(1, NULL, &set1));
  DPU_ASSERT(dpu_load(set1, DPU_BINARY, NULL));
  populate_mram(set1);

  DPU_ASSERT(dpu_launch(set1, DPU_SYNCHRONOUS));
  DPU_FOREACH(set1, dpu1) {
    DPU_ASSERT(dpu_copy_from(dpu1, "checksum", 0, (uint32_t *)&checksum1, sizeof(checksum1)));
    printf("Computed checksum1 = %d\n", checksum1);
  }
  DPU_ASSERT(dpu_free(set1));
  
  DPU_ASSERT(dpu_alloc(1, NULL, &set2));
  DPU_ASSERT(dpu_load(set2, DPU_BINARY, NULL));
  populate_mram1(set2);

  DPU_ASSERT(dpu_launch(set2, DPU_SYNCHRONOUS));
  DPU_FOREACH(set2, dpu2) {
    DPU_ASSERT(dpu_copy_from(dpu2, "checksum", 0, (uint32_t *)&checksum2, sizeof(checksum2)));
    printf("Computed checksum2 = %d\n", checksum2);
  }
  DPU_ASSERT(dpu_free(set2));
  return 0;
}