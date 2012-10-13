// COMPILE	: g++ -fopenmp primeOpenMP.c -o primeOpenMP
// EXECUTE	: (export OMP_NUM_THREADS=2) ./primeOpenMP

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <omp.h>

/*
 * Read the value of N from the input file
 * 
 * @param filename File to be read
 */
int readFromFile (char *filename) {
	FILE *infile;
        char line[100];
        infile = fopen (filename, "r");
	if (infile == 0) {
                printf("Unable to read from input file\n");
                exit(1);
        }
        else {
                fgets(line,10,infile);
		fclose(infile);
        }
        return atoi(line);
}

int main (int argc, char *argv[]);
void prime_number_sweep (int n_lo, int n_hi, int n_factor);
int prime_number (int n);

int main (int argc, char *argv[]) {
  int n_factor;
  int n_hi;
  int n_lo;
  int i;
  char* inputFile;
  char* outputFile;
  FILE *file;
 
  if (argc != 5) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
	printf("Usage is -in <infile> -out <outdir>\n"); // Inform the user of how to use the program
	exit(-1);
  }
  else  { // if we got enough parameters...
	for (i = 1; i < argc; i++) { /* Iterate to find the inputs */
    		if (strcmp(argv[i],"-i") == 0) {
    			inputFile = argv[i + 1];
       		} 
		else if (strcmp(argv[i],"-o") == 0) {
            		outputFile = argv[i + 1];
		}
    	}
	n_lo = 5;
	n_hi = readFromFile(inputFile);
	n_factor = 10;

	printf(" Prime Numbers counted between 1 and %d \n",n_hi);
	printf ("  Number of processors available = %d\n", omp_get_num_procs ());
	printf ("  Number of threads =              %d\n", omp_get_max_threads ());

	n_hi = prime_number (n_hi);
 	file = fopen(outputFile,"w+");
	fprintf(file,"%d",n_hi);
        fprintf(file,"\n");
        fclose(file);
	
	return 0;
  }
}


/*
 * Calculates the number of primes between 1 and N
 * 
 * @param N
 */
int prime_number (int n) {
  int i;
  int j;
  int prime;
  int total = 0;
  double wtime;
  int *prime_list;
  prime_list = (int *) malloc(sizeof(int)*(n/2));
  wtime = omp_get_wtime ();
# pragma omp parallel shared ( n ) private ( i, j, prime )
# pragma omp for reduction ( + : total )
  for (i = 2; i <= n; i++) {
    prime = 1;

    for (j = 2; j < i; j++) {
      if (i % j == 0) {
        prime = 0;
        break;
      }
    }
    if(j==i) {
	//printf("%d\n",j);
    	//prime_list = (int *) malloc(sizeof(int)*1);
	prime_list[total] = j;
    }
    total = total + prime;
  }
  wtime = omp_get_wtime () - wtime;
  printf("There are %d prime numbers between 1 and %d\n Time taken %5f \n",total, n, wtime);
  return total;
}
