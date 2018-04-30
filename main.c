#include "tree.h"
#include "dfs.h"
#include <math.h>

int get_right_most_value(Tree *tree);

int main(int argc, char **argv)
{
    int d = 8;
    int bf = 4;

    Tree *tree = init_tree(d, bf);
    if (d < 5 && bf < 5)
    {   
        print_tree(tree);
    }


    int right_most = get_right_most_value(tree);
    printf("Right most value is: %d\n", right_most);

    printf("DFS searching: %d\n", right_most);

    DFSResult result = depth_first_search(tree, right_most);

    if (result.goal == -1)
        printf("No goal found with with %d checks in %f seconds\n", result.checks, result.time_used);

    printf("Goal %d found with %d checks in %f seconds\n", result.goal, result.checks, result.time_used);
    print_path(result.path);

    return 0;
}

int get_right_most_value(Tree *tree)
{
    int bf = tree->bf;
    Node *node = tree->root;

    while(node->children)
        node = node->children[0];
    
    return node->value;
}