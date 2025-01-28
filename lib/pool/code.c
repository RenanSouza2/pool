#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "../../utils/assert.h"



#ifdef DEBUG

#include "../../utils/clu/bin/header.h"

#endif



pool_t pool_global;

void pool_intialize(int size, int clean_frequency)
{
    pool_global = (pool_t){size, clean_frequency, 0, 0, NULL};
}

void pool_clean()
{
    for(handler_p h = pool_global.h; h;)
    {
        handler_p h_aux = NEXT(h);
        free(h);
        h = h_aux;
    }
}

void pool_clean_half()
{
    int clean = pool_global.count / 2;
    pool_global.count -= clean;

    handler_p h = pool_global.h;
    for(int i=0; i<clean; i++)
    {
        handler_p h_aux = NEXT(h);
        free(h);
        h = h_aux;
    }
    pool_global.h = h;
}



handler_p palloc()
{
    if(pool_global.h)
    {
        handler_p h = pool_global.h;
        pool_global.h = NEXT(h);
        return h;
    }

    handler_p h = calloc(1, pool_global.size);
    assert(h);
    return h;
}

void pfree(handler_p h)
{
    NEXT(h) = pool_global.h;
    pool_global.h = h;

    pool_global.ticks++;
    if(pool_global.ticks == pool_global.clean_frequency)
        pool_clean_half();
}
