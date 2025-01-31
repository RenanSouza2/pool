#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "../../utils/assert.h"



#ifdef DEBUG

#include "../../utils/clu/bin/header.h"

#endif



pool_2_t pool_2_global;

void pool_2_intialize(int size, int clean_frequency)
{
    pthread_mutex_init(&pool_2_global.lock, NULL);
    pool_2_global = (pool_2_t){size, clean_frequency, 0, 0, NULL};
}

void pool_2_clean()
{
    pthread_mutex_lock(&pool_2_global.lock);
    for(handler_p h = pool_2_global.h; h;)
    {
        handler_p h_aux = NEXT(h);
        free(h);
        h = h_aux;
    }
    pthread_mutex_unlock(&pool_2_global.lock);
}

void pool_2_clean_half()
{
    long clean = pool_2_global.count / 2;
    pool_2_global.count -= clean;

    handler_p h = pool_2_global.h;
    for(long i=0; i<clean; i++)
    {
        handler_p h_aux = NEXT(h);
        free(h);
        h = h_aux;
    }
    pool_2_global.h = h;
}

long pool_2_count()
{
    return pool_2_global.count;
}


handler_p palloc_2()
{
    pthread_mutex_lock(&pool_2_global.lock);
    if(pool_2_global.h)
    {
        handler_p h = pool_2_global.h;
        pool_2_global.h = NEXT(h);
        pool_2_global.count--;
        pthread_mutex_unlock(&pool_2_global.lock);
        return h;
    }

    handler_p h = calloc(1, pool_2_global.size);
    assert(h);
    pthread_mutex_unlock(&pool_2_global.lock);
    return h;
}

void pfree_2(handler_p h)
{
    pthread_mutex_lock(&pool_2_global.lock);
    NEXT(h) = pool_2_global.h;
    pool_2_global.h = h;
    pool_2_global.count++;

    pool_2_global.ticks++;
    if(pool_2_global.ticks == pool_2_global.clean_frequency)
    {
        pool_2_global.ticks = 0;

        if(pool_2_global.count > 1000)
            pool_2_clean_half();
    }
    pthread_mutex_unlock(&pool_2_global.lock);
}
