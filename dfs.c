#include "tree.h"
#include "dfs.h"

DFSResult depth_first_search(Tree *tree, int goal)
{
    int bf = tree->bf;
    int depth = 0;

    GArray *open_list = g_array_new(FALSE, TRUE, sizeof(SearchStep *));

    DFSResult result;
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

            result.checks = checked_count + 1;
            result.goal = goal;

            g_array_free(open_list, FALSE);
            return result;
        }

        checked_count += 1; // increment # of expanded nodes

        if (node->children)
        {
            for (int i = 0; i < bf; i++)
            {
                SearchStep *child = malloc(sizeof(SearchStep));
                child->node = node->children[i];
                child->depth = depth + 1;

                g_array_append_val(open_list, child);
            }
        }
    }

    result.goal = -1;

    g_array_free(open_list, FALSE);
    return result;
}





DFSResult *parallel_dfs(Tree *tree, int goal, int num_threads)
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
            exit(-1);
        }
    }

    void *result;

    // wait for conditional variable
    
    pthread_cond_wait(&found, &mutex);
    pthread_t tid = thread_found; // in case other thread changed it
    pthread_mutex_unlock(&mutex);

    int th = pthread_join(tid, &result);
    
    if (th)
    {
        printf("ERROR in threads join\n");
        exit(-1);
    }

    for (int i = 0; i < num_threads; i++)
    {
        if (threads[i] != tid)
            pthread_cancel(threads[i]);
    }

    DFSResult *final_result = (DFSResult *)result;

    return final_result;

}

void *dfs_threads(void *thread_args)
{

    DFSThreadArgs *args = (DFSThreadArgs *)thread_args;
    GArray *open_list = args->open_list;
    int bf = args->bf;
    int goal = args->goal;

    DFSResult *final_result = malloc(sizeof(DFSResult));

    while (1)
    { // lock here
        pthread_mutex_lock(&open_list_lock);
        int len = open_list->len;

        if (len <= 0)
        {
            pthread_mutex_unlock(&open_list_lock);

            pthread_mutex_lock(&thread_count_lock);
            if (total_threads == 1)
            {
                total_threads -= 1;
                pthread_mutex_lock(&mutex);
                thread_found = pthread_self();
                pthread_cond_signal(&found);
                printf("not found and signaled\n");
                pthread_mutex_unlock(&mutex);

                pthread_mutex_unlock(&thread_count_lock);
                final_result->goal = -1;
                return (void *)final_result;
            } else {
                total_threads -= 1;
                pthread_mutex_unlock(&thread_count_lock);
                pthread_exit(NULL);
            }
        }

        
        SearchStep *node_step = g_array_index(open_list, SearchStep *, len - 1);
        g_array_remove_index(open_list, len - 1);
        pthread_mutex_unlock(&open_list_lock);

        DFSResult *result = dfs_search_left(node_step, open_list, bf, goal);

        if (result->goal == goal)
        {   

            pthread_mutex_lock(&mutex);
            thread_found = pthread_self();
            pthread_cond_signal(&found);
            printf("found and signaled\n");
            pthread_mutex_unlock(&mutex);

            return (void *) result;
        }
    }
}

DFSResult *dfs_search_left(SearchStep *node_step, GArray *open_list, int bf, int goal)
{
    DFSResult *result = malloc(sizeof(DFSResult));

    Node *node = node_step->node;
    int depth = node_step->depth;

    if (node->value == goal)
    {
        result->checks = checked_count + 1;
        result->goal = node->value;
        return result;
    }
    
    checked_count += 1; // increment # of expanded nodes

    if (node->children)
    {
        for (int i = 1; i < bf; i++)
        {
            SearchStep *child = malloc(sizeof(SearchStep));
            child->node = node->children[i];
            child->depth = depth + 1;

            // Lock here
            pthread_mutex_lock(&open_list_lock);
            g_array_append_val(open_list, child);
            pthread_mutex_unlock(&open_list_lock);
            // unlock here
        }

        SearchStep *left_child = malloc(sizeof(SearchStep));
        left_child->node = node->children[0];
        left_child->depth = depth + 1;

        return dfs_search_left(left_child, open_list, bf, goal);

    } else {

        result->goal = -1;
        return result;
    }

}
