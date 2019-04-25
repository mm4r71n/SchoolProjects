/*
* Algorithm : Bucket Sort
* Time-Complexity : O(n)
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mpi.h"
#include <sys/time.h>

#define NARRAY 10  /* array size */
#define NBUCKET 10 /* bucket size */
#define INTERVAL 10 /* bucket range */

struct Node
{
	int data;
	struct Node *next;
};

void BucketSort(int arr[]);
struct Node *InsertionSort(struct Node *list);
int getBucketIndex(int value);

		
void BucketSort(int arr[])
{
    int i,j;
    struct Node **buckets;

    //allocate memory for array of pointers to the buckets
    buckets = (struct Node **)malloc(sizeof(struct Node*) * NBUCKET);

    //initialize pointers to the buckets*/ 
    for(i = 0; i < NBUCKET; ++i)
    {
        buckets[i] = NULL;
    }

    //put items into the buckets 
    //creates a link list in each bucket slot
    for(i = 0; i < NARRAY; ++i)
    {
        struct Node *current;
        int pos = getBucketIndex(arr[i]);
        current = (struct Node *) malloc(sizeof(struct Node));
        current->data = arr[i];
        current->next = buckets[pos];
        buckets[pos] = current;
    }


    //sorting bucket using Insertion Sort
    for(i = 0; i < NBUCKET; ++i)
    {
        buckets[i] = InsertionSort(buckets[i]);
    }

    //free memory
    for(i = 0; i < NBUCKET; ++i)
    {
        struct Node *node;
        node = buckets[i];
        while(node)
        {
            struct Node *tmp;
            tmp = node;
            node = node->next;
            free(tmp);
        }
    }
    free(buckets);
    return;
}

//Insertion Sort 
struct Node *InsertionSort(struct Node *list)
{
    struct Node *k,*nodeList;
    // need at least two items to sort
    if(list == NULL || list->next == NULL)
    {
        return list;
    }

    nodeList = list;
    k = list->next;
    nodeList->next = NULL; // 1st node is new list
    while(k != NULL)
    {
        struct Node *ptr;
        // check if insert before first 
        if(nodeList->data > k->data)
        {
            struct Node *tmp;
            tmp = k;
            k = k->next; // important for the while
            tmp->next = nodeList;
            nodeList = tmp;
            continue;
        }

        // from begin up to end
        // finds [i] > [i+1]
        for(ptr = nodeList; ptr->next != NULL; ptr = ptr->next)
        {
            if(ptr->next->data > k->data) break;
        }

        // if found (above)
        if(ptr->next != NULL)
        {
            struct Node *tmp;
            tmp = k;
            k = k->next; // important for the while
            tmp->next = ptr->next;
            ptr->next = tmp;
            continue;
        }
        else
        {
            ptr->next = k;
            k = k->next; // important for the while
            ptr->next->next = NULL;
            continue;
        }
    }
    return nodeList;
}

int getBucketIndex(int value)
{
    return value/INTERVAL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HELPER CODE TO INITIALIZE, PRINT AND TIME
	struct timeval start, end;
	
	void initialize(int arr[]) 
	{
	  int i;
	  
	  for (i = 0; i < NARRAY; ++i) 
	  { 
	    arr[i] = (rand() % 100) + 1; 
		//printf("%d" , arr[i]);
	  } 
	  //printf("\n");                                                                                                                                                                                      
	}

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
	
	void init(int arr[], const char* c) 
	{
	  printf("***************** %s **********************\n", c);
	  initialize(arr); 
	  //print(a, N);
	  starttime();
	}

	void finish(int arr[], const char* c) 
	{
	  endtime(c);
	  //print(a, N);
	  printf("***************************************************\n");
    }
//////////////////////////////////////////////////////////////
	
int main(int argc, char** argv)
{
   ///////////////////////////////////////////////////////////
	//Initialization

	int i;
	int array[NARRAY];	
	
   ///////////////////////////////////////////////////////////
    // MPI Starter Code
	
	int cpu,numcpus;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &cpu);
    MPI_Comm_size(MPI_COMM_WORLD, &numcpus);
	
   //////////////////////////////////////////////////////////
	//Start Program
	if (cpu == 0)
	{
		//Master Code
		initialize(array);
		//Test 1 Normal
		init(array, "Normal");
		BucketSort(array);
		finish(array, "Normal");
		//Test 2 MPI
		init(array, "MPI");   
	    starttime();                                    
	}
	
	//////////////////////////////////////////////////////////
	 //Scatter Data
	
	int numeach = NARRAY/numcpus;
	
	int *a = malloc(numeach * sizeof(int));
	MPI_Scatter(array, numeach, MPI_INT, a, numeach, MPI_INT, 0, MPI_COMM_WORLD);
	
	///////////////////////////////////////////////////////////
	 //Sort Array
	
	BucketSort(a);
	
	///////////////////////////////////////////////////////////
	 //Gather Information
	
	int *sorted = NULL;
	
	if(cpu == 0)
	{
		sorted = malloc(NARRAY * sizeof(int));
	}
	MPI_Gather(a, numeach, MPI_INT, sorted, numeach, MPI_INT, 0, MPI_COMM_WORLD);
	////////////////////////////////////////////////////////////
	
	if (cpu == 0)
		finish(array,"MPI");
	////////////////////////////////////////////////////////////
	 //MPI Finish Code
	
	MPI_Finalize();
	
	////////////////////////////////////////////////////////////
    return 0;
	///////////////////////////////////////////////////////////
}

