
/*objective:
matrix multiplication in dynamic way

COMPILE : g++ -fopenmp -O0 -g3 -Wall -o matrixOpenMP matrixOpenMP.c 
OPTIONAL: export OMP_NUM_THREAD=2
EXECUTE : ./matrixOpenMP
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

int readFromFile(char *filename) {
//        ifstream infile;
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

int main(int argc, char *argv[])
{
	int **a,**b,**c;
	// variables to store allocated memory
	int a_r,a_c,b_r,b_c, nthreads, tid, chunk =10, size; //variables to input matrix size and variables to be used by OpenMP functions
	double dif;
	//variable to calculate the time difference between the parallelization
	int i,j,k;
	FILE *file;
	char *inputFile; char *outputFile;
	if ( argc != 5 ) /* argc should be 5 for correct execution */
    	{
        	/* We print argv[0] assuming it is the program name */
 	       printf( "usage: %s -i <INPUT_FILE> -o <OUTPUT_FILE>\n", argv[0] );
    	}	
	else {
		for (i = 1; i < argc; i++) { /* Iterate to find the inputs */
            		if (strcmp(argv[i],"-i") == 0) {
            			inputFile = argv[i + 1];
	       		} 
			else if (strcmp(argv[i],"-o") == 0) {
	            		outputFile = argv[i + 1];
        		}
            	}
		size = readFromFile(inputFile);
		printf("Matrix Multiplication for %d x %d\n",size,size);
		a_r = b_r = a_c = b_c = size;
		
		/* allocate memory for matrix one */
		a=(int **) malloc(10*a_r);
		for( i=0;i<a_c; i++) {
			a[i]=(int *) malloc(10*a_c);
		}

		/* allocate memory for matrix two */
		b=(int **) malloc(10*b_r);
		for( i=0;i<b_c; i++) {
			b[i]=(int *) malloc(10*b_c);
		}
		/* allocate memory for product matrix */
		c=(int **) malloc(10*a_r);
		for( i=0;i<b_c; i++) {
			c[i]=(int *) malloc(10*b_c);
		}
		printf("Initializing matrices...\n");
		double start = omp_get_wtime(); //start the timer
		/*** Spawn a parallel region explicitly scoping all variables ***/
		#pragma omp parallel shared(a,b,c,nthreads,chunk) private(tid,i,j,k)
		{
			tid = omp_get_thread_num();
			if (tid == 0) {
				nthreads = omp_get_num_threads();
				printf("Starting matrix multiple example with %d threads\n",nthreads);
			}
			//initializing first matrix
			#pragma omp for schedule (static, chunk)
			for(i=0;i<a_r; i++) {
				for(j=0;j<a_c; j++) {
					a[i][j] = (i+j)%1000;
				}
			}
			// initializing second matrix
			#pragma omp for schedule (static, chunk)
			for(i=0;i<b_r; i++) {
				for(j=0;j<b_c; j++) {
					b[i][j] = ((i*j)+20)%1000;
				}
			}

			/*initialize product matrix */
			#pragma omp for schedule (static, chunk)
			for(i=0;i<a_r; i++) {
				for(j=0;j< b_c; j++) {
					c[i][j]=0;
				}
			}

			/*** Do matrix multiply sharing iterations on outer loop ***/
			/*** Display who does which iterations for demonstration purposes ***/
			printf("Thread %d starting matrix multiply...\n",tid);
			#pragma omp for schedule (static, chunk)
			for(i=0;i<a_r; i++) {
	
			//	printf("Thread=%d did row=%d\n",tid,i);
				for(j=0;j<a_c; j++) {
					for(k=0;k<b_c; k++) {
						c[i][j]=c[i][j]+a[i][k]*b[k][j];
					}
				}
			}
		} /*****end of parallel region*****/

		printf("******************************************************\n");
		printf ("Done.\n");
		double end = omp_get_wtime();	//end the timer
		dif = end - start; //store the difference
		printf("It took %f sec. time.\n", dif);

		file = fopen(outputFile,"w+");
		for(i=0;i<size; i++) {
			for(j=0;j<size;j++) {
				fprintf(file,"%d ",c[i][j]);
		
			}
			fprintf(file,"\n");
		}
		fclose(file);

		/*file = fopen("A","w+");
		for(i=0;i<size; i++) {
			for(j=0;j<size;j++) {
				fprintf(file,"%d ",a[i][j]);
		
			}
			fprintf(file,"\n");
		}
		fclose(file);

		file = fopen("B","w+");
		for(i=0;i<size; i++) {
			for(j=0;j<size;j++) {
				fprintf(file,"%d ",b[i][j]);
		
			}
			fprintf(file,"\n");
		}
		fclose(file);
		*/
		/*free memory*/
		for(i=0;i<a_r; i++) {
			free(a[i]);
		}
		free(a);
		for(i=0;i<a_c; i++) {
			free(b[i]);
		}
		free(b);
		for(i=0;i<b_c; i++) {
			free(c[i]);
		}
		free(c);
	}
}


