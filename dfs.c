#include "tree.h"
#include "dfs.h"

DFS_Result depth_first_search(Tree *tree, int goal)
{
    Node *start = tree->root;
    int bf = tree->bf;

    GArray *open_list = g_array_new(FALSE, TRUE, sizeof(Node *));
    GArray *path = g_array_new(FALSE, TRUE, sizeof(Node *));

    DFS_Result result;

    g_array_append_val(open_list, start);

    while (open_list->len > 0)
    {
        // pop node from open list
        int len = open_list->len;
        Node *node = g_array_index(open_list, Node *, len - 1);
        g_array_remove_index(open_list, len - 1);

        if (node->value == goal)
        {
            //TODO: return result struct
            result.goal = goal;
            return result;
        }

        for(int i = 0; i < bf; i++)
        {
            Node *child = node->children[i];
            g_array_append_val(open_list, child);
        }

    }

}