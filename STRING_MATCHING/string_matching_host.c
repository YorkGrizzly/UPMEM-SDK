/* Communication with a DPU via the MRAM. */
/* Populate the MRAM with a collection of bytes and request the DPU to */
/* searching the string. */

#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <dpu_profiler.h>
#ifndef DPU_BINARY
#define DPU_BINARY "string_matching"
#endif

/* Size of the buffer for which we compute the checksum: 1MBytes. */
#define QUERY_SIZE 8
#define CORPUS_SIZE 262144

void populate_mram(struct dpu_set_t set) {
  uint8_t QUERY[QUERY_SIZE] = {'A', 'A', 'C', 'C', 'T', 'G', 'A', 'G'};
  // ASCII coded

  
  
  // QUERY[0] = (uint8_t)1;
  // QUERY[1] = (uint8_t)2;
  // QUERY[2] = (uint8_t)3;
  // QUERY[3] = (uint8_t)4;
// for (int byte_index = 0; byte_index < QUERY_SIZE; byte_index++) {
//     QUERY[byte_index] = (uint8_t)byte_index;
//   }

  DPU_ASSERT(dpu_copy_to(set, "QUERY", 0, QUERY, QUERY_SIZE));
}

int main() {
  FILE *f = fopen("corpus.txt", "r");
  if(f == NULL){
	printf("File Opening Error!");
  }

  // uint8_t query[QUERY_SIZE];
  // uint8_t QUERY[3] = {'A', 'A', 'C'};
  uint8_t CORPUS[CORPUS_SIZE];
  uint32_t count_onhost;
  uint32_t marker_onhost[64];
  int j = 0;
  
  
  struct dpu_set_t set, dpu;
  
  DPU_ASSERT(dpu_alloc(1, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  populate_mram(set);
  
  DPU_FOREACH(set, dpu) {
	  fgets(CORPUS, CORPUS_SIZE + 1, f);
    //printf("%s", CORPUS);
	  DPU_ASSERT(dpu_copy_to(dpu, "CORPUS", 0, CORPUS, CORPUS_SIZE));
  }
  
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  
  DPU_FOREACH(set, dpu)
  {
    DPU_ASSERT(dpu_log_read(dpu, stdout));
    DPU_ASSERT(dpu_copy_from(dpu, "count", 0, (uint32_t *)&count_onhost, sizeof(count_onhost)));
    DPU_ASSERT(dpu_copy_from(dpu, "marker", 0, (uint32_t *)& marker_onhost, sizeof(marker_onhost)));
    printf("counts: %d\n", count_onhost);
    printf("markers: ");
    for(size_t i = 0; i < 64; i++){
      if(marker_onhost[i] != -1) printf("%d ", j * CORPUS_SIZE + marker_onhost[i]);
    }
    printf("\n");
    j = j + 1;
  }
  /*printf("Markers:\n");
  for(size_t i = 0; i < 64; i++)
  {
    printf("%d ", marker_onhost[i]);
  }
  printf("\n");
  /*
  DPU_FOREACH(set, dpu) {
    
    printf("Computed matching number = %d\n", count);
  }
  */
  DPU_ASSERT(dpu_free(set));
  return 0;
}