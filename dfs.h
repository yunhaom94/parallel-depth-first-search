#include <glib.h>
#include <gmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>



typedef struct DFS_Result
{
    int goal; // if -1, then no goal is found
    int checks; // number of node checked
    double time_used; //run time
    GArray *path;


} DFSResult;

// keep track of path lead to a node
typedef struct Search_Step
{   
    Node *node;
    int depth;

} SearchStep;

int checked_count;

DFSResult depth_first_search(Tree *tree, int goal);
void print_path(GArray *path);