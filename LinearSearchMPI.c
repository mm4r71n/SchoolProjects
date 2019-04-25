#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>

struct timeval start, end;

void starttime()
{
	gettimeofday( &start, 0 );
}

void endtime(const char* c)
{
	gettimeofday( &end, 0 );
	double elapsed = ( end.tv_sec - start.tv_sec ) * 1000.0 + ( end.tv_usec - start.tv_usec ) / 1000.0;
	printf("%s: %f ms\n", c, elapsed);
}

int main(int argc, char *argv[])
{
	int p;
	int np;
	int pfound;
	int size = 1000000;
	int v = 10000000;
	int *sub;
	int arr[size];
	int done = 0;
	int found = 0;
	int nvalues;
	int i, j, dummy, index;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &p);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	if (p == 0) 
	{/*
		printf("Enter the size of the array:\n");
		fflush (stdout);
		scanf("%d", &size);
		arr = (int *)malloc(size*sizeof(int));
		*/
		printf("Creating an array of size %d............\n", size);
		fflush (stdout);
		for(i = 0; i < size; i++) 
		{
			arr[i] = rand() % size + 1;
		}
		starttime();
		/*
		printf("Enter the value to be searched:\n");
		fflush (stdout);
		scanf("%d", &v);
		printf("hi1\n");
		
		if(np > 1)
		{
			for(i = 1; i < np; i++)
			{
				printf("hi2\n");
				fflush (stdout);
				MPI_Send(&v, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			}
		}*/
	}
	
	nvalues = size/np;
	sub = (int *)malloc(nvalues *sizeof(int));
	MPI_Scatter(arr, nvalues, MPI_INT, sub, nvalues, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	i = 0;
	while(!done && i < nvalues) 
	{
		if (sub[i] == v) 
		{
			done = 1;
			pfound = p;
			found = 1;
		}
		++i;
	}
	
	if(found)
	{
		printf("\n*******************************************************************\n");
		fflush (stdout);
		printf("\tProcess %d found the number %d at local index %d\n", pfound, v, i);
		fflush (stdout);
		printf("\n*******************************************************************\n");
		fflush (stdout);
	}
	if(!found)
	{
		printf("\n*******************************************************************\n");
		fflush (stdout);
		printf("\tProcess %d did not find number %d on array.\n", p, v);
		fflush (stdout);
		printf("\n*******************************************************************\n");
		fflush (stdout);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(p == 0)
	{
		endtime("Normal");
	}
	MPI_Finalize();
	return 0;
}
