/*
 *  Program:    Integer keys Min-Heap and Max-Heap C library
 *  Made by:    ransomware3301 (https://www.github.com/ransomware3301)
 * 
 *  Date:       16/08/2024
 */


#include <stdio.h>
#include <stdlib.h>


#define FALSE 0
#define TRUE 1
#define INT_MAX 0x7fffffff
#define ERROR_VALUE 0


/* ==== Type Definitions ==== */


typedef unsigned char uint8_t;


typedef enum htype
{
    MAX_HEAP,
    MIN_HEAP
}
htype_t;


typedef struct heap
{
    int key;
    htype_t heap_type;
    size_t qleft;
    size_t qright;
    size_t qlastlayer;
    struct heap *parent;
    struct heap *left;
    struct heap *right;
}
heap_t;


/* ==== Function Declarations ==== */

// I/O
void print_2D_heap(heap_t*, size_t);


// Heap Functions
heap_t * heap_insert(heap_t*, htype_t, int);
heap_t * heap_pop_front(heap_t*, int**);
void     heap_clear(heap_t**);


/* ==== Main ==== */


int main (int argc, char **argv)
{
    heap_t *root;
    htype_t type;
    int *buf;
    int t;


    root = NULL;
    buf = NULL;

    printf("Choose type of heap:\n");
    printf(" (0)        MAX_HEAP\n");
    printf(" (1)        MIN_HEAP\n");
    printf(" (other)    quit\n");
    printf("?> ");

    if (scanf("%d", &t) == 1)
    {
        if (t == MAX_HEAP)
        {
            type = MAX_HEAP;
        }
        else if (t == MIN_HEAP)
        {
            type = MIN_HEAP;
        }
        else
        {
            t = -1;
        }

        if (t != -1)
        {
            printf("Insert a number (-1 to quit, -2 to pop): ");
            while ((scanf("%d", &t) == 1) && (t != -1))
            {
                if (t == -2)
                {
                    printf("\n====== (BEFORE) HEAP_POP_FRONT ======\n\n");
                    print_2D_heap(root, 0);

                    root = heap_pop_front(root, &buf);
                    
                    if (buf)
                    {
                        printf("--> Last heap_pop_front result: [key=%d]\n", *buf);
                    }

                    printf("\n====== (AFTER) HEAP_POP_FRONT ======\n\n");
                    print_2D_heap(root, 0);
                }
                else
                {
                    printf("\n====== INSERTING [key=%d] ======\n\n", t);
                    root = heap_insert(root, type, t);
                    print_2D_heap(root, 0);
                }

                printf("\nInsert a number (-1 to quit, -2 to pop): ");
            }

            // Freeing memory
            if (root) heap_clear(&root);
            if (buf)  free(buf);
        }
    }

    return 0;
}


/* ==== Function Definitions ==== */


/*
 *  Prints the given heap in order and in 2D
 *
 *  Time Complexity:    O(n)
 */
void print_2D_heap(heap_t *root, size_t depth)
{
    size_t i;


    if (root)
    {
        print_2D_heap(root->right, ++depth);
        depth--;

        for (i = 0; i < depth; i++)
        {
            printf("\t");
        }

        if (root->parent == NULL)
        {
            printf("(r) ");
        }

        printf("[key=%d, qleft=%lu, qright=%lu, qlastlayer=%lu]", root->key, root->qleft, root->qright, root->qlastlayer);
        printf("\n\n");
        
        print_2D_heap(root->left, ++depth);
    }
}


/*
 *  Inserts a new element with the given key from the heap
 *
 *  Time Complexity:    O(log(n))
 */
heap_t * heap_insert(heap_t *root, htype_t htype, int key)
{
    heap_t *item, *curr, *prev;
    uint8_t pos;
    int tmp;

    
    if (( item = (heap_t*)malloc(sizeof(heap_t)) ))
    {
        item->key = key;
        item->heap_type = htype;
        item->qleft = 0;
        item->qright = 0;
        item->qlastlayer = 2;
        item->left = NULL;
        item->right = NULL;

        if (root == NULL)
        {
            item->parent = NULL;
            root = item;
        }
        else if (root && root->parent == NULL)
        {
            if (htype != root->heap_type)
            {
                printf("[heap_insert()] ERROR: Given heap type is incompatible with the given heap structure\n");
                free(item);
            }
            else
            {
                prev = NULL;
                curr = root;
                
                // Inserting the element at the right place  
                while (curr)
                {
                    prev = curr;

                    if (curr->qleft == curr->qright && curr->left > 0)
                    {
                        prev->qlastlayer += prev->qlastlayer;
                    }
                    
                    if (curr->qleft < curr->qlastlayer - 1)
                    {
                        curr->qleft++;
                        curr = curr->left;
                        pos = 0;
                    }
                    else
                    {
                        curr->qright++;
                        curr = curr->right;
                        pos = 1;
                    }
                }

                // Adjusting the child-parent links
                item->parent = prev;

                if (pos == 0)
                {
                    prev->left = item;
                }
                else
                {
                    prev->right = item;
                }

                // Adjusting the keys (waterfall fashion)
                // From the inserted leaf, the keys will fall down like water in a waterfall
                // until the given key is bigger/smaller than the current one
                curr = item->parent;
                prev = item;

                if (htype == MAX_HEAP)
                {
                    // MAX_HEAP: From leaves to roots, the keys are ascending
                    while (curr && prev->key > curr->key)
                    {
                        // Swapping the key
                        tmp = curr->key;
                        curr->key = prev->key;
                        prev->key = tmp;

                        prev = curr;
                        curr = curr->parent;
                    }
                }
                else
                {
                    // MIN_HEAP: From leaves to roots, the keys are descending
                    while (curr && prev->key < curr->key)
                    {
                        // Swapping the key
                        tmp = curr->key;
                        curr->key = prev->key;
                        prev->key = tmp;

                        prev = curr;
                        curr = curr->parent;
                    }
                }
            }
        }
        else
        {
            printf("[heap_insert()] ERROR: The given node isn't the root\n");
            free(item);
        }
    }
    else
    {
        printf("[heap_insert() -> item] ERROR: Memory allocation was unsuccessful\n");
    }

    return root;
}


/*
 *  Pops the top element of the heap and overwrites the
 *  given buffer with that key if all is ok, otherwise
 *  the latter is left untouched.
 *  If the buffer is NULL, the function allocates one
 *
 *  Time Complexity:    O(log(n))
 */
heap_t * heap_pop_front(heap_t *root, int **buf)
{
    heap_t *prev, *curr;
    uint8_t allocated, pos;


    if (root && root->parent == NULL)
    {
        if ((*buf) == NULL)
        {
            (*buf) = (int*)malloc(sizeof(int));
            allocated = TRUE;
        }

        if (buf)
        {
            (**buf) = root->key;
            pos = 0;
            prev = NULL;
            curr = root;

            if (root->heap_type == MAX_HEAP)
            {
                while (curr)
                {
                    prev = curr;

                    if (curr->left && curr->right)
                    {
                        // Case 1:  Confronting the next two keys is needed to 
                        //          estabilish the correct key as the new root

                        if (curr->left->key > curr->right->key)
                        {
                            curr->key = curr->left->key;
                            curr = curr->left;
                            pos = 0;
                        }
                        else
                        {
                            curr->key = curr->right->key;
                            curr = curr->right;
                            pos = 1;
                        }
                    }
                    else if (curr->left && curr->right == NULL)
                    {
                        // Case 2:  There is only the left branch, the next 
                        //          key just shift by one position upwards

                        curr->key = curr->left->key;
                        curr = curr->left;
                        pos = 0;
                    }
                    else if (curr->left == NULL && curr->right)
                    {
                        // Case 3:  There is only the right branch, the next 
                        //          key just shift by one position upwards

                        curr->key = curr->right->key;
                        curr = curr->right;
                        pos = 1;
                    }
                    else
                    {
                        // Case 4:  The key waterfall has reached the leaves, thus
                        //          it's time to delete prev, which is now pointing to the
                        //          leaf that was pointed by curr before being set to NULL
                        
                        curr = NULL;
                    }
                }
            }
            else
            {
                while (curr)
                {
                    prev = curr;

                    if (curr->left && curr->right)
                    {
                        // Case 1:  Confronting the next two keys is needed to 
                        //          estabilish the correct key as the new root

                        if (curr->left->key < curr->right->key)
                        {
                            curr->key = curr->left->key;
                            curr = curr->left;
                            pos = 0;
                        }
                        else
                        {
                            curr->key = curr->right->key;
                            curr = curr->right;
                            pos = 1;
                        }
                    }
                    else if (curr->left && curr->right == NULL)
                    {
                        // Case 2:  There is only the left branch, the next 
                        //          key just shift by one position upwards

                        curr->key = curr->left->key;
                        curr = curr->left;
                        pos = 0;
                    }
                    else if (curr->left == NULL && curr->right)
                    {
                        // Case 3:  There is only the right branch, the next 
                        //          key just shift by one position upwards

                        curr->key = curr->right->key;
                        curr = curr->right;
                        pos = 1;
                    }
                    else
                    {
                        // Case 4:  The key waterfall has reached the leaves, thus
                        //          it's time to delete prev, which is now pointing to the
                        //          leaf that was pointed by curr before being set to NULL
                        
                        curr = NULL;
                    }
                }
            }

            // Adjusting child-parent links
            // NOTE: prev points to the leaf to be deleted
            if (prev->parent)
            {
                if (pos == 0)
                {
                    prev->parent->left = NULL;
                }
                else
                {
                    prev->parent->right = NULL;
                }
            }
            else
            {
                root = NULL;
            }

            // Freeing memory
            free(prev);
        }
        else if (allocated == TRUE)
        {
            printf("[heap_pop_front() -> buf] ERROR: Memory allocation was unsuccessful. Could not retireve the heap's root key\n");
        }
    }

    return root;   
}


/*
 *  Clears the given heap completely
 *
 *  Time Complexity:    O(n)
 */
void heap_clear(heap_t **root)
{
    if (root && (*root))
    {
        heap_clear(&((*root)->left));
        heap_clear(&((*root)->right));
        free((*root));
        (*root) = NULL;
    }
}
