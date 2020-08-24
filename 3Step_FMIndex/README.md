# 3Step_FMIndex
3-step FM Index Software Implementation

The current implementation requires input reads in the syntax char *read[] (as in the Line 8 of main.cpp)

The current implementation does not create Suffix Array in order to save space, since the FM Index querying requires it only for the overlap stage and not otherwise.
Support for it can be added easily and will be added in rivision.

You can configure the parameters in the main.cpp file as required.

Variable names and meaning:

read : original read array, used to generate suffixes

read_count : number of reads in read

read_length : Equal length of all strings in str

num_suffixes : Total number of suffixes generated. Equals to read_count * read_length

suffixes : Holds the suffixes (before as well as after sorting)

d_major : Sampling distance d-value (integer) for the Occ table

d_minor : Sampling distance d-value (byte) for the Occ table



Compile using:

g++ main.cpp -o app

Run using:

./app
