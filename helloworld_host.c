#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>

#ifndef DPU_BINARY
#define DPU_BINARY "./helloworld"
#endif

int main(void) {
  struct dpu_set_t set, rank, dpu;
  uint32_t nr_ranks;
  uint32_t nr_dpus;

// individual DPU task assignment...
//upload

  // DPU_ASSERT handles errors in the DPU API and exits in case of error
  DPU_ASSERT(dpu_alloc(64, NULL, &rank)); // allocates a set of UPMEM dpu ranks
  
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL)); // reads and loads binary executables into the allocated DPU set
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS)); // starts execution of program
  //DPU_ASSERT(dpu_get_nr_ranks(set, nr_ranks));
  DPU_RANK_FOREACH(rank, dpu) { // iterate over the individual DPUs from the allocated set
    DPU_ASSERT(dpu_log_read(dpu, stdout)); // fetches the DPU stdout buffer and display it on the host stdout
    DPU_ASSERT(dpu_get_nr_ranks(set, &nr_ranks));
    DPU_ASSERT(dpu_get_nr_dpus(dpu, &nr_dpus));
    printf("%u %u\n", nr_ranks, nr_dpus);

  }

  DPU_ASSERT(dpu_free(set)); // when execution completes, the allocated DPU set must be free

  return 0;
}
