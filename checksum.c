#include <mram.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <perfcounter.h>
#define CACHE_SIZE 8
#define BUFFER_SIZE 8

__mram_noinit uint32_t buffer[BUFFER_SIZE];
__host uint32_t checksum;
__dma_aligned uint32_t local_cache[CACHE_SIZE];
int main() {
  checksum = 0;
  perfcounter_t initial_time = perfcounter_config(COUNT_CYCLES, false);
  for (unsigned int bytes_read = 0; bytes_read < BUFFER_SIZE;) {
    mram_read(&buffer[bytes_read], &local_cache, CACHE_SIZE * 4);
    
    for (unsigned int byte_index = 0; (byte_index < CACHE_SIZE) && (bytes_read < BUFFER_SIZE); byte_index++, bytes_read++) {
      checksum -= (uint32_t)local_cache[byte_index];
    }
  }
  perfcounter_t duration = perfcounter_get() - initial_time;
  printf("%lu\n", duration); 
  return checksum;
}