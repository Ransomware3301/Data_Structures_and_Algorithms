/*
 *  Program:    Hashmap (open hashing) implementation for strings in C
 *  
 *  Made by:    ransomware3301 (https://www.github.com/ransomware3301)
 *  Date:       15/07/2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define TRUE 1
#define FALSE 0
#define HASH_PRIME 97
#define HASHMAP_BLOCK_SIZE 32
#define END_OF_STRING '\0'
#define GOLDEN_RATIO 1.618033989
#define LOAD_FACTOR (GOLDEN_RATIO - 1)   
#define ALPHA (GOLDEN_RATIO - 1)
#define HASH_CONST (double)(ALPHA * (((unsigned long int) 1) << 32))


/* ==== Type Definitions ==== */


typedef unsigned int boolean_t;


typedef struct bucket
{
    char *str;
    struct bucket *next;
}
bucket_t;


typedef struct hashmap
{
    bucket_t **buckets;
    size_t size;
    size_t occupied;
}
hashmap_t;


/* ==== Function Declarations ==== */


// Miscellaneous
char * generate_random_string(size_t);


// I/O
void print_hashmap(hashmap_t*);


// Hashmap functions
size_t      hash(char*, size_t);
size_t      bucket_size(bucket_t*);
hashmap_t * init(size_t);
hashmap_t * clear(hashmap_t*);
hashmap_t * rehash(hashmap_t*, size_t);
hashmap_t * insert(hashmap_t*, char*);
hashmap_t * delete(hashmap_t*, char*);
hashmap_t * extend(hashmap_t*, size_t);
hashmap_t * contract(hashmap_t*, size_t);
bucket_t *  search(hashmap_t*, char*);


/* ==== Main ==== */


int main(int argc, char **argv)
{
    hashmap_t *hashmap;
    size_t generations, maxlen, i;
    size_t optimal, suboptimal;
    char *str;

   
    // Random seed initialization
    srand(time(NULL));

    printf("Insert amount of string to generate: ");
    scanf("%lu", &generations);
    printf("Insert max string length: ");
    scanf("%lu", &maxlen);


    if (generations > 0 && maxlen > 0)
    {
        if (( hashmap = init(HASHMAP_BLOCK_SIZE) ))
        {
            i = 0;
            optimal = suboptimal = 0;

            while (i < generations)
            {
                str = generate_random_string(maxlen);

                printf("\n ============ (%lu) GENERATED_STRING=[%s] ============\n", i, str);

                hashmap = insert(hashmap, str);
                free(str);
                i++;

                print_hashmap(hashmap);
                printf("\n ==== (%lu) STATISTICS ====\n\n", i);
                printf("    IDEAL_LOAD: %f\n", ((double) i / hashmap->size));
                printf("    CURR_LOAD: %f\n", ((double)hashmap->occupied / hashmap->size));

                if (((double) i / hashmap->size) == ((double)hashmap->occupied / hashmap->size))
                {
                    printf("    CURR_BEHAVIOUR: optimal\n");
                    optimal++;
                }
                else
                {
                    printf("    CURR_BEHAVIOUR: sub-optimal\n");
                    suboptimal++;
                }
            }

            printf("\n ======== [FINAL HASHMAP] (size: %lu | occupied: %lu) ========\n\n", 
                hashmap->size, 
                hashmap->occupied
            );

            print_hashmap(hashmap);

            printf("\n ==== FINAL STATISTICS ====\n\n");
            printf("    OCCUPATION_PERCENTAGE: %4.2f%%\n", 100 * ((double) hashmap->occupied / hashmap->size));
            printf("    AVG_BEHAVIOUR: %s (optimal=%lu, suboptimal=%lu)\n", 
                (optimal > suboptimal ? "optimal" : "sub-optimal"),
                optimal,
                suboptimal
            );

            // Freeing memory
            hashmap = clear(hashmap);
        }
    }

    return 0;
}


/* ==== Function Definitions ==== */


/*
 *  Generates a random string of visible ASCII 7-bit characters 
 *  (range 32-126 are visible characters) of length maxlen
 */
char * generate_random_string(size_t maxlen)
{
    char *str;
    size_t randlen, i;
    size_t lowerbound;
    size_t upperbound;


    str = NULL;
    
    if (maxlen > 0)
    {
        lowerbound = 32;
        upperbound = 126;
        randlen = (rand() % maxlen) + 1;    // min=1, max=maxlen

        if (( str = (char*)malloc(sizeof(char) * (randlen + 1)) ))
        {
            *(str + randlen) = END_OF_STRING;

            for (i = 0; i < randlen; i++)
            {
                *(str + i) = (char) (rand() % (upperbound - lowerbound) + lowerbound);
            }
        }
        else
        {
            printf("[generate_random_string() -> str] ERROR: Memory allocation was unsuccessful\n");
        }
    }
    
    return str;
}


/*
 *  Prints the entire hashmap
 *
 *  Time Compelxity:    O(n)
 */
void print_hashmap(hashmap_t *hashmap)
{
    size_t i, j, dim;
    bucket_t *ptr;


    if (hashmap && hashmap->buckets)
    {
        printf("\n[HASHMAP]\n");

        for (i = 0; i < hashmap->size; i++)
        {
            ptr = (*(hashmap->buckets + i));

            if (ptr)
            {
                dim = bucket_size(ptr);

                printf(" |---> ");

                for (j = 0; j < dim - 1; j++)
                {
                    printf("[%s] --> ", ptr->str);
                    ptr = ptr->next;
                }

                printf("[%s]\n", ptr->str);
            }
            else
            {
                printf(" |---> [EMPTY]\n");
            }         
        }

        printf("\n");
    }
}


/*
 *  Calculates the hash of the given string and gets
 *  shortened to the addressable slots in the hashmap
 *  represented by the parameter size
 *
 *  Time Complexity:    O(n)
 */
// Sum of Characters Hash Functions
size_t hash_v1(char *str, size_t size)
{
    size_t hash, sum, i;


    if (str)
    {
        i = 0;
        sum = 0;

        while (*(str + i) != END_OF_STRING)
        {
            sum += (size_t) (*(str + i));
            i++;
        }

        hash = (size_t) (sum * HASH_CONST) % size;
    }
    else
    {
        hash = 0;
    }
    
    return hash;
}


// Polinomial Rolling Hash Function
size_t hash(char *str, size_t size)
{
    size_t hash, prime_pow, i;


    hash = 0;
    prime_pow = 1;

    if (str)
    {
        i = 0;

        while (*(str + i) != '\0')
        {
            /*
             *  Rebasing the ASCII 7-bit table to the character SPACE (HEX=0x20) such
             *  that we get the following mapping of exclusively printable characters:  
             * 
             *  (0x20) SPACE --MAP--> 1
             *  (0x21) !     --MAP--> 2
             *  (0x22) "     --MAP--> 3 
             *  etc...
             */

            hash = (hash + ((*(str + i) - 0x20 + 1) * prime_pow)) % size;
            prime_pow = (prime_pow * HASH_PRIME) % size ;
            i++;
        }
    }

    return hash;
}


/*
 *  Returns the size of the given bucket
 *
 *  Time Complexity:    O(k)
 */
size_t bucket_size(bucket_t *bucket)
{
    size_t size;


    size = 0;

    while (bucket)
    {
        size++;
        bucket = bucket->next;
    }

    return size;
}


/*
 *  Returns a hashmap data structure of the given size
 *
 *  Time Complexity:    O(1)
 */
hashmap_t * init(size_t size)
{
    hashmap_t *hashmap;
    size_t i;


    if (( hashmap = (hashmap_t*)malloc(sizeof(hashmap_t)) ))
    {
        if (( hashmap->buckets = (bucket_t**)malloc(sizeof(bucket_t*) * size) ))
        {
            hashmap->size = size;
            hashmap->occupied = 0;

            // Initializing each bucket to NULL
            for (i = 0; i < hashmap->size; i++)
            {
                (*(hashmap->buckets + i)) = NULL;
            }
        }
        else
        {
            printf("[init() -> hashmap->bucket] ERRROR: Memory allocation was unsuccessful\n");
            free(hashmap);
            hashmap = NULL;
        }
    }
    else
    {
        printf("[init() -> hashmap] ERRROR: Memory allocation was unsuccessful\n");
    }

    return hashmap;
}


/*
 *  Clears the given hashmap, including its contents
 *
 *  Time Complexity:    O(n)
 */
hashmap_t * clear(hashmap_t *hashmap)
{
    bucket_t *ptr, *del;
    size_t i;


    if (hashmap)
    {
        for (i = 0; i < hashmap->size; i++)
        {
            ptr = (*(hashmap->buckets + i));

            while (ptr)
            {
                del = ptr;
                ptr = ptr->next;
                free(del->str);
                free(del);
            }
        }

        free(hashmap->buckets);
        free(hashmap);   
    }

    return hashmap;
}


/*
 *  Rehashes all the items in the old hashmap and puts 
 *  them in the new hashmap.
 *  Returns the old hashmap if rehashing fails.
 * 
 *  Time Complexity:    O(nk)
 */
hashmap_t * rehash(hashmap_t *old_hashmap, size_t new_size)
{
    hashmap_t *new_hashmap;
    bucket_t *ptr;
    size_t i;


    if (old_hashmap && new_size > 0)
    {
        if (( new_hashmap = (hashmap_t*)malloc(sizeof(hashmap_t)) ))
        {
            if(( new_hashmap->buckets = (bucket_t**)malloc(sizeof(bucket_t*) * new_size) ))
            {
                // Initializing the new hashmap
                new_hashmap->size = new_size;
                new_hashmap->occupied = 0;

                for (i = 0; i < new_size; i++)
                {
                    (*(new_hashmap->buckets + i)) = NULL;
                }

                // Rehashing each element in the old hashmap and
                // inserting them in the new hashmap
                for (i = 0; i < old_hashmap->size; i++)
                {  
                    ptr = (*(old_hashmap->buckets + i));

                    while (ptr)
                    {
                        new_hashmap = insert(new_hashmap, ptr->str);
                        ptr = ptr->next;
                    }
                } 

                // Freeing memory occupied by the old hashmap
                old_hashmap = clear(old_hashmap);

                return new_hashmap;
            }
            else
            {
                printf("[rehash() -> new_hashmap->buckets] ERROR: Memory allocation was unsuccessful\n");
                free(new_hashmap);
            }
        }
        else
        {
            printf("[rehash() -> new_hashmap] ERROR: Memory allocation was unsuccessful\n");
        }
    }

    return old_hashmap;
}


/*
 *  Inserts the given string in the given hashmap
 *  If the given hashmap is enough occupied (LOAD_FACTOR), 
 *  then before inserting the hasmap gets extended
 * 
 *  Time Complexity:    O(k)  if no extension
 *                      O(nk) if extension
 */
hashmap_t * insert(hashmap_t *hashmap, char *str)
{
    bucket_t *elem;
    size_t hashval;
    char *copy;


    if (hashmap && str)
    {
        if (( copy = (char*)malloc(sizeof(char) * (strlen(str) + 1)) ))
        {
            if (( elem = (bucket_t*)malloc(sizeof(bucket_t)) ))
            {
                // Initializing the new element
                strcpy(copy, str);

                // If the load factor is surpassed, then the hashmap is extended,
                // which also triggers the rehashing of all elements
                if ((((double) hashmap->occupied) / hashmap->size) > LOAD_FACTOR)
                {
                    hashmap = extend(hashmap, HASHMAP_BLOCK_SIZE);
                }
                
                hashval = hash(copy, hashmap->size);               

                if ((*(hashmap->buckets + hashval)) == NULL)
                {
                    hashmap->occupied++;
                }

                // Pushing the element in the bucket_t list
                elem->str = copy;
                elem->next = (*(hashmap->buckets + hashval));
                (*(hashmap->buckets + hashval)) = elem;
            }
            else
            {
                printf("[insert() -> elem] ERROR: Memory allocation was unsuccessful\n");
                free(copy);
            }
        }
        else
        {
            printf("[insert() -> copy] ERRROR: Memory allocation was unsuccessful\n");
        }
    }

    return hashmap;
}


/*
 *  Deletes the given string from the hashmap
 *
 *  Time Complexity:    O(k)
 */
hashmap_t * delete(hashmap_t *hashmap, char *str)
{
    bucket_t *prev, *curr;
    size_t hashval;


    if (hashmap && str)
    {
        prev = NULL;
        hashval = hash(str, hashmap->size);
        curr = (*(hashmap->buckets + hashval));

        // Searching the string in the bucket
        while (curr && strcmp(curr->str, str) != 0)
        {
            prev = curr;
            curr = curr->next;
        }

        // If the given string is actually in the hashmap
        // then delete it, otherwise don't bother
        if (curr)
        {
            if (prev)
            {
                // Case 1:  The element to delete IS NOT the
                //          first of the bucket
                prev->next = curr->next;
                free(curr->str);
                free(curr);
                curr = prev->next;
            }
            else
            {
                // Case 2:  The element to delete IS the first
                //          of the bucket
                prev = curr;
                curr = curr->next;
                free(prev->str);
                free(prev);
                *(hashmap->buckets + hashval) = curr;
            }
        }
    }

    return hashmap;
}


/*
 *  Returns a longer hashmap than the old one and 
 *  rehashes all the elements in it 
 * 
 *  Time Complexity:    O(nk)
 */
hashmap_t * extend(hashmap_t *hashmap, size_t size)
{
    if (size > 0)
    {
        if (hashmap)
        {
            hashmap = rehash(hashmap, hashmap->size + size);
        }
        else
        {
            hashmap = init(size);
        }
    }

    return hashmap;
}


/*
 *  Contracts the given hashmap by the given amount
 *
 *  Time Complexity:    O(nk) if size < hashmap->size
 *                      O(n)  otherwise
 */
hashmap_t * contract(hashmap_t *hashmap, size_t size)
{
    if (hashmap && size > 0)
    {    
        if (size < hashmap->size)
        {
            hashmap = rehash(hashmap, hashmap->size - size);
        }
        else
        {
            hashmap = clear(hashmap);
        }
    }

    return hashmap;
}


/*
 *  Searches the given string in the hashmap
 *
 *  Time Complexity:    O(n + k)  (hashing + linear search)
 */
bucket_t * search(hashmap_t *hashmap, char *str)
{
    bucket_t *ptr;


    if (hashmap && str)
    {
        ptr = (*(hashmap->buckets + hash(str, hashmap->size)));

        while (ptr && strcmp(str, ptr->str) != 0)
        {
            ptr = ptr->next;
        }
    }
    else
    {
        ptr = NULL;
    }
    
    return ptr;
}
