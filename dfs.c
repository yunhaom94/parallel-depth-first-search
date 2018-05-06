#include "tree.h"
#include "dfs.h"

DFSResult *init_dfs_result()
{
    DFSResult *r = malloc(sizeof(DFSResult));
    r->goal = 0;
    r->checks = 0;
}

void destory_dfs_result(DFSResult * dfsresult)
{
    free(dfsresult);
}

// A generic iterative DFS
// return 1 for found
// return 0 for not found
int depth_first_search(Tree *tree, int goal, DFSResult *result)
{
    int bf = tree->bf;
    int depth = 0;

    GArray *open_list = g_array_new(FALSE, TRUE, sizeof(SearchStep *));
    
    checked_count = 0;

    SearchStep *root = malloc(sizeof(SearchStep));
    root->node = tree->root;
    root->depth = 0;

    g_array_append_val(open_list, root);

    while (open_list->len > 0)
    {
        // pop node from open list
        // get the last item, then remove from the list
        int len = open_list->len;
        SearchStep *node_step = g_array_index(open_list, SearchStep *, 0);
        Node *node = node_step->node;
        int depth = node_step->depth;
        g_array_remove_index(open_list, 0);
        free(node_step);


        if (node->value == goal)
        {

            result->checks = checked_count + 1;
            result->goal = goal;

            g_array_free(open_list, FALSE);
            return 1;
        }

        checked_count += 1; // increment # of expanded nodes

        if (get_children(node))
        {
            for (int i = 0; i < bf; i++)
            {
                SearchStep *child = malloc(sizeof(SearchStep));
                child->node = get_children(node)[i];
                child->depth = depth + 1;

                g_array_append_val(open_list, child);
            }
        }
    }

    result->goal = -1;

    g_array_free(open_list, FALSE);
    return 0;
}


/*=============================================================================*/

// Idea is one thread only search one branch at a time (always go left) and
// put rest of children into the open list, instead of wait for one thread to finish
// searching, another thread take over and search another branch
int parallel_dfs(Tree *tree, int goal, int num_threads, DFSResult *result)
{   
    checked_count = 0;
    total_threads = num_threads;

    int bf = tree->bf;
    int depth = 0;
    GArray *open_list = g_array_new(FALSE, TRUE, sizeof(SearchStep *));

    SearchStep *root = malloc(sizeof(SearchStep));
    root->node = tree->root;
    root->depth = 0;
    g_array_append_val(open_list, root);

    pthread_t threads[num_threads];
    DFSThreadArgs args;
    args.open_list = open_list;
    args.bf = bf;
    args.goal = goal;
    args.result = result;

    pthread_cond_init(&found, NULL);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&open_list_lock, NULL);
    pthread_mutex_init(&thread_count_lock, NULL);


    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_threads; i++)
    {
        int th = pthread_create(&threads[i], NULL, dfs_threads, (void *)&args);
        if (th)
        {
            printf("ERROR in threads creation\n");
            g_array_free(open_list, FALSE);
            exit(-1);
        }
    }

    void *is_found;
    
    // use conditional variable to wait for a thread to find goal
    pthread_cond_wait(&found, &mutex); 
    // get the thread that reached the goal, in case other thread changed it
    pthread_t tid = thread_found; 
    pthread_mutex_unlock(&mutex);

    int th = pthread_join(tid, &is_found);
    
    if (th)
    {
        printf("ERROR in threads join\n");
        g_array_free(open_list, FALSE);
        exit(-1);
    }

    // cancel other threads, if there is any
    for (int i = 0; i < num_threads; i++)
    {
        if (threads[i] != tid)
            pthread_cancel(threads[i]);
    }

    g_array_free(open_list, FALSE);
    return (long)is_found;

}

void *dfs_threads(void *thread_args)
{

    DFSThreadArgs *args = (DFSThreadArgs *)thread_args;
    GArray *open_list = args->open_list;
    int bf = args->bf;
    int goal = args->goal;
    DFSResult *result = args->result;

    while (1)
    { // lock here
        pthread_mutex_lock(&open_list_lock);
        int len = open_list->len;

        // if nothing found and open list is empty
        if (len <= 0)
        {
            pthread_mutex_unlock(&open_list_lock);

            pthread_mutex_lock(&thread_count_lock);
            // if current thread is the last thread
            if (total_threads == 1)
            {
                total_threads -= 1;
                pthread_mutex_lock(&mutex);
                // declare no result is found
                thread_found = pthread_self();
                pthread_cond_signal(&found);
                pthread_mutex_unlock(&mutex);

                pthread_mutex_unlock(&thread_count_lock);
                if (!result->goal)
                    result->goal = -1;
                return (void *)0;
            } else {
                // if there is still other thread running
                // decrement total thread count
                total_threads -= 1;
                pthread_mutex_unlock(&thread_count_lock);
                pthread_exit(NULL);
            }
        }

        // pop from open list
        SearchStep *node_step = g_array_index(open_list, SearchStep *, len - 1);
        g_array_remove_index(open_list, len - 1);
        pthread_mutex_unlock(&open_list_lock);

        int is_found = dfs_search_left(node_step, open_list, bf, goal, result);

        free(node_step);

        if (is_found == 1)
        {   
            // if current thread found a result
            // DO NOT decrement thread count, so that other threads
            // that didn't find a result will exit on their own
            // look at line 171 for reference
            pthread_mutex_lock(&mutex);
            thread_found = pthread_self();
            pthread_cond_signal(&found);
            pthread_mutex_unlock(&mutex);

            return (void *)1;
        }
    }
}

// return 1 for found
// return 0 for not found
int dfs_search_left(SearchStep *node_step, GArray *open_list, int bf, int goal, DFSResult *result)
{

    Node *node = node_step->node;
    int depth = node_step->depth;
    
    
    if (node->value == goal)
    {
        result->checks = checked_count + 1;
        result->goal = node->value;
        return 1;
    }
    
    checked_count += 1; // increment # of expanded nodes

    if (get_children(node))
    {   
        // push children to the openlist
        // except for the left most child
        for (int i = 1; i < bf; i++)
        {
            SearchStep *child = malloc(sizeof(SearchStep));
            child->node = get_children(node)[i];
            child->depth = depth + 1;

            // Lock here
            pthread_mutex_lock(&open_list_lock);
            g_array_append_val(open_list, child);
            pthread_mutex_unlock(&open_list_lock);
            // unlock here
        }

        // only search left child
        SearchStep *left_child = malloc(sizeof(SearchStep));
        left_child->node = get_children(node)[0];
        left_child->depth = depth + 1;
        
        return dfs_search_left(left_child, open_list, bf, goal, result);

    } else {
        if (!result->goal)
            result->goal = -1;
        return 0;
    }

}
