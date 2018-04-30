#include "tree.h"
#include "dfs.h"

DFSResult depth_first_search(Tree *tree, int goal)
{
    int bf = tree->bf;
    int depth = 0;

    GArray *open_list = g_array_new(FALSE, TRUE, sizeof(SearchStep *));
    GArray *path = g_array_new(FALSE, TRUE, sizeof(Node *));

    DFSResult result;
    checked_count = 0;

    SearchStep *start = malloc(sizeof(SearchStep));
    start->node = tree->root;
    start->depth = 0;

    g_array_append_val(open_list, start);

    while (open_list->len > 0)
    {
        // pop node from open list
        // get the last item, then remove from the list
        int len = open_list->len;
        SearchStep *node_step = g_array_index(open_list, SearchStep *, len - 1);
        Node *node = node_step->node;
        int depth = node_step->depth;
        g_array_remove_index(open_list, len - 1);
        free(node_step);

        g_array_remove_range(path, depth, path->len - depth);
        g_array_append_val(path, node); 


        if (node->value == goal)
        {
            result.checks = checked_count + 1;
            result.goal = goal;
            result.path = path;

            g_array_free(open_list, FALSE);    
            return result;
        }

        checked_count += 1; // increment # of expanded nodes

        if (node->children)
        {
            for(int i = 0; i < bf; i++)
            {
                SearchStep *child = malloc(sizeof(SearchStep));
                child->node = node->children[i];
                child->depth = depth + 1;

                g_array_append_val(open_list, child);
            }
        }
    }

     g_array_free(open_list, FALSE);    

    result.goal = -1;
    return result;

}

void print_path(GArray *path)
{
    printf("The path leads to the goal is: ");
    for (int i = 0; i < path->len; i++)
    {
        Node *node = g_array_index(path, Node *, i);
        printf(" %d ", node->value);
        if (i != path->len - 1)
            printf("->");
    }

    printf("\n");
}