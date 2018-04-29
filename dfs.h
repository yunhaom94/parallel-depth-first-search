#include <glib.h>
#include <gmodule.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct dfs_result
{
    int goal;
    int checks; // number of node checked
    GArray *path;
    

} DFS_Result;


DFS_Result depth_first_search(Tree *tree, int goal);