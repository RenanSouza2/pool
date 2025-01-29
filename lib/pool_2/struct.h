#ifndef __POOL_STRUCT_H__
#define __POOL_STRUCT_H__

#include "header.h"

STRUCT(pool)
{
    int size, clean_frequency, ticks, count;
    handler_p h;
};

#endif