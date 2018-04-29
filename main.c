#include "tree.h"
#include "dfs.h"

int get_right_most_value(Tree *tree);

int main(int argc, char **argv)
{
    int d = 3;
    int bf = 2;

    Tree *tree = init_tree(d, bf);
    print_tree(tree);

    int right_most = get_right_most_value(tree);
    printf("Right most value is: %d\n", right_most);

    printf("DFS searching: %d\n", right_most);

    //DFS_Result result = depth_first_search(tree, right_most);

    //printf("Goal %d\n found", result.goal);

    return 0;
}

int get_right_most_value(Tree *tree)
{
    int bf = tree->bf;
    Node *node = tree->root;

    while(node->children)
        node = node->children[bf - 1];
    
    return node->value;
}