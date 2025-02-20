/*
 *  Program:    Red-Black tree (RB tree) library
 *  
 *  Made by:    ransomware3301 (https://www.github.com/ransomware3301)
 *  Date:       26/07/2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BLACK 0
#define RED 1
#define FALSE 0
#define TRUE 1
#define FORMAT "%s"
#define BUFSIZE 256
#define END_OF_STRING '\0'
#define QUIT_CMD "!q"
#define DEL_CMD "!d"


/* ==== Type Definitions ==== */


typedef unsigned char boolean_t;


typedef struct RB_node
{
    char *key;
    boolean_t color;
    struct RB_node *parent;
    struct RB_node *left;
    struct RB_node *right;
}
RB_node_t;


/* ==== Functions Declarations ==== */


// I/O
void show_usage(char*);
void print_tree_in_order(RB_node_t*);
void print_2D_tree(RB_node_t*, size_t);


// Structure Manipulation
RB_node_t * insert(RB_node_t*, char*);
RB_node_t * delete(RB_node_t*, char*);
RB_node_t * left_rotate(RB_node_t*, RB_node_t*);
RB_node_t * right_rotate(RB_node_t*, RB_node_t*);
void        clear_tree(RB_node_t*);


// Querying
RB_node_t * search(RB_node_t*, char*);
RB_node_t * max(RB_node_t*);
RB_node_t * min(RB_node_t*);
RB_node_t * successor(RB_node_t*);


/* ==== Main ==== */


int main(int argc, char **argv)
{
    RB_node_t *root, *succ, *ptr;
    char* item;

    
    // Initializations
    root = NULL;

    if (( item = (char*)malloc(sizeof(char) * (BUFSIZE + 1)) ))
    {
        *(item + BUFSIZE) = END_OF_STRING;

        printf("Insert value to add (\"%s\" to quit, \"%s\" to delete an element): ", QUIT_CMD, DEL_CMD);
        scanf(FORMAT, item);

        while (strcmp(item, QUIT_CMD) != 0)
        {
            if (strcmp(item, DEL_CMD) == 0)
            {
                printf("Insert value to delete (\"%s\" to quit): ", QUIT_CMD);
                scanf(FORMAT, item);

                if (strcmp(item, QUIT_CMD) != 0)
                {
                    printf("\n[BEFORE DELETING "FORMAT"]\n", item);
                    print_2D_tree(root, 0);
                    printf("\n");

                    root = delete(root, item);

                    printf("\n[AFTER DELETING "FORMAT"]\n", item);
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

            printf("Insert value to add (\"%s\" to quit, \"%s\" to delete an element): ", QUIT_CMD, DEL_CMD);
            scanf(FORMAT, item);
        }

        printf("\n\n[STATISTICS]:\n");
        printf(" - MAX: "FORMAT"\n", max(root)->key);
        printf(" - MIN: "FORMAT"\n\n", min(root)->key);

        printf("Insert string to find its successor: ");
        scanf(FORMAT, item);

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

        // Freeing memory
        clear_tree(root);
        free(item);
    }
    
    return 0;
}


/* ==== Functions Definitions ==== */


// Prints the BST tree or sub-tree in order
void print_tree_in_order(RB_node_t *root)
{
    if (root)
    {
        print_tree_in_order(root->left);
        printf("[k="FORMAT", c=%u]\n", root->key, root->color);

        if (root->right)
        {
            printf(" |--(R)--> [k="FORMAT", c=%u]\n", root->right->key, root->color);
        }
        
        if (root->left)
        {
            printf(" |--(L)--> [k="FORMAT", c=%u]\n", root->left->key, root->color);
        }
        
        printf("\n");

        print_tree_in_order(root->right);
    }
}


// Prints the given BST in a tree-shape output
void print_2D_tree(RB_node_t *root, size_t depth)
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

        if (root->parent == NULL)
            printf("(r) ");

        if (root->color == BLACK)
        {
            printf("["FORMAT", B]", root->key);
        }
        else
        {
            printf("["FORMAT", R]", root->key);
        }

        printf("\n\n");
        
        print_2D_tree(root->left, ++depth);
    }
}


// Inserts a new item and repairs the RB tree
RB_node_t * insert(RB_node_t *root, char *key)
{
    RB_node_t *item, *curr, *prev, *ptr;
    size_t i, pos, len_curr, len_key;
    boolean_t found;
    char *keyc;


    len_key = strlen(key);

    if (len_key > 0)
    {
        if (
            (item = (RB_node_t*)malloc(sizeof(RB_node_t)))
            &&
            (keyc = (char*)malloc(sizeof(char) * (len_key + 1)))
        )
        {
            strcpy(keyc, key);

            // Initializing the new node
            item->key = keyc;
            item->color = RED;
            item->left = NULL;
            item->right = NULL;

            // Searching the insertion position
            curr = root;
            prev = NULL;

            while (curr)
            {
                len_curr = strlen(curr->key);
                prev = curr;

                if (len_key < len_curr)
                {
                    curr = curr->left;
                    pos = 0;
                }
                else if (len_key > len_curr)
                {
                    curr = curr->right;
                    pos = 1;
                }
                else
                {
                    found = FALSE;
                    i = 0;
                    
                    while (!found && i < len_key)
                    {
                        if (*(key + i) > *(curr->key + i))
                        {
                            curr = curr->right;
                            found = TRUE;
                            pos = 1;
                        }
                        else if (*(key + i) < *(curr->key + i))
                        {
                            curr = curr->left;
                            found = TRUE;
                            pos = 0;
                        }
                        else
                        {
                            i++;
                        }
                    }

                    if (i == len_key)
                    {
                        curr = curr->right;
                        pos = 1;
                    }
                }
            }

            // Adjusting links to the new node in the BST
            item->parent = prev;

            if (prev == NULL)
            {
                root = item;
            }
            else if (pos == 1)
            {
                prev->right = item;
            }
            else if (pos == 0)
            {
                prev->left = item;
            }

            // Repairing the RB tree after insertion
            while (item->parent && item->parent->color == RED)
            {
                if (item->parent == item->parent->parent->left)
                {
                    ptr = item->parent->parent->right;
                    
                    if (ptr && ptr->color == RED)
                    {
                        item->parent->color = BLACK;
                        ptr->color = BLACK;
                        item->parent->parent->color = RED;
                        item = item->parent->parent;
                    }
                    else if (item == item->parent->right)
                    {
                        item = item->parent;
                        root = left_rotate(root, item);
                    }
                    else
                    {
                        item->parent->color = BLACK;
                        item->parent->parent->color = RED;
                        root = right_rotate(root, item->parent->parent);
                    }
                }
                else
                {   
                    ptr = item->parent->parent->left;
                    
                    if (ptr && ptr->color == RED)
                    {
                        item->parent->color = BLACK;
                        ptr->color = BLACK;
                        item->parent->parent->color = RED;
                        item = item->parent->parent;
                    }
                    else if (item == item->parent->left)
                    {
                        item = item->parent;
                        root = right_rotate(root, item);
                    }
                    else
                    {
                        item->parent->color = BLACK;
                        item->parent->parent->color = RED;
                        root = left_rotate(root, item->parent->parent);
                    }
                }
            }

            root->color = BLACK;
        }
        else
        {
            printf("[insert() -> item] ERROR: Memory allocation was not successful\n");
        }
    }
    else
    {
        printf("[insert() -> key] ERROR: The given key is empty or NULL\n");
    }

    return root;
}


// Deletes and repairs the new item from the RB tree
RB_node_t * delete(RB_node_t *root, char *key)
{
    RB_node_t *item, *item2, *del, *child;


    if (root && root->parent == NULL)
    {
        item = search(root, key);
        del = item;
        
        if (item)
        {
            RECALL: 
            if (item->color == RED)
            {
                item->color = BLACK;
            }
            else
            {
                if (item->parent)
                {
                    if (item == item->parent->left)
                    {
                        item2 = item->parent->right;

                        if (item2)
                        {
                            if (item2->color == RED)
                            {
                                // Case 1
                                item2->color = BLACK;
                                item2->parent->color = RED;
                                root = left_rotate(root, item->parent);
                            }
                            
                            if (item2->color == BLACK && item2->left && item2->right)
                            {
                                if (item2->left->color == BLACK && item2->right->color == BLACK)
                                {
                                    // Case 2
                                    if (item->right && item->left)
                                    {
                                        if (
                                            (item->left->color == BLACK && item->right->color == RED)
                                            ||
                                            (item->left->color == RED && item->right->color == BLACK)
                                        )
                                        {
                                            item2->color = RED;
                                            item = item->parent;
                                            goto RECALL;
                                        }
                                    }
                                }
                                else
                                {
                                    if (item2->left->color == RED && item2->right->color == BLACK)
                                    {
                                        // Case 3
                                        item2->color = RED;
                                        item2->left->color = BLACK;
                                        root = right_rotate(root, item2);
                                    }
                                    
                                    if (item2->right->color == RED)
                                    {
                                        // Case 4
                                        item2->color = item2->parent->color;
                                        item2->right->color = BLACK;
                                        root = left_rotate(root, item2->parent);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        item2 = item->parent->left;

                        if (item2)
                        {
                            if (item2->color == RED)
                            {
                                // Case 1
                                item2->color = BLACK;
                                item2->parent->color = RED;
                                root = right_rotate(root, item->parent);
                            }
                            
                            if (item2->color == BLACK && item2->left && item2->right)
                            {
                                if (item2->right->color == BLACK && item2->left->color == BLACK)
                                {
                                    // Case 2
                                    if (item->right && item->left)
                                    {
                                        if (
                                            (item->left->color == BLACK && item->right->color == RED)
                                            ||
                                            (item->left->color == RED && item->right->color == BLACK)
                                        )
                                        {
                                            item2->color = RED;
                                            item = item->parent;
                                            goto RECALL;
                                        }
                                    }
                                }
                                else
                                {
                                    if (item2->right->color == RED && item2->left->color == BLACK)
                                    {
                                        // Case 3
                                        item2->color = RED;
                                        item2->right->color = BLACK;
                                        root = left_rotate(root, item2);
                                    }
                                    
                                    if (item2->left->color == RED)
                                    {
                                        // Case 4
                                        item2->color = item2->parent->color;
                                        item2->left->color = BLACK;
                                        root = right_rotate(root, item2->parent);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Resetting the pointer to the actual element to delete
            item = del;

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
                
                // Freeing memory
                free(item->key);
                free(item);
            }
        }
    }

    return root;
}


// Left-rotates the subtree in the given node position of the given RB tree 
RB_node_t * left_rotate(RB_node_t *root, RB_node_t *node)
{
    RB_node_t *node2;
    
    
    if (root && node)
    {
        node2 = node->right;

        if (node2)
        {
            node->right = node2->left;

            if (node2->left != NULL)
            {
                node2->left->parent = node;
            }

            node2->parent = node->parent;

            if (node->parent == NULL)
            {
                root = node2;
            }
            else if (node == node->parent->left)
            {
                node->parent->left = node2;
            }
            else
            {
                node->parent->right = node2;
            }

            node2->left = node;
            node->parent = node2;
        }
    }

    return root;
}


// Right-rotates the subtree in the given node position of the given RB tree 
RB_node_t * right_rotate(RB_node_t *root, RB_node_t *node)
{
    RB_node_t *node2;


    if (root && node)
    {
        node2 = node->left;

        if (node2)
        {
            node->left = node2->right;

            if (node2->right != NULL)
            {
                node2->right->parent = node;
            }

            node2->parent = node->parent;

            if (node->parent == NULL)
            {
                root = node2;
            }
            else if (node == node->parent->right)
            {
                node->parent->right = node2;
            }
            else
            {
                node->parent->left = node2;
            }

            node2->right = node;
            node->parent = node2;
        }
    }

    return root;
}


// Searches the given key in the RB tree, returns the address
// if it's present, NULL otherwise
RB_node_t * search(RB_node_t *root, char *key)
{
    size_t len_key, len_curr, i;
    boolean_t found, pos;


    if (root == NULL || strcmp(key, root->key) == 0)
    {
        return root;
    }
    else if (key == NULL)
    {
        return NULL;
    }

    len_key = strlen(key);
    len_curr = strlen(root->key);

    if (len_key < len_curr)
    {
        return search(root->left, key);
    }
    else if (len_key > len_curr)
    {
        return search(root->right, key);
    }
    else
    {
        found = FALSE;
        i = 0;

        while (!found && i < len_key)
        {
            if (*(key + i) > *(root->key + i))
            {
                found = TRUE;
                pos = 1;
            }
            else if (*(key + i) < *(root->key + i))
            {
                found = TRUE;
                pos = 0;
            }
            else
            {
                i++;
            }
        }

        if (found == TRUE)
        {
            if (pos == 1)
            {
                return search(root->right, key);
            }
            else
            {
                return search(root->left, key);
            }
        }
        else
        {
            return root;
        }
    }
}


// Returns the address of the element with the highest key value
RB_node_t * max(RB_node_t *root)
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


// Returns the address of the element with the lowest key value
RB_node_t * min(RB_node_t *root)
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


// Returns the successor of the given element
RB_node_t * successor(RB_node_t *node)
{   
    RB_node_t *node2;


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


// Completely clears the given RB tree
void clear_tree(RB_node_t *root)
{
    if (root)
    {
        clear_tree(root->left);
        clear_tree(root->right);
        free(root->key);
        free(root);
    }
}
