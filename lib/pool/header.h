#ifndef __POOL_H__
#define __POOL_H__

#include "../../utils/struct.h"

void pool_intialize(int size);
void pool_clean();

handler_p palloc();

#endif
