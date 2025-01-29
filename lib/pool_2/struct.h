#ifndef __POOL_2_STRUCT_H__
#define __POOL_2_STRUCT_H__

#include "header.h"

STRUCT(pool_2)
{
    int size;
    int clean_frequency;
    int ticks;
    int count;
    handler_p h;
};

#endif