#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_FILE_LEN 8192


typedef struct {
  int num;
  char str[MAX_FILE_LEN];
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
    F[i] = p[i].str[0];
    L[i] = p[i].str[len];
    //printf("{num: %2d, char: '%s'}\n", p[i].num, p[i].str);

    // suffix add
    // if (L == NULL) {
    //   sprintf(L, "%c", p[i].str[len]);
    // } else {
    //   sprintf(L, "%s%c", L, p[i].str[len]);
    // }

    // if (F == NULL) {
    //   sprintf(F, "%c", p[i].str[0]);
    // } else {
    //   sprintf(F, "%s%c", F, p[i].str[0]);
    // }
  }
}

/*
* main
*
*/
int main() {
  // Read File
  char s[MAX_FILE_LEN];
  int len;

  FILE *f = fopen("input.txt", "r");
  if(f == NULL){
	printf("File Opening Error!");
  }
  fgets(s, MAX_FILE_LEN + 1, f);
  len = strlen(s);
  fclose(f);

  // char F[len+1];
  // memset(&F, 0, sizeof(F));

  // char L[len+1];
  // memset(&L, 0, sizeof(L));
  
  // int OCC[len+1][4];
  // memset(&OCC, 0, sizeof(OCC));

  int sample_rate = 20;

  // int sampled_OCC[len/sample_rate + 1][4];
  // memset(&sampled_OCC, 0, sizeof(sampled_OCC));

  
  
  // int SA[len+1];
  // memset(&SA, 0, sizeof(SA));


  char *F;
  F = (char *) malloc((len + 1) * sizeof(char));

  char *L;
  L = (char *) malloc((len + 1) * sizeof(char));

  int **OCC;
  OCC = (int **) malloc((len + 1) * sizeof(int *));
  for(int i = 0; i < len + 1; i++){
    OCC[i] = (int *) malloc(4 * sizeof(int));
  }

  int *SA;
  SA = (int *) malloc((len + 1) * sizeof(int));

  int **sampled_OCC;
  sampled_OCC = (int **) malloc((len/sample_rate + 1) * sizeof(int *));
  for(int i = 0; i < len/sample_rate + 1; i++){
    sampled_OCC[i] = (int *) malloc(4 * sizeof(int));
  }
  
 
  create_suffix_array(s, len, F, L, SA);
  


  
  int OCC_A = 0;
  int OCC_C = 0;
  int OCC_G = 0;
  int OCC_T = 0;
  
  
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

  int A_offset = 1;
  int C_offset = 1 + OCC_A;
  int G_offset = 1 + OCC_A + OCC_C;
  int T_offset = 1 + OCC_A + OCC_C + OCC_G;

  int sampled_OCC_A[OCC_A/sample_rate + 1][4];
  memset(&sampled_OCC_A, 0, sizeof(sampled_OCC_A));

  int sampled_OCC_C[(OCC_C - 1)/sample_rate + 1][4];
  memset(&sampled_OCC_C, 0, sizeof(sampled_OCC_C));

  int sampled_OCC_G[(OCC_G - 1)/sample_rate + 1][4];
  memset(&sampled_OCC_G, 0, sizeof(sampled_OCC_G));

  int sampled_OCC_T[(OCC_T - 1)/sample_rate + 1][4];
  memset(&sampled_OCC_T, 0, sizeof(sampled_OCC_T));


  for(int i = 0; i < C_offset; i++){
    int j = i - 0;
    if(j % sample_rate == 0){
      for(int k = 0; k < 4; k++){
        sampled_OCC_A[j/sample_rate][k] = OCC[i][k];
      }
    }
  }

  for(int i = C_offset; i < G_offset; i++){
    int j = i - C_offset;
    if(j % sample_rate == 0){
      for(int k = 0; k < 4; k++){
        sampled_OCC_C[j/sample_rate][k] = OCC[i][k];
      }
    }
  }

  for(int i = G_offset; i < T_offset; i++){
    int j = i - G_offset;
    if(j % sample_rate == 0){
      for(int k = 0; k < 4; k++){
        sampled_OCC_G[j/sample_rate][k] = OCC[i][k];
      }
    }
  }

  for(int i = T_offset; i < len + 1; i++){
    int j = i - T_offset;
    if(j % sample_rate == 0){
      for(int k = 0; k < 4; k++){
        sampled_OCC_T[j/sample_rate][k] = OCC[i][k];
      }
    }
  }



  FILE *L_table;
  FILE *SA_table;
  FILE *sampled_OCC_table;

  L_table = fopen("L_table.txt", "w");
  fwrite(L, sizeof(char), len + 1, L_table);

  SA_table = fopen("SA_table.txt", "w");
  for(int i = 0; i < len + 1; i++){
    fprintf(SA_table, "%d ", SA[i]);
  }

  sampled_OCC_table = fopen("sampled_OCC_table.txt", "w");
  for(int i = 0; i < len/sample_rate + 1; i++){
    for(int j = 0; j < 4; j++){
      fprintf(sampled_OCC_table, "%d ", sampled_OCC[i][j]);
    }
  }


  FILE *L_A_table;
  FILE *sampled_OCC_A_table;

  L_A_table = fopen("L_A_table.txt", "w");
  fwrite(&L[0], sizeof(char), OCC_A + 1, L_A_table);

  sampled_OCC_A_table = fopen("sampled_OCC_A_table.txt", "w");
  for(int i = 0; i < OCC_A/sample_rate + 1; i++){
    for(int j = 0; j < 4; j++){
      fprintf(sampled_OCC_A_table, "%d ", sampled_OCC_A[i][j]);
    }
  }

  FILE *L_C_table;
  FILE *sampled_OCC_C_table;

  L_C_table = fopen("L_C_table.txt", "w");
  fwrite(&L[C_offset], sizeof(char), OCC_C, L_C_table);

  sampled_OCC_C_table = fopen("sampled_OCC_C_table.txt", "w");
  for(int i = 0; i < (OCC_C - 1)/sample_rate + 1; i++){
    for(int j = 0; j < 4; j++){
      fprintf(sampled_OCC_C_table, "%d ", sampled_OCC_C[i][j]);
    }
  }

  FILE *L_G_table;
  FILE *sampled_OCC_G_table;

  L_G_table = fopen("L_G_table.txt", "w");
  fwrite(&L[G_offset], sizeof(char), OCC_G, L_G_table);

  sampled_OCC_G_table = fopen("sampled_OCC_G_table.txt", "w");
  for(int i = 0; i < (OCC_G - 1)/sample_rate + 1; i++){
    for(int j = 0; j < 4; j++){
      fprintf(sampled_OCC_G_table, "%d ", sampled_OCC_G[i][j]);
    }
  }

  FILE *L_T_table;
  FILE *sampled_OCC_T_table;

  L_T_table = fopen("L_T_table.txt", "w");
  fwrite(&L[T_offset], sizeof(char), OCC_T, L_T_table);

  sampled_OCC_T_table = fopen("sampled_OCC_T_table.txt", "w");
  for(int i = 0; i < (OCC_T - 1)/sample_rate + 1; i++){
    for(int j = 0; j < 4; j++){
      fprintf(sampled_OCC_T_table, "%d ", sampled_OCC_T[i][j]);
    }
  }


  
  //printf("-- Result --\n%s\n", L);
  printf("-- Result --\n");
  printf("SA   F   L   OCC_A   OCC_C   OCC_G   OCC_T\n");
  for(int i = 0; i <= len; i++){
    printf("%3d  %c   %c    %3d     %3d     %3d     %3d\n", SA[i], F[i], L[i], OCC[i][0], OCC[i][1], OCC[i][2], OCC[i][3]);
  }
  printf("sampled OCC:\n");
  for(int i = 0; i <= len/sample_rate; i++){
    printf("%2d   %2d   %2d   %2d\n", sampled_OCC[i][0], sampled_OCC[i][1], sampled_OCC[i][2], sampled_OCC[i][3]);
  }
  printf("offset:\n");
  printf("%2d   %2d   %2d   %2d\n", A_offset, C_offset, G_offset, T_offset);

  printf("sampled OCC_A:\n");
  for(int i = 0; i <= OCC_A/sample_rate; i++){
    printf("%2d   %2d   %2d   %2d\n", sampled_OCC_A[i][0], sampled_OCC_A[i][1], sampled_OCC_A[i][2], sampled_OCC_A[i][3]);
  }
  printf("sampled OCC_C:\n");
  for(int i = 0; i <= (OCC_C - 1)/sample_rate; i++){
    printf("%2d   %2d   %2d   %2d\n", sampled_OCC_C[i][0], sampled_OCC_C[i][1], sampled_OCC_C[i][2], sampled_OCC_C[i][3]);
  }
  printf("sampled OCC_G:\n");
  for(int i = 0; i <= (OCC_G - 1)/sample_rate; i++){
    printf("%2d   %2d   %2d   %2d\n", sampled_OCC_G[i][0], sampled_OCC_G[i][1], sampled_OCC_G[i][2], sampled_OCC_G[i][3]);
  }
  printf("sampled OCC_T:\n");
  for(int i = 0; i <= (OCC_T - 1)/sample_rate; i++){
    printf("%2d   %2d   %2d   %2d\n", sampled_OCC_T[i][0], sampled_OCC_T[i][1], sampled_OCC_T[i][2], sampled_OCC_T[i][3]);
  }


  free(F);
  free(OCC);


  


  //QUERYING
  int query_size = 3;
  int search_round = query_size - 1;
  int range_max, range_min;
  char query[query_size];
  strcpy(query, "TCG");

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

    /*for(int j = range_min; j <= range_max; j++){
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
    }*/
    if(range_min % sample_rate <= sample_rate / 2){
        update_range_min = offset + sampled_OCC[range_min/sample_rate][OCC_index] - 1;
        for(int k = 1; k <= range_min % sample_rate; k++){
            if(L[sample_rate * (range_min/sample_rate) + k] == target_L){
              update_range_min += 1;
            }
        }
    }
    else if(range_min % sample_rate > sample_rate / 2){
        if(range_min/sample_rate + 1 < len/sample_rate + 1){
          update_range_min = offset + sampled_OCC[range_min/sample_rate + 1][OCC_index] - 1;
          for(int k = 0; k < sample_rate - range_min % sample_rate; k++){
            if(L[sample_rate * (range_min/sample_rate + 1) - k] == target_L){
              update_range_min -= 1;
            }
          }
        }
        else if(range_min/sample_rate + 1 >= len/sample_rate + 1){
          update_range_min = offset + sampled_OCC[range_min/sample_rate][OCC_index] - 1;
          for(int k = 1; k <= range_min % sample_rate; k++){
            if(L[sample_rate * (range_min/sample_rate) + k] == target_L){
              update_range_min += 1;
            }
          }
        }
    }
    if(L[range_min] != target_L) update_range_min += 1;


    if(range_max % sample_rate <= sample_rate / 2){
        update_range_max = offset + sampled_OCC[range_max/sample_rate][OCC_index] - 1;
        for(int k = 1; k <= range_max % sample_rate; k++){
            if(L[sample_rate * (range_max/sample_rate) + k] == target_L){
              update_range_max += 1;
            }
        }
    }
    else if(range_max % sample_rate > sample_rate / 2){
        if(range_max/sample_rate + 1 < len/sample_rate + 1){
          update_range_max = offset + sampled_OCC[range_max/sample_rate + 1][OCC_index] - 1;
          for(int k = 0; k < sample_rate - range_max % sample_rate; k++){
            if(L[sample_rate * (range_max/sample_rate + 1) - k] == target_L){
              update_range_max -= 1;
            }
          }
        }
        else if(range_max/sample_rate + 1 >= len/sample_rate + 1){
          update_range_max = offset + sampled_OCC[range_max/sample_rate][OCC_index] - 1;
          for(int k = 1; k <= range_max % sample_rate; k++){
            if(L[sample_rate * (range_max/sample_rate) + k] == target_L){
              update_range_max += 1;
            }
          }
        }
    }

    printf("updated range_min: %d, updated range_max: %d\n", update_range_min, update_range_max);
    if(update_range_min > update_range_max){
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
  return 0;
}