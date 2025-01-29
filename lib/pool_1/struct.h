#ifndef __POOL_1_STRUCT_H__
#define __POOL_1_STRUCT_H__

#include "header.h"

STRUCT(pool_1)
{
    int size;
    long count;
    handler_p h;
};

#endif