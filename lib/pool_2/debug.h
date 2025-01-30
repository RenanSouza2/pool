#ifndef __POOL_2_DEBUG_H__
#define __POOL_2_DEBUG_H__

#include "struct.h"

#ifdef DEBUG

extern pool_2_t pool_2_global;

#endif

#define NEXT(POINTER) (*((handler_p*)(POINTER)))

#endif
