#include "tree.h"


Tree *init_tree(int d, int bf)
{
    Tree *tree = malloc(sizeof(Tree));
    tree->d = d;
    tree->bf = bf;
    tree->count = 1;
    
    Node *root = malloc(sizeof(Node));
    srand(time(NULL));
    root->value = rand() % 131; 
    tree->root = root;

    tree_generation(d, bf, root, tree);

    return tree;
}


void tree_generation(int d, int bf, Node *node, Tree* tree)
{
    if (d <= 0)
        return;

    int cur_depth = d - 1;
    node->children = malloc(sizeof(Node *) * bf);
    
    Node *child;
    for (int i = 0; i < bf; i++)
    {   
        child = malloc(sizeof(Node));
        tree->count += 1;

        int value = rand();
        
        child->value = value; 
        child->children = NULL;

        node->children[i] = child;
        tree_generation(cur_depth, bf, child, tree);
    }

}


Node **get_children(Node *node)
{
    return node->children;
}

void destory_tree(Tree *tree)
{
    int bf = tree->bf;
    destory_tree_recursion(tree->root, bf);
    free(tree);

}

void destory_tree_recursion(Node *node, int bf)
{
    if (node == NULL)
       return;
  
    Node *child;
    for (int i = 0; i < bf; i++)
    {   
        if (!node->children)
            child = NULL;
        else
            child = node->children[i];

        destory_tree_recursion(child, bf);

    }

    free(node->children);
    free(node);
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
