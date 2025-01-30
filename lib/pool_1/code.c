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
    size = size > sizeof(handler_p) ? size : sizeof(handler_p);
    pool_1_global = (pool_1_t){size, 0, NULL};
}

void pool_1_clean()
{
    for(handler_p h = pool_1_global.h; h;)
    {
        handler_p h_aux = NEXT(h);
        free(h);
        h = h_aux;
    }
}

long pool_1_count()
{
    return pool_1_global.count;
}



handler_p palloc_1()
{
    if(pool_1_global.h)
    {
        handler_p h = pool_1_global.h;
        pool_1_global.h = NEXT(h);
        pool_1_global.count--;
        return h;
    }

    handler_p h = calloc(1, pool_1_global.size);
    assert(h);
    return h;
}

void pfree_1(handler_p h)
{
    NEXT(h) = pool_1_global.h;
    pool_1_global.h = h;
    pool_1_global.count++;
}
