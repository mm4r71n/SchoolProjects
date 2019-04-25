#include <stdio.h> 
#include <stdlib.h> 
#include <sys/time.h> 
#include "mpi.h"

struct timeval start, end;

int MAX_LENGTH = 20;

// cpu holds my processor number, cpu=0 is master, rest are slaves
// numcpus is the total number of processors
int cpu, numcpus;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER CODE TO INITIALIZE AND PRINT TIME

void starttime() {
  gettimeofday( & start, 0);
}

void endtime(const char * c) {
  gettimeofday( & end, 0);
  double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
  printf("%s: %f ms\n", c, elapsed);
}

void initialize(int* ARRAY){
  ///////////////////////////////////////////////////////////
  // Array to be sorted is initialized
  int ARRAY_LENGTH = 0;

  ARRAY = (int * ) malloc(sizeof(int) * (MAX_LENGTH));

  printf("Value of Max Length: %d \n", MAX_LENGTH);

  //This code part is for filling our array
  while (ARRAY_LENGTH < MAX_LENGTH) {
    ARRAY[ARRAY_LENGTH] = rand() % 500 + 1;
    ARRAY_LENGTH += 1;
  }
  
  printArray(ARRAY);
}

void printArray(int* Array){
  //Displays array
  for(int i=0;i<MAX_LENGTH;i++){
    printf("%d, ", Array[i]);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void normal(int * ARRAY) {

  int isSorted = 0, i, TEMPORARY_ELEMENT;

  starttime();

  //Bubble Sort Alg.

  while (!isSorted) { //While our array's not sorted
    isSorted = 1; //we suppose that it's sorted
    for (i = 0; i < MAX_LENGTH - 1; i++) { //then for each element of the array
      if (ARRAY[i] > ARRAY[i + 1]) { // if the two elements aren't sorted
        isSorted = 0; //it means that the array is not sorted
        TEMPORARY_ELEMENT = ARRAY[i]; //and we switch these elements using TEMPORARY_ELEMENT
        ARRAY[i] = ARRAY[i + 1];
        ARRAY[i + 1] = TEMPORARY_ELEMENT;
      }
    }
  }

  endtime("Normal");
}

// MPI function to square root values
void mpi(int* a) {
  int isSorted = 0, TEMPORARY_ELEMENT;

  int i, slave;
  MPI_Status status;

  int numeach = MAX_LENGTH / numcpus;

  ////////////////////////////////////////////////////////////////////////////////////////
  // I AM THE MASTER
  if (cpu == 0) {
    for (slave = 1; slave < numcpus; slave++)
      MPI_Send( & a[numeach * slave], numeach, MPI_INT, slave, 1, MPI_COMM_WORLD);

    while (!isSorted) { //While our array's not sorted
      isSorted = 1; //we suppose that it's sorted
      for (i = 0; i < numeach - 1; i++) { //then for each element of the array
        if (a[i] > a[i + 1]) { // if the two elements aren't sorted
          isSorted = 0; //it means that the array is not sorted
          TEMPORARY_ELEMENT = a[i]; //and we switch these elements using TEMPORARY_ELEMENT
          a[i] = a[i + 1];
          a[i + 1] = TEMPORARY_ELEMENT;
        }
      }
    }
	
	for (slave = 1; slave < numcpus; slave++)
      MPI_Recv( & a[numeach * slave], numeach, MPI_INT, slave, 2, MPI_COMM_WORLD, & status);
  }
  /////////////////////////////////////////////////////////////////////////////////////////
  // I AM A SLAVE
  else {
    int data[numeach];
    MPI_Recv( & data[0], numeach, MPI_INT, 0, 1, MPI_COMM_WORLD, & status);
	
    while (!isSorted) { //While our array's not sorted
      isSorted = 1; //we suppose that it's sorted
      for (i = 0; i < numeach - 1; i++) { //then for each element of the array
        if (data[i] > data[i + 1]) { // if the two elements aren't sorted
          isSorted = 0; //it means that the array is not sorted
          TEMPORARY_ELEMENT = data[i]; //and we switch these elements using TEMPORARY_ELEMENT
          data[i] = data[i + 1];
          data[i + 1] = TEMPORARY_ELEMENT;
        }
      }
    }
	
    MPI_Send( & data[0], numeach, MPI_INT, 0, 2, MPI_COMM_WORLD);
  }
  /////////////////////////////////////////////////////////////////////////////////////////
  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv) {

  ///////////////////////////////////////////////////////////
  // MPI Starter Code
  MPI_Init( & argc, & argv);
  MPI_Comm_rank(MPI_COMM_WORLD, & cpu);
  MPI_Comm_size(MPI_COMM_WORLD, & numcpus);
  //////////////////////////////////////////////////////////
  
  int ARRAY[MAX_LENGTH];
  
  if(cpu == 0){
	initialize(ARRAY);
	normal(ARRAY);
	print("Normal Sort Result: \n");
	printArray(ARRAY);

	initialize(ARRAY);
	starttime();	
  }
	  
  mpi(ARRAY);
  
  if(cpu == 0){
	endtime("MPI");  
	print("MPI Sort Result: \n");
	printArray(ARRAY);
  }

  ////////////////////////////////////////////////////////////////////////////////
  // MPI Finish Code
  MPI_Finalize();
  ////////////////////////////////////////////////////////////////////////////////

  return 0;
}