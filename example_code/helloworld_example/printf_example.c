#include <stdio.h>

int main() {
  int i;
  for (i = 0; i < 32; i++) {
    printf("hello %02d=0x%02x times\n", i, i);
  }
  return 0;
}
