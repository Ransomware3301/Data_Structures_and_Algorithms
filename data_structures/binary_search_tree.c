/*
 *  Program:    Binary Search Tree (BST) library
 *  
 *  Made by:    ransomware3301 (https://www.github.com/ransomware3301)
 *  Date:       26/07/2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define FALSE 0
#define TRUE 1
#define FORMAT "%d"


/* ==== Type Definitions ==== */


typedef int  item_t;
typedef unsigned char boolean_t;


typedef struct BST_node
{
    item_t key;
    struct BST_node *parent;
    struct BST_node *left;
    struct BST_node *right;
}
BST_node_t;


/* ==== Functions Declarations ==== */


// I/O
void show_usage(char*);
void print_tree_in_order(BST_node_t*);
void print_2D_tree(BST_node_t*, size_t);


// Structure Manipulation
BST_node_t * insert(BST_node_t*, item_t);
BST_node_t * delete(BST_node_t*, item_t);
void         clear_tree(BST_node_t*);


// Querying
BST_node_t * search(BST_node_t*, item_t);
BST_node_t * max(BST_node_t*);
BST_node_t * min(BST_node_t*);
BST_node_t * successor(BST_node_t*);


/* ==== Main ==== */


int main(int argc, char **argv)
{
    BST_node_t *root, *ptr, *succ;
    size_t i, size;
    item_t *rand_arr;
    item_t item;
    int sign;

    
    // Initializations
    srand(time(NULL));
    root = NULL;
    rand_arr = NULL;

    if (argc == 2)
    {
        size = atoi(argv[1]);

        if (size > 0)
        {
            if (( rand_arr = (item_t*)malloc(sizeof(item_t) * size) ))
            {
                sign = 1;

                // Array of unique values from [0, size-1] in random positions
                for (i = 0; i < size; i++)
                {
                    *(rand_arr + i) = sign * ((item_t) (rand() % size));
                    sign *= -1;
                }

                printf("\nGenerated + Filtered sequence: [ ");

                for (i = 0; i < size; i++)
                {
                    if (search(root, *(rand_arr + i)) == NULL)
                    {
                        root = insert(root, *(rand_arr + i));
                        printf(FORMAT" ", *(rand_arr + i));
                    }
                }
                
                printf("]\n\n\n");

                print_tree_in_order(root);
                printf("\n\n");
                print_2D_tree(root, 0);

                printf("\nStatistics:");
                printf("\n - MAX_VALUE: "FORMAT, max(root)->key);
                printf("\n - MIN_VALUE: "FORMAT"\n\n", min(root)->key);

                printf("Digit an item to search for it: ");
                scanf(FORMAT, &item);
                   
                ptr = search(root, item);
                
                printf("ITEM: "FORMAT" | FOUND ?: %s (address=%p)\n", 
                    item, 
                    ptr ? "YES" : "NO", 
                    (void*) ptr
                );

                if (ptr)
                {
                    succ = successor(ptr);

                    if (succ)
                    {
                        printf("SUCCESSOR of "FORMAT" is: "FORMAT" (address=%p)\n\n", item, succ->key, (void*) succ);
                    }
                    else
                    {
                        printf("SUCCESSOR of "FORMAT" DOES NOT EXIST (it's equal to MAX)\n\n", item);
                    }
                }

                clear_tree(root);
                free(rand_arr);
            }
        }
        else
        {
            printf("\nERROR: Size and Range must both be greater than zero\n");
        }
    }
    else if (argc == 1)
    {
        printf("Insert value to add (0 to quit, -1 to delete an element): ");
        scanf(FORMAT, &item);

        while (item != 0)
        {
            if (item == -1)
            {
                printf("Insert value to delete (0 to quit): ");
                scanf(FORMAT, &item);

                if (item != 0)
                {
                    printf("\n ==== BEFORE DELETING "FORMAT" ====\n\n", item);
                    print_2D_tree(root, 0);
                    printf("\n");

                    root = delete(root, item);

                    printf(" ==== AFTER DELETING "FORMAT" ====\n\n", item);
                    print_2D_tree(root, 0);
                    printf("\n");
                }
            }
            else
            {
                root = insert(root, item);

                printf("\n\n");
                print_2D_tree(root, 0);
                printf("\n");
            }

            printf("Insert value to add (0 to quit, -1 to delete an element): ");
            scanf(FORMAT, &item);
        }

        // Freeing memory
        clear_tree(root);
    }
    else
    {   
        printf("\nERROR: Incorrect command line parameter input\n");
        show_usage(argv[0]);
    }
    
    return 0;
}


/* === Functions Definitions */


// Shows how to give command line input to the program
void show_usage(char *progname)
{
    printf("\nUsage:\t%s [SIZE]\n\n", progname);
    printf("Legend:\n");
    printf(" - [SIZE] = Size of the binary search tree\n");
    printf("   (If SIZE is not inserted, then the program asks for manual input)\n");
}


// Prints the BST tree or sub-tree in order
void print_tree_in_order(BST_node_t *root)
{
    if (root)
    {
        print_tree_in_order(root->left);
        printf("[k=%d]\n", root->key);
        
        if (root->right)
        {
            printf(" |--(R)--> [k=%d]\n", root->right->key);
        }

        if (root->left)
        {
            printf(" |--(L)--> [k=%d]\n", root->left->key);
        }

        printf("\n");
        print_tree_in_order(root->right);
    }
}


// Prints the given BST or sub-BST in a tree-shape output
void print_2D_tree(BST_node_t *root, size_t depth)
{
    size_t i;


    if (root)
    {
        print_2D_tree(root->right, ++depth);
        depth--;

        for (i = 0; i < depth; i++)
        {
            printf("\t");
        }

        printf("[%d]", root->key);

        printf("\n\n");
        
        print_2D_tree(root->left, ++depth);
    }
}


// Inserts a new key in the given BST tree
BST_node_t * insert(BST_node_t *root, item_t key)
{
    BST_node_t *item, *curr, *prev;


    if (( item = (BST_node_t*)malloc(sizeof(BST_node_t)) ))
    {
        // Initializing the new node
        item->key = key;
        item->left = NULL;
        item->right = NULL;

        // Searching the insertion position
        curr = root;
        prev = NULL;

        while (curr)
        {
            prev = curr;

            if (key >= curr->key)
            {
                curr = curr->right;
            }
            else
            {
                curr = curr->left;
            }
        }

        // Adjusting links to the new node in the BST
        item->parent = prev;
        
        if (prev == NULL)
        {
            root = item;
        }
        else if (item->key < prev->key)
        {
            prev->left = item;
        }
        else
        {
            prev->right = item;
        }
    }
    else
    {
        printf("[insert() -> item] ERROR: Memory allocation was unsuccessful\n");
    }

    return root;
}


// Deletes the given key from the given BST tree
BST_node_t * delete(BST_node_t *root, item_t key)
{
    BST_node_t *item, *child;


    if (root && root->parent == NULL)
    {
        item = search(root, key);

        
        RECHECK: 
        if (item)
        {
            if (item->left && item->right)
            {
                // Case 1: the node to delete has both children
                child = successor(item);
                item->key = child->key;
                item = child;
                goto RECHECK;
            }
            else if (item->left && item->right == NULL)
            {
                // Case 2a: the node to delete has only a left children
                child = item->left;
            }
            else if (item->left == NULL && item->right)
            {
                // Case 2b: the node to delete has only a right children
                child = item->right;
            }
            else
            {
                // Case 3: the node to delete has no children
                child = NULL;
            }

            // Adjusting child-parent pointers in both directions
            // (1) child --> parent
            if (child)
            {
                child->parent = item->parent;
            }

            // (2) parent --> child
            if (item->parent)
            {
                if (item == item->parent->left)
                {
                    item->parent->left = child;
                }
                else
                {
                    item->parent->right = child;
                }
            }
            else
            {
                root = child;
            }
            
            free(item);
        }
    }

    return root;
}


// Clears the given tree from memory
void clear_tree(BST_node_t *root)
{
    if (root)
    {
        clear_tree(root->left);
        clear_tree(root->right);
        free(root);
    }
}


// Searches the given key in the given RB tree, returns the 
// address of the node with that key if is present, NULL otherwise 
BST_node_t * search(BST_node_t *root, item_t key)
{
    if (root == NULL || root->key == key)
    {
        return root;
    }
    
    if (key > root->key)
    {
        return search(root->right, key);
    }
    else
    {
        return search(root->left, key);
    }
}


// Returns the address of the node with the maximum key value
BST_node_t * max(BST_node_t *root)
{
    if (root)
    {
        while (root->right)
        {
            root = root->right;
        }
    }

    return root;
}


// Returns the address of the node with the minimum key value
BST_node_t * min(BST_node_t *root)
{
    if (root)
    {
        while (root->left)
        {
            root = root->left;
        }
    }

    return root;
}


// Returns the node address of the successor of the given node
// if present in the tree, NULL otherwise
BST_node_t * successor(BST_node_t *node)
{
    BST_node_t *node2;


    if (node->right != NULL)
    {
        return min(node->right);
    }
    
    node2 = node->parent;

    while (node2 && node2->right == node)
    {
        node = node2;
        node2 = node2->parent;
    }

    return node2;
}
