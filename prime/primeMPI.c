# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>

# include "mpi.h"

int main ( int argc, char *argv[] );
int prime_number ( int n, int id, int p );

/******************************************************************************/

int main ( int argc, char *argv[] )

/******************************************************************************/
/*
    This program calls a version of PRIME_NUMBER that includes
    MPI calls for parallel processing.
*/
{
  int id;
  int ierr;
  int master = 0;
  int n;
  int n_factor;
  int n_hi;
  int n_lo;
  int p;
  int primes;
  int primes_part;
  double wtime;

  n_lo = 1;
  n_hi = 131072;
  n_factor = 2;

  //Initialize MPI.
  ierr = MPI_Init ( &argc, &argv );

  //Get the number of processes.
  ierr = MPI_Comm_size ( MPI_COMM_WORLD, &p );

  //Determine this processes's rank.
  ierr = MPI_Comm_rank ( MPI_COMM_WORLD, &id );

  if ( id == master )
  {
    timestamp ( );
    printf ( "\n" );
    printf ( "'PRIME_MPI\n" );
    printf ( "  An MPI example program to count the number of primes.\n" );
    printf ( "  The number of processes is %d\n", p );
  }

  n = n_lo;

  while ( n <= n_hi )
  {
    if ( id == master )
    {
      wtime = MPI_Wtime ( );
    }
    ierr = MPI_Bcast ( &n, 1, MPI_INT, master, MPI_COMM_WORLD );

    primes_part = prime_number ( n, id, p );

    ierr = MPI_Reduce ( &primes_part, &primes, 1, MPI_INT, MPI_SUM, master, 
      MPI_COMM_WORLD );

    if ( id == master )
    {
      wtime = MPI_Wtime ( ) - wtime;
    }
    n = n * n_factor;
  }

  //Terminate MPI.
  ierr = MPI_Finalize ( );

  
  //Terminate the main program
  if ( id == master ) {
    printf ( "  Terminated.\n"); 
   
  }

  return 0;
}

/*
    Returns the number of primes between 1 and N.
  
  Description:
    In order to divide the work up evenly among P processors, processor
    ID starts at 2+ID and skips by P.

  Parameters:

    Input, int N, the maximum number to check.

    Input, int ID, the ID of this process,
    between 0 and P-1.

    Input, int P, the number of processes.

    Output, int PRIME_NUMBER, the number of prime numbers up to N.
*/
int prime_number ( int n, int id, int p ) {
  int i;
  int j;
  int prime;
  int total;

  total = 0;

  for ( i = 2 + id; i <= n; i = i + p )
  {
    prime = 1;
    for ( j = 2; j < i; j++ )
    {
      if ( ( i % j ) == 0 )
      {
        prime = 0;
        break;
      }
    }
    total = total + prime;
  }
  return total;
}