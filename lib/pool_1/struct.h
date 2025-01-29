#ifndef __POOL_STRUCT_H__
#define __POOL_STRUCT_H__

#include "header.h"

STRUCT(pool)
{
    int size;
    handler_p h;
};

#endif