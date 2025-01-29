#ifndef __POOL_DEBUG_H__
#define __POOL_DEBUG_H__

#include "struct.h"

#ifdef DEBUG

extern pool_1_t pool_1_global;

#endif

#define NEXT(POINTER) (*((handler_p*)(POINTER)))

#endif
