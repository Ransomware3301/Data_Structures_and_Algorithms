/*
 *  Program:    C implementation of the Merge Sort Algorithm for integers
 *              with command line input
 *  
 *  Made by:    ransomware3301 (https://www.github.com/ransomware3301)
 *  Date:       01/07/2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define INT_MAX 2147483647


/* ==== Functions Declarations ==== */


void show_usage(char*);
void  print_arr(int*, size_t);
int * generate_random_arr(size_t, size_t);
void  merge(int*, size_t, size_t, size_t);
void  merge_sort(int*, size_t, size_t);


/* ==== Main ==== */


int main(int argc, char **argv)
{
    int *arr;
    unsigned int len, range;


    if (argc == 3)
    {
        len = atoi(argv[1]);
        range = atoi(argv[2]);

        if (len > 0 && range > 0)
        {
            if (( arr = generate_random_arr(len, range) ))
            {
                printf("\n ==== BEFORE MERGE_SORT ====\n");
                print_arr(arr, len);

                merge_sort(arr, 0, len - 1);

                printf("\n ==== AFTER MERGE_SORT ====\n");
                print_arr(arr, len);

                free(arr);
            }
        }
        else
        {
            printf("[main() -> LEN || RANGE] ERROR: The given command line arguments LEN and RANGE must both be greater than zero\n");
        }   
    }
    else
    {
        printf("\nERROR: Incorrect command line parameter input\n");
        show_usage(argv[0]);
    }

    return 0;
}


/* ==== Functions Definitions ==== */


/*
 *  Prints to terminal how to use command line input for this program
 */
void show_usage(char *progname)
{
    printf("\nUsage:\t%s [LEN] [RANGE]\n\n", progname);
    printf("Legend:\n");
    printf(" - [LEN] = Length of the integer array\n");
    printf(" - [RANGE] = The generated values will belong in the range [0, RANGE - 1]\n\n");
}


/*
 *  Prints to terminal the given integer array
 */
void print_arr(int *arr, size_t len)
{
    size_t i;


    if (arr)
    {
        printf("\n[ARRAY CONTENTS]\n");

        for (i = 0; i < len; i++)
        {
            printf("%d ", *(arr + i));
        }
    }

    printf("\n\n");
}


/*
 *  Returns an integer array of size len filled with random values that
 *  belong in the range [0, max - 1]
 */
int * generate_random_arr(size_t len, size_t max)
{
    int *arr;
    int i;


    if (len > 0)
    {
        if (( arr = (int*)malloc(sizeof(int) * len) ))
        {
            srand(time(NULL));

            for (i = 0; i < len; i++)
            {
                *(arr + i) = rand() % max;
            }

            return arr;
        }
        else
        {
            printf("[generate_random_arr() -> arr] ERROR: Memory allocation was unsuccessful\n");
        }
    }

    return NULL;
}


/*
 *  Given the indexes of begin and end of the two partitions, which are [p, q] for 
 *  the left partition and [q + 1, r] for the right partition, it merges them (in place)
 *  by confronting each value and putting them in the correct order (which is ascending)
 */
void merge(int *arr, size_t p, size_t q, size_t r)
{
    int *left, *right;
    size_t len1, len2;
    unsigned int i, j, k;


    if (arr)
    {
        len1 = q - p + 1;
        len2 = r - q;

        if (
            (left = (int*)malloc(sizeof(int) * (len1 + 1)))
            && (right = (int*)malloc(sizeof(int) * (len2 + 1)))
        )
        {
            for (i = 0; i < len1; i++)
            {
                *(left + i) = *(arr + i + p);
            }

            for (i = 0; i < len2; i++)
            {
                *(right + i) = *(arr + i + q + 1);
            }

            *(left + len1) = INT_MAX;
            *(right + len2) = INT_MAX;

            i = 0;
            j = 0;

            for (k = p; k <= r; k++)
            {
                if (*(left + i) <= *(right + j))
                {
                    *(arr + k) = *(left + i);
                    i++;
                }
                else
                {
                    *(arr + k) = *(right + j);
                    j++;
                }
            }

            free(left);
            free(right);
        }
        else
        {
            printf("[merge() -> left || right] ERROR: Memory allocation was unsuccessful\n");
        }
    }
}


/* 
 *  Given an array with indexes in range [p, r], it splits it into two partition
 *  (left and right) continuously by applying the "divide et impera" method
 *  until the original array is correctly sorted (in ascending order)
 */
void merge_sort(int *arr, size_t p, size_t r)
{
    size_t q;
    int tmp;


    if (arr)
    {
        if (r > 0 && p < r - 1)
        {
            q = (p + r) / 2;

            merge_sort(arr, p, q);
            merge_sort(arr, q + 1, r);
            merge(arr, p, q, r);
        }
        else
        {
            if (*(arr + p) > *(arr + r))
            {
                tmp = *(arr + r);
                *(arr + r) = *(arr + p);
                *(arr + p) = tmp;
            }
        }
    }   
}
