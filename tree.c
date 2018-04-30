#include "tree.h"


Tree *init_tree(int d, int bf)
{
    Tree *tree = malloc(sizeof(Tree));
    tree->d = d;
    tree->bf = bf;
    
    Node *root = malloc(sizeof(Node));
    srand(time(NULL));
    root->value = rand() % 131; 
    tree->root = root;

    tree_generation(d, bf, root);

    return tree;
}


void tree_generation(int d, int bf, Node *node)
{
    if (d <= 0)
        return;

    int cur_depth = d - 1;
    node->children = malloc(sizeof(Node *) * bf);
    
    Node *child;
    for (int i = 0; i < bf; i++)
    {   
        child = malloc(sizeof(Node));
        
        int value = rand();
        
        child->value = value; 
        child->children = NULL;

        node->children[i] = child;
        tree_generation(cur_depth, bf, child);
    }

}


void clean_tree(Tree *tree)
{
    //
}


void print_tree(Tree *tree)
{
    int bf = tree->bf;
    print_tree_recursion(tree->root, 0, bf);

}

void print_tree_recursion(Node *node, int space, int bf)
{   

    if (node == NULL)
        return;
 
    // Increase distance between levels
    space += 10;
 
    // Process right child first
    
    Node *child;
    for (int i = 0; i < bf; i++)
    {   
        if (!node->children)
            child = NULL;
        else
            child = node->children[i];

        if (i == bf / 2)
        {
            printf("\n");
            for (int i = 10; i < space; i++)
                printf(" ");
            printf("%d\n", node->value);
        }

        print_tree_recursion(child, space, bf);

    }
}
