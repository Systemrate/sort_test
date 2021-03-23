/*-----------------------------------------------------------------------------
 *
 * sort_test.cpp
 * 
 * An exploration of serveral sorting algorithms
 * 
 *----------------------------------------------------------------------------*/

#include <memory.h>
#include <stdio.h>
#include <time.h>

/*-----------------------------------------------------------------------------
 * Constants and macro definitions
 *---------------------------------------------------------------------------*/
#define TEST_DATA_LEN (64)

#define SHOW_PROGRESS

#ifdef SHOW_PROGRESS
    #define DUMP_PROGRESS(i, d, n) dump_progress(i, d, n)
#else
    #define DUMP_PROGRESS(i, d, n)
#endif

// Uncomment one of these to test that sort
#define PERFORM_SORT bubble_sort(data, num_items);
//#define PERFORM_SORT selection_sort(data, num_items);
//#define PERFORM_SORT insertion_sort(data, num_items);
//#define PERFORM_SORT merge_sort(data, temp_data, num_items);
//#define PERFORM_SORT quick_sort(data, num_items);


/*-----------------------------------------------------------------------------
 * Local function prototypes
 *---------------------------------------------------------------------------*/
static void bubble_sort(int *data, int num_items);
static void selection_sort(int *data, int num_items);
static void insertion_sort(int *data, int num_items);

static void merge_sort(int *a, int *b, int num_items);
static void merge(int *a, int left, int right, int end, int *b);
static void copy_array(int *b, int *a, int num_items);
static int minimum(int a, int b);

static void quick_sort(int *data, int num_items);

/*-----------------------------------------------------------------------------
 * Test data
 *----------------------------------------------------------------------------*/
int test_data [TEST_DATA_LEN] =
{
    34, 23, 13,  7, 47,  4,  8, 50,
    49,  9,  6, 35, 53, 51, 42, 11,
    22, 63,  3, 25, 39,  2, 10, 18,
    38, 30, 61, 46, 32, 28, 17, 54,
    48, 12,  1, 21, 56, 55, 57, 16,
    44,  0, 36, 40, 62, 43, 58, 15,
    60, 14, 19, 26, 52, 41, 59, 29,
    33, 45, 31, 24, 27, 5, 37, 20
};

/*-----------------------------------------------------------------------------
 * Global data
 *----------------------------------------------------------------------------*/
static int temp_data[TEST_DATA_LEN];

/*-----------------------------------------------------------------------------
 * Main entry point
 *----------------------------------------------------------------------------*/
int main(int argc, const char * argv[])
{
    int num_items = TEST_DATA_LEN;

    int data[TEST_DATA_LEN];


#ifdef SHOW_PROGRESS
    memcpy(data, test_data, sizeof(data));
    // Perform the sort once and display the progress of each iteration
    PERFORM_SORT;
#else
    // Perform the sort multiple times and time it
    double time_spent = 0;
    int num_tests = 1000000;
    
    for (int it = 0; it < num_tests; it++)
    {
        memcpy(data, test_data, sizeof(data));
        clock_t begin = clock();
    
        PERFORM_SORT;
        
        clock_t end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    }

    time_spent /= num_tests;
    printf("Time taken was %fus", time_spent* 1000000.0f);
#endif    
    
    printf("\n");
    return 0;
}

/*-----------------------------------------------------------------------------
 * Display the order of the data for the current iteration
 * 
 * iteraion.... Currnt interation number
 * 
 * data........ The items to sort
 * 
 * num_items... The number of items to sort
 *----------------------------------------------------------------------------*/
void dump_progress(int iteration, int *data, int num_items)
{
    printf("%3i: ", iteration);
    
    for (int i = 0; i < num_items; i++)
    {
        printf("%d, ", data[i]);
    }
    printf("\n");
}

/*-----------------------------------------------------------------------------
 * Perform Bubble Sort
 * 
 * data........ The items to sort
 * 
 * num_items... The number of items to sort
 *----------------------------------------------------------------------------*/
void bubble_sort(int *data, int num_items)
{
    int items_swapped;
    int iteration = 0;
    
    do
    {
        items_swapped = 0;

        for (int i = 1; i < num_items; i++)
        {
            if (data[i-1] > data[i])
            {
                int temp = data[i-1];
                data[i-1] = data[i];
                data[i] = temp;
                
                items_swapped = 1;
            }
        }
        
        DUMP_PROGRESS(iteration, data, num_items);
        iteration++;

    } while (items_swapped != 0);
}

/*-----------------------------------------------------------------------------
 * Perform Selection Sort
 * 
 * data........ The items to sort
 * 
 * num_items... The number of items to sort
 *----------------------------------------------------------------------------*/
void selection_sort(int *data, int num_items)
{
    for (int i = 0; i < num_items; i++)
    {

        int min = i;
        
        for (int j = i+1; j < num_items; j++)
        {
            if (data[j] < data[min])
            {
                min = j;
            }
        }
        
        int temp = data[min];
        data[min] = data[i];
        data[i] = temp;

        DUMP_PROGRESS(i, data, num_items);
    }
}

/*-----------------------------------------------------------------------------
 * Perform Insertion Sort
 * 
 * data........ The items to sort
 * 
 * num_items... The number of items to sort
 *----------------------------------------------------------------------------*/
void insertion_sort(int *data, int num_items)
{
    DUMP_PROGRESS(0, data, num_items);
    
    for (int i = 1; i < num_items; i++)
    {
        for (int j = i; j > 0 && (data[j] < data[j-1]); j--)
        {
            int temp = data[j-1];
            data[j-1] = data[j];
            data[j] = temp;
        }

        DUMP_PROGRESS(i, data, num_items);
    }
}

/*-----------------------------------------------------------------------------
 * Perform Merge Sort
 * 
 * data........ The items to sort
 * 
 * work........ Working array
 * 
 * num_items... The number of items to sort
 *----------------------------------------------------------------------------*/
void merge_sort(int *data, int *work, int num_items)
{
    int iteration = 0;
    DUMP_PROGRESS(iteration, data, num_items);

    // Each 1-element run in data is already "sorted".
    // Make successively longer sorted runs of length 2, 4, 8, 16...
    // until whole array is sorted.
    for (int width = 1; width < num_items; width = 2 * width)
    {
        // data array  is full of runs of length width.
        for (int i = 0; i < num_items; i = i + 2 * width)
        {
            // Merge two runs: data[i..i+width-1] and data[i+width..i+2*width-1] to work[]
            // or copy data[i..n-1] to work[] ( if(i+width >= n) )
            merge(data, i, minimum(i+width, num_items), minimum(i+2*width, num_items), work);
        }

        // Now work array is full of runs of length 2*width.
        // Copy array work to data array for next iteration.
        // A more efficient implementation would swap the roles of data and work.
        copy_array(work, data, num_items);

        // Now data array is full of runs of length 2*width.
        
        DUMP_PROGRESS(iteration, data, num_items);
        iteration++;

    }
}

/*-----------------------------------------------------------------------------
 * Merge two runs of data together
 * 
 * data........ The two runs
 * 
 * left........ The start of the first run
 * 
 * right....... The start of the second run
 * 
 * end......... The index after the end of the second run
 * 
 * work........ The output containing the two merges runs
 * 
 * First run is data[left..right-1].
 * 
 * Second run is data[right..end-1].
 *----------------------------------------------------------------------------*/
void merge(int *data, int left, int right, int end, int *work)
{
    int i = left;
    int j = right;
    // While there are elements in the left or right runs...
    for (int k = left; k < end; k++) {
        // If left run head exists and is <= existing right run head.
        if (i < right && (j >= end || data[i] <= data[j])) {
            work[k] = data[i];
            i = i + 1;
        } else {
            work[k] = data[j];
            j = j + 1;
        }
    }
}

/*-----------------------------------------------------------------------------
 * Copy an array
 * 
 * src........ The src array
 * 
 * dest........ The destination array
 * 
 * num_items... The number of items
 *----------------------------------------------------------------------------*/
void copy_array(int *src, int *dest, int num_items)
{
    for(int i = 0; i < num_items; i++)
    {
        dest[i] = src[i];
    }
}

/*-----------------------------------------------------------------------------
 * Return the minimum of two items
 * 
 * a... The first items
 * 
 * b... The second item
 *----------------------------------------------------------------------------*/
int minimum(int a, int b)
{
    return a < b ? a : b;
}

/*-----------------------------------------------------------------------------
 * Working data for the quick sort algorithm
 *----------------------------------------------------------------------------*/
int qIter = 0;
int *qData = NULL;
int qItems = 0;

/*-----------------------------------------------------------------------------
 * Perform Quick Sort
 * 
 * data........ The items to sort
 * 
 * num_items... The number of items to sort
 *----------------------------------------------------------------------------*/
void quick_sort(int *data, int num_items)
{
    if (qData == NULL)
    {
        qData = data;
        qItems = num_items;
    }
    
    if (num_items < 2)
    {
        return;
    }
 
    // Choose the middle element as the pivot
    int pivot = data[num_items / 2];
 
    int i;
    int j;
    
    for (i = 0, j = num_items - 1; ; i++, j--)
    {
        // Find the first element that is less than the pivot
        while (data[i] < pivot)
        {
            i++;
        }
  
        // Find the last element that is greater than the pivot
        while (data[j] > pivot)
        {
            j--;
        }
      
        // If the two indices have crossed then terminate the loop
        if (i >= j)
        {
            break;
        }
 
        // Swap the elements at [i] and [j]
        int temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    DUMP_PROGRESS(qIter, qData, qItems);
    qIter++;
    
    // Thw element at [i] is in the correct position
    // Now recurse to sort the two partiions either side of it.
    quick_sort(data, i);
    quick_sort(data + i, num_items - i);
}
