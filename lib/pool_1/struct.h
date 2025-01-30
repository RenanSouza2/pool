#ifndef __POOL_1_STRUCT_H__
#define __POOL_1_STRUCT_H__

#include <pthread.h>

#include "header.h"

STRUCT(pool_1)
{
    int size;
    long count;
    handler_p h;
    pthread_mutex_t lock;
};

#endif