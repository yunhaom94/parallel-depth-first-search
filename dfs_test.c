#include "tree.h"
#include "dfs.h"
#include <math.h>
#include <sys/time.h>

int get_right_most_value(Tree *tree);

int main(int argc, char **argv)
{

    if (argc != 4) 
    {
        printf("Incorrect number of arguments, USAGE: dfs_test <depth_of_tree> <branching_factor> <num_of_threads>\n");
        exit(0);
    }

    
    int d = atoi(argv[1]);
    int bf = atoi(argv[2]);
    int threads = atoi(argv[3]);

    struct timeval tv1, tv2;
    double time_used;

    Tree *tree = init_tree(d, bf);
    if (d < 5 && bf < 5)
    {   
        print_tree(tree);
    }
    else
        printf("Tree too big, do not print\n");

    printf("Tree with %ld nodes created\n", tree->count);

    int right_most = get_right_most_value(tree);
    printf("Right most value is: %d\n", right_most);

    printf("DFS searching: %d\n", right_most);

    gettimeofday(&tv1, NULL);
    DFSResult result = depth_first_search(tree, right_most);
    gettimeofday(&tv2, NULL);
    time_used = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec);
    
    printf("DFS: Goal %d found with %d checks in %f seconds\n", result.goal, result.checks, time_used);


    gettimeofday(&tv1, NULL);
    DFSResult *result_1 = parallel_dfs(tree, right_most, threads);
    gettimeofday(&tv2, NULL);
    time_used = (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec);

    printf("Parallel DFS with %d threads: Goal %d found with %d checks in %f seconds\n", threads, result_1->goal, result_1->checks, time_used);

    return 0;
}

// this rewrites the right most value to a negative number
// to make sure that (single threaded)DFS goes through the
// entire tree
int get_right_most_value(Tree *tree)
{
    int bf = tree->bf;
    Node *node = tree->root;

    while(node->children)
        node = node->children[bf - 1];
    
    node->value = 0 - node->value;
    return node->value;
}