#include <stdlib.h>
#include <time.h>
#include <stdio.h>


typedef struct node
{
    int value;
    struct node **children;

} Node;


typedef struct tree 
{
    int d; // depths
    int bf; // branching factor
    long count; // # of nodes
    Node *root;

} Tree;



Tree *init_tree(int d, int bf);
void clean_tree(Tree *tree);
void tree_generation(int d, int bf, Node *node, Tree* tree);
void print_tree(Tree *tree);
void print_tree_recursion(Node *node, int space, int bf);