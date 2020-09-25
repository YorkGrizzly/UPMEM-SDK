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
static void create_suffix_array(char *str, int len, char *F, char *L, int *SA) {
  // add $ -> len+1
  Pair p[len + 1];
  char strs[len + 1];
  sprintf(strs, "%s%s", str, "$");
  char tmp;

  // create_char_pair
  //printf("-- Before Sort --\n");
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
    
    //printf("{num: %2d, char: '%s'\n", p[i].num, p[i].str);
  }

  //printf("-- Quick Sort --\n");
  // quick_sort
  qsort(p, len + 1, sizeof(Pair), compare);

  // create suffix array
  //printf("-- After Sort --\n");

  for (int i = 0; i < len + 1; i++) {
    SA[i] = p[i].num;
    //printf("{num: %2d, char: '%s'}\n", p[i].num, p[i].str);
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

    if (F == NULL) {
      sprintf(F, "%c", p[i].str[0]);
    } else {
      sprintf(F, "%s%c", F, p[i].str[0]);
    }
  }
}

/*
* get_options
*
*/
static int get_options(int argc, char *const *argv) {
  int c;
  while ((c = getopt(argc, argv, "t:s")) != -1) {
    switch (c) {
      case 't':
        //printf("{option: '%c', mode: 'Burrows-Wheeler Transform', input_file: '%s'}\n", optopt, optarg);
        mode_bwt_file = 1;
        return 0;

      case 's':
        //printf("{option: '%c', mode: 'Burrows-Wheeler Transform', input_stream: '%s'}\n", optopt, optarg);
        mode_bwt_stream = 1;
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
      //printf("{input: %s, length: %d}\n", s, len);
    }
    fclose(fp);
  }
  char F[len+1];
  memset(&F, 0, sizeof(F));

  char L[len+1];
  memset(&L, 0, sizeof(L));
  
  int OCC[len+1][4];
  memset(&OCC, 0, sizeof(OCC));

  int sample_rate = 40;

  int sampled_OCC[len/sample_rate + 1][4];
  memset(&sampled_OCC, 0, sizeof(sampled_OCC));
  
  int SA[len+1];
  memset(&SA, 0, sizeof(SA));
  
  /*if (show_help) {
    printf("\nOptions:\n");
    printf("  -t :  Burrows-Wheeler Transform. Input File.\n");
    printf("  -s :  Burrows-Wheeler Transform. Input Stream.\n");
    return 0;
  }

  if (show_version) {
    printf("\nVersion:\n%s\n", VERSION);
    return 0;
  }*/

  if (mode_bwt_file) {
    create_suffix_array(s, len, F, L, SA);
  }


  if (mode_bwt_stream) {
    create_suffix_array(optarg, strlen(optarg), F, L, SA);
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

    if(i % sample_rate == 0){
      for(int j = 0; j < 4; j++)
        sampled_OCC[i/sample_rate][j] = OCC[i][j];
    }
	}
  } 

  int A_offset = 1;
  int C_offset = 1 + OCC_A;
  int G_offset = 1 + OCC_A + OCC_C;
  int T_offset = 1 + OCC_A + OCC_C + OCC_G;
  
  
  //printf("-- Result --\n%s\n", L);
  //printf("OCC table:\n");
  /*printf("SA   F   L   OCC_A   OCC_C   OCC_G   OCC_T\n");
  for(int i = 0; i <= len; i++){
    printf("%2d   %c   %c    %2d      %2d      %2d      %2d\n", SA[i], F[i], L[i], OCC[i][0], OCC[i][1], OCC[i][2], OCC[i][3]);
  }
  printf("sampled OCC:\n");
  for(int i = 0; i <= len/sample_rate; i++){
    printf("%2d   %2d   %2d   %2d\n", sampled_OCC[i][0], sampled_OCC[i][1], sampled_OCC[i][2], sampled_OCC[i][3]);
  }
  printf("offset:\n");
  printf("%d   %d   %d   %d\n", A_offset, C_offset, G_offset, T_offset);*/
  
  


  //QUERYING
  int query_size = 9;
  int search_round = query_size - 1;
  int range_max, range_min;
  char query[query_size];
  strcpy(query, "ATCGCTACT");

  if(query[query_size - 1] == 'A'){
    range_min = A_offset;
    range_max = C_offset - 1;
  }
  else if(query[query_size - 1] == 'C'){
    range_min = C_offset;
    range_max = G_offset - 1;
  }
  else if(query[query_size - 1] == 'G'){
    range_min = G_offset;
    range_max = T_offset - 1;
  }
  else if(query[query_size - 1] == 'T'){
    range_min = T_offset;
    range_max = len;
  }

  //printf("initial range_min: %d, initial range_max: %d\n", range_min, range_max);


  
  int not_found_flag = 0;


  for(int i = 0; i < search_round; i++){
    //char target_F = query[query_size - 1 - i];
    char target_L = query[query_size - 2 - i];
    //printf("target_L: %c\n", target_L);
    int OCC_index;
    int offset;
    int update_range_min = -1;
    int update_range_max = -1;

    if(target_L == 'A'){
      OCC_index = 0;
      offset = A_offset;
    }
    else if(target_L == 'C'){
      OCC_index = 1;
      offset = C_offset;
    }
    else if(target_L == 'G'){
      OCC_index = 2;
      offset = G_offset;
    }
    else if(target_L == 'T'){
      OCC_index = 3;
      offset = T_offset;
    }

    for(int j = range_min; j <= range_max; j++){
      if(L[j] == target_L){
        if(j % sample_rate <= sample_rate/2){
          update_range_min = offset + sampled_OCC[j/sample_rate][OCC_index] - 1;
          for(int k = 1; k <= j % sample_rate; k++){
            if(L[sample_rate * (j/sample_rate) + k] == target_L){
              update_range_min += 1;
            }
          }
        }
        else if(j % sample_rate > sample_rate/2){
          update_range_min = offset + sampled_OCC[j/sample_rate + 1][OCC_index] - 1;
          for(int k = 0; k < sample_rate - j % sample_rate; k++){
            if(L[sample_rate * (j/sample_rate + 1) - k] == target_L){
              update_range_min -= 1;
            }
          }
        }
      }
      if(L[j] == target_L) break;
    }


    for(int j = range_max; j >= range_min; j--){
      if(L[j] == target_L){
        if(j % sample_rate <= sample_rate/2){
          update_range_max = offset + sampled_OCC[j/sample_rate][OCC_index] - 1;
          for(int k = 1; k <= j % sample_rate; k++){
            if(L[sample_rate * (j/sample_rate) + k] == target_L){
              update_range_max += 1;
            }
          }
        }
        else if(j % sample_rate > sample_rate/2){
          update_range_max = offset + sampled_OCC[j/sample_rate + 1][OCC_index] - 1;
          for(int k = 0; k < sample_rate - j % sample_rate; k++){
            if(L[sample_rate * (j/sample_rate + 1) - k] == target_L){
              update_range_max -= 1;
            }
          }
        }
      }
      if(L[j] == target_L) break;
    }

    if(update_range_min == -1 && update_range_max == -1){
      not_found_flag = 1;
      break;
    }

    range_min = update_range_min;
    range_max = update_range_max;
    //printf("updated range_min: %d, updated range_max: %d\n", update_range_min, update_range_max);

  }

  printf("position of the query:\n");
  if(not_found_flag == 0){
    for(int i = range_min; i <= range_max; i++){
      printf("%d  ", SA[i]);
    }
  }
  else printf("not found");
  printf("\n");
  /*FILE *f1 = fopen("dpu1.txt", "w");
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
  fclose(f4);*/
  return 0;
}
