#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){int n=300;if(argc>1)n=atoi(argv[1]);if(n<200)n=200;if(n>500)n=500;if(!generate_dataset("dataset/programs.txt",n)){fprintf(stderr,"Generation failed\n");return 1;}printf("Generated %d programs in dataset/programs.txt\n",n);return 0;}
