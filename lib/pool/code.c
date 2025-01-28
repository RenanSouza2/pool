#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "../../utils/assert.h"



#ifdef DEBUG

#include "../../utils/clu/bin/header.h"

#endif



pool_t pool_global;

void pool_intialize(int size)
{
    pool_global = (pool_t){size > sizeof(handler_p) ? size : sizeof(handler_p), NULL};
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
}
