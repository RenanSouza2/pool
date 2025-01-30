#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "../../utils/assert.h"



#ifdef DEBUG

#include "../../utils/clu/bin/header.h"

#endif



pool_1_t pool_1_global;

void pool_1_intialize(int size)
{
    pthread_mutex_init(&pool_1_global.lock, NULL);
    size = size > sizeof(handler_p) ? size : sizeof(handler_p);
    pool_1_global = (pool_1_t){size, 0, NULL};
}

void pool_1_clean()
{
    pthread_mutex_lock(&pool_1_global.lock);
    for(handler_p h = pool_1_global.h; h;)
    {
        handler_p h_aux = NEXT(h);
        free(h);
        h = h_aux;
    }
    pool_1_global.count = 0;
    pthread_mutex_unlock(&pool_1_global.lock);
}

long pool_1_count()
{
    pthread_mutex_lock(&pool_1_global.lock);
    return pool_1_global.count;
    pthread_mutex_unlock(&pool_1_global.lock);
}



handler_p palloc_1()
{
    pthread_mutex_lock(&pool_1_global.lock);
    if(pool_1_global.h)
    {
        handler_p h = pool_1_global.h;
        pool_1_global.h = NEXT(h);
        pool_1_global.count--;
        pthread_mutex_unlock(&pool_1_global.lock);
        return h;
    }

    handler_p h = calloc(1, pool_1_global.size);
    assert(h);
    pthread_mutex_unlock(&pool_1_global.lock);
    return h;
}

void pfree_1(handler_p h)
{
    pthread_mutex_lock(&pool_1_global.lock);
    NEXT(h) = pool_1_global.h;
    pool_1_global.h = h;
    pool_1_global.count++;
    pthread_mutex_unlock(&pool_1_global.lock);
}
