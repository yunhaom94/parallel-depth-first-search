#include <glib.h>
#include <gmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>



typedef struct DFS_Result
{
    int goal; // if -1, then no goal is found
    int checks; // number of node checked

} DFSResult;

// keep track of path lead to a node
typedef struct Search_Step
{   
    Node *node;
    int depth;

} SearchStep;

int checked_count;

typedef struct DFS_Thread_Args
{   
    GArray *open_list;
    int bf;
    int goal;

} DFSThreadArgs;

pthread_cond_t found;
pthread_mutex_t mutex;
pthread_t thread_found;
pthread_mutex_t open_list_lock;

int total_threads;
pthread_mutex_t thread_count_lock;

DFSResult depth_first_search(Tree *tree, int goal);
void print_path(GArray *path);

DFSResult *parallel_dfs(Tree *tree, int goal, int num_threads);
void *dfs_threads(void *thread_args);
DFSResult *dfs_search_left(SearchStep *node, GArray *open_list, int bf, int goal);