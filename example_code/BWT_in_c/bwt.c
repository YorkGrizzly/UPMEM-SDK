/*
* String search using the Burrows-Wheeler transform in C
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#define VERSION "0.0.1"
#define MAX_LEN 2048
#define MAX_FILE_LEN 8192

static int mode_bwt_file;
static int mode_ibwt_file;
static int mode_bwt_stream;
static int mode_ibwt_stream;
static int show_version;
static int show_help;

typedef struct {
  int num;
  char str[MAX_LEN];
} Pair;

/*
* compare string
*
*/
int compare(const void *x, const void *y) {
  return strcmp(((Pair *)x)->str, ((Pair *)y)->str);
  // return strncmp(((Pair*)x)->str ,((Pair*)y)->str);
}

/*
* create_suffix_array
*
*/
static void create_suffix_array(char *str, int len, char *L, int *SA) {
  // add $ -> len+1
  Pair p[len + 1];
  char strs[len + 1];
  sprintf(strs, "%s%s", str, "$");
  char tmp;

  // create_char_pair
  printf("-- Before Sort --\n");
  for (int i = 0; i < len + 1; i++) {
    p[i].num = i;
    const char *ctmp = strs;
    strcpy(p[i].str, ctmp);

    // shift
    /*tmp = strs[len];
    int j;
    for (j = len; j > 0; j--) {
      strs[j] = strs[j - 1];
    }
    strs[j] = tmp;*/
    
    tmp = strs[0];
    int j;
    for (j = 0; j  < len; j++) {
      strs[j] = strs[j + 1];
    }
    strs[j] = tmp;
    
    printf("{num: %2d, char: '%s'\n", p[i].num, p[i].str);
  }

  printf("-- Quick Sort --\n");
  // quick_sort
  qsort(p, len + 1, sizeof(Pair), compare);

  // create suffix array
  printf("-- After Sort --\n");

  for (int i = 0; i < len + 1; i++) {
    SA[i] = p[i].num;
    printf("{num: %2d, char: '%s'}\n", p[i].num, p[i].str);
    const char *ptr = &p[i].str[len];
    const char *preptr;

    if (i != 0) {
      preptr = &p[i - 1].str[len];
    } else {
      preptr = "";
    }

    // printf("%s:%s\n",ptr, preptr);

    // suffix add
    if (L == NULL) {
      sprintf(L, "%c", p[i].str[len]);
    } else {
      sprintf(L, "%s%c", L, p[i].str[len]);
    }
  }
}

/*
* TODO : create_inverse_suffix_array
*
*/
static void create_inverse_suffix_array(char *str, int len, char *L) {
  printf("-- starting --\n");
  Pair p[len];
  char strs[len];
  sprintf(strs, "%s", str);
  char tmp;

  // create_char_pair
  printf("-- Before Sort --\n");
  for (int i = 0; i < len; i++) {
    p[i].num = i;
    char a = strs[i];
    const char *s = &a;
    strcpy(p[i].str, s);

    printf("{num: %d, char: %c}\n", p[i].num, a);
  }

  printf("-- Quick Sort --\n");
  qsort(p, len, sizeof(Pair), compare);

  // create suffix array
  printf("-- After Sort --\n");
  int index, first = 0;

  // get index
  for (int i = 0; i < len; i++) {
    char a = p[i].str[0];
    if (a == '$') {
      first = i;
      break;
    }
  }

  for (int i = 0, index = first; i < len; i++) {
    L[i] = p[index].str[0];
    printf("{num: %d, char: %c}\n", p[index].num, p[index].str[0]);
    index = p[index].num;

    if (L == NULL) {
      sprintf(L, "%c", p[i].str[len]);
    } else {
      sprintf(L, "%s%c", L, p[i].str[len]);
    }
  }
}

/*
* get_options
*
*/
static int get_options(int argc, char *const *argv) {
  int c;
  while ((c = getopt(argc, argv, "t:i:s:r:vh")) != -1) {
    switch (c) {
      case 't':
        printf("{option: '%c', mode: 'Burrows-Wheeler Transform', input_file: '%s'}\n", optopt, optarg);
        mode_bwt_file = 1;
        return 0;

      case 'i':
        printf("{option: '%c', mode: 'Inverse Burrows-Wheeler Transform', input_file: '%s'}\n", optopt, optarg);
        mode_ibwt_file = 1;
        return 0;

      case 's':
        printf("{option: '%c', mode: 'Burrows-Wheeler Transform', input_stream: '%s'}\n", optopt, optarg);
        mode_bwt_stream = 1;
        return 0;

      case 'r':
        printf("{option: '%c', mode: 'Inverse Burrows-Wheeler Transform', input_stream: '%s'}\n", optopt, optarg);
        mode_ibwt_stream = 1;
        return 0;

      case 'v':
        show_version = 1;
        return 0;

      case 'h':
        show_help = 1;
        return 0;

      default:
      return -1;
    }
  }
  return -2;
}

/*
* main
*
*/
int main(int argc, char **argv) {
  // Parse options
  if (get_options(argc, argv) != 0) {
    printf("[Error] invalid option.\n");
    return 1;
  }

  // Read File
  FILE *fp;
  char s[MAX_FILE_LEN];
  int len;

  if (mode_bwt_file || mode_ibwt_file) {
    if ((fp = fopen(optarg, "r")) == NULL) {
      printf("[Error] errno %d\n", errno);
      return errno;
    }
    while (fgets(s, MAX_FILE_LEN, fp) != NULL) {
      len = strlen(s);
      printf("{input: %s, length: %d}\n", s, len);
    }
    fclose(fp);
  }

  char L[len];
  memset(&L, 0, sizeof(L));
  
  int OCC[len+1][4];
  memset(&OCC, 0, sizeof(OCC));
  
  int SA[len + 1];
  memset(&SA, 0, sizeof(SA));
  
  if (show_help) {
    printf("\nOptions:\n");
    printf("  -t :  Burrows-Wheeler Transform. Input File.\n");
    printf("  -i :  Inverse Burrows-Wheeler Transform. Input File.\n");
    printf("  -s :  Burrows-Wheeler Transform. Input Stream.\n");
    printf("  -r :  Inverse Burrows-Wheeler Transform. Input Stream.\n");
    printf("  -v :  Show Version.\n");
    printf("  -h :  Show Help.\n");
    return 0;
  }

  if (show_version) {
    printf("\nVersion:\n%s\n", VERSION);
    return 0;
  }

  if (mode_bwt_file) {
    create_suffix_array(s, len, L, SA);
  }

  if (mode_ibwt_file) {
    create_inverse_suffix_array(s, len, L);
  }

  if (mode_bwt_stream) {
    create_suffix_array(optarg, strlen(optarg), L, SA);
  }

  if (mode_ibwt_stream) {
    create_inverse_suffix_array(optarg, strlen(optarg), L);
  }
  
    int OCC_A = 0;
    int OCC_C = 0;
    int OCC_G = 0;
    int OCC_T = 0;
  
  if(mode_bwt_file || mode_bwt_stream){
  	for(int i = 0; i <= len; i++){
		if(L[i] == 'A'){
      OCC_A = OCC_A + 1;
    }
    else if(L[i] == 'C'){
      OCC_C = OCC_C + 1;
    }
    else if(L[i] == 'G'){
      OCC_G = OCC_G + 1;
    }
    else if(L[i] == 'T'){
      OCC_T = OCC_T + 1;
    }
    
    OCC[i][0] = OCC_A;
    OCC[i][1] = OCC_C;
    OCC[i][2] = OCC_G;
    OCC[i][3] = OCC_T;
	}
  } 
  
  
  //printf("-- Result --\n%s\n", L);
  //printf("OCC table:\n");
  printf("SA   L   OCC_A   OCC_C   OCC_G   OCC_T\n");
  for(int i = 0; i <= len; i++){
    printf("%2d   %c    %2d      %2d      %2d      %2d\n", SA[i], L[i], OCC[i][0], OCC[i][1], OCC[i][2], OCC[i][3]);
  }
  
  
  FILE *f1 = fopen("dpu1.txt", "w");
  if(f1 == NULL){
	printf("File Opening Error!");
  }
  fwrite(&L[1], sizeof(char), OCC_A, f1);
  fclose(f1);
  
  
  FILE *f2 = fopen("dpu2.txt", "w");
  if(f2 == NULL){
	printf("File Opening Error!");
  }
  fwrite(&L[OCC_A+1], sizeof(char), OCC_C, f2);
  fclose(f2);
  
  FILE *f3 = fopen("dpu3.txt", "w");
  if(f3 == NULL){
	printf("File Opening Error!");
  }
  fwrite(&L[OCC_A+OCC_C+1], sizeof(char), OCC_G, f3);
  fclose(f3);
  
  FILE *f4 = fopen("dpu4.txt", "w");
  if(f4 == NULL){
	printf("File Opening Error!");
  }
  fwrite(&L[OCC_A+OCC_C+OCC_G+1], sizeof(char), OCC_T, f4);
  fclose(f4);
  return 0;
}
