//MM Using MPI
/*
COMPILE : mpicc -o matrixMPI matrixMPI.c
EXECUTE : mpirun -n 2 ./matrixMPI

** Changed from double to int
* changed the matrix declaration and in lines 66, 78, 91, 101 changed from 
* MPI_DOUBLE to MPI_INT

*/

/**********************************************************************************************
    * Matrix Multiplication Program using MPI.
    *
    * Viraj Brian Wijesuriya - University of Colombo School of Computing, Sri Lanka.
    *
    * Works with any type of two matrixes [A], [B] which could be multiplied to produce a matrix [c].
    *
    * Master process initializes the multiplication operands, distributes the muliplication
    * operation to worker processes and reduces the worker results to construct the final output.
    *
    ************************************************************************************************/
    #include<stdio.h>
    #include<mpi.h>
    #define MAT_SIZE 1000
    #define NUM_ROWS_A 3000 //rows of input [A]
    #define NUM_COLUMNS_A 3000 //columns of input [A]
    #define NUM_ROWS_B 3000 //rows of input [B]
    #define NUM_COLUMNS_B 3000 //columns of input [B]
    #define MASTER_TO_SLAVE_TAG 1 //tag for messages sent from master to slaves
    #define SLAVE_TO_MASTER_TAG 4 //tag for messages sent from slaves to master
    void makeAB(); //makes the [A] and [B] matrixes
    void FileprintArray(); //print the content of output matrix [C];
    int rank; //process rank
    int size; //number of processes
    int i, j, k; //helper variables
    int mat_a[MAT_SIZE][MAT_SIZE]; //declare input [A]
    int mat_b[MAT_SIZE][MAT_SIZE]; //declare input [B]
    int mat_result[MAT_SIZE][MAT_SIZE]; //declare output [C]
    double start_time; //hold start time
    double end_time; // hold end time
    int low_bound; //low bound of the number of rows of [A] allocated to a slave
    int upper_bound; //upper bound of the number of rows of [A] allocated to a slave
    int portion; //portion of the number of rows of [A] allocated to a slave
    MPI_Status status; // store status of a MPI_Recv
    MPI_Request request; //capture request of a MPI_Isend
    FILE *file;

    int main(int argc, char *argv[])
    {
		MPI_Init(&argc, &argv); //initialize MPI operations
		MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank
		MPI_Comm_size(MPI_COMM_WORLD, &size); //get number of processes
		/* master initializes work*/
		if (rank == 0) {
			makeAB();
			start_time = MPI_Wtime();
			for (i = 1; i < size; i++) {//for each slave other than the master
				portion = (MAT_SIZE / (size - 1)); // calculate portion without master
				low_bound = (i - 1) * portion;
				if (((i + 1) == size) && ((MAT_SIZE % (size - 1)) != 0)) {//if rows of [A] cannot be equally divided among slaves
						upper_bound = MAT_SIZE; //last slave gets all the remaining rows
				} else {
					upper_bound = low_bound + portion; //rows of [A] are equally divisable among slaves
				}
					//send the low bound first without blocking, to the intended slave
				MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
				//next send the upper bound without blocking, to the intended slave
				MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &request);
				//finally send the allocated row portion of [A] without blocking, to the intended slave
				MPI_Isend(&mat_a[low_bound][0], (upper_bound - low_bound) * MAT_SIZE, MPI_INT, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &request);
			}
		}
		//broadcast [B] to all the slaves
		MPI_Bcast(&mat_b, MAT_SIZE*MAT_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		/* work done by slaves*/
		if (rank > 0) {
			//receive low bound from the master
			MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
			//next receive upper bound from the master
			MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &status);
			//finally receive row portion of [A] to be processed from the master
			MPI_Recv(&mat_a[low_bound][0], (upper_bound - low_bound) * MAT_SIZE, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);
			for (i = low_bound; i < upper_bound; i++) {//iterate through a given set of rows of [A]
				for (j = 0; j < MAT_SIZE; j++) {//iterate through columns of [B]
						for (k = 0; k < MAT_SIZE; k++) {//iterate through rows of [B]
							mat_result[i][j] += (mat_a[i][k] * mat_b[k][j]);
						}
				}
			}
			//send back the low bound first without blocking, to the master
			MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
			//send the upper bound next without blocking, to the master
			MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &request);
			//finally send the processed portion of data without blocking, to the master
			MPI_Isend(&mat_result[low_bound][0], (upper_bound - low_bound) * MAT_SIZE, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &request);
		}
		/* master gathers processed work*/
		if (rank == 0) {
			for (i = 1; i < size; i++) {// untill all slaves have handed back the processed data
				//receive low bound from a slave
				MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
				//receive upper bound from a slave
				MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &status);
				//receive processed data from a slave
				MPI_Recv(&mat_result[low_bound][0], (upper_bound - low_bound) * MAT_SIZE, MPI_INT, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
			}
			end_time = MPI_Wtime();
			printf("\nRunning Time = %f\n\n", end_time - start_time);
			//printArray();
		}
		MPI_Finalize(); //finalize MPI operations
		FileprintArray();
		return 0;
    }

    void makeAB() {
		for (i = 0; i < MAT_SIZE; i++) {
			for (j = 0; j < MAT_SIZE; j++) {
				mat_a[i][j] = (i + j)%1000;
			}
		}
		for (i = 0; i < MAT_SIZE; i++) {
			for (j = 0; j < MAT_SIZE; j++) {
				mat_b[i][j] = ((i*j)+20)%1000;
			}
		}
    }

    void FileprintArray() {
	file = fopen("A","w+");
	for (i = 0; i < MAT_SIZE; i++) {
		for (j = 0; j < MAT_SIZE; j++)
			fprintf(file,"%d ", mat_a[i][j]);
		fprintf(file,"\n");
	}
	fclose(file);

	file = fopen("B","w+");
	for (i = 0; i < MAT_SIZE; i++) {
		for (j = 0; j < MAT_SIZE; j++)
			fprintf(file,"%d ", mat_b[i][j]);
		fprintf(file,"\n");
	}
	fclose(file);

	file = fopen("output","w+");
	for (i = 0; i < MAT_SIZE; i++) {
		for (j = 0; j < MAT_SIZE; j++)
			fprintf(file,"%d ", mat_result[i][j]);
		fprintf(file,"\n");
	}
	fclose(file);
    }
