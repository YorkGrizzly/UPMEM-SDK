#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>

#ifndef DPU_BINARY
#define DPU_BINARY "./helloworld"
#endif

int main(void) {
  struct dpu_set_t set, rank, dpu;
  uint32_t nr_ranks1, nr_ranks2;
  uint32_t nr_dpus1, nr_dpus2;

// individual DPU task assignment...
//upload

  // DPU_ASSERT handles errors in the DPU API and exits in case of error
  DPU_ASSERT(dpu_alloc(10, NULL, &set)); // allocates a set of UPMEM dpu ranks
  //DPU_ASSERT(dpu_alloc(10, NULL, &rank));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL)); // reads and loads binary executables into the allocated DPU set
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS)); // starts execution of program
  DPU_RANK_FOREACH(set, rank) { // iterate over the individual DPUs from the allocated set
    //DPU_ASSERT(dpu_log_read(dpu, stdout)); // fetches the DPU stdout buffer and display it on the host stdout
    DPU_ASSERT(dpu_get_nr_ranks(set, &nr_ranks1));
    DPU_ASSERT(dpu_get_nr_dpus(set, &nr_dpus1));
    DPU_ASSERT(dpu_get_nr_ranks(rank, &nr_ranks2));
    DPU_ASSERT(dpu_get_nr_dpus(rank, &nr_dpus2));
    printf("%u %u %u %u\n", nr_ranks1, nr_dpus1, nr_ranks2, nr_dpus2);

  }

  DPU_ASSERT(dpu_free(set)); // when execution completes, the allocated DPU set must be free

  return 0;
}
