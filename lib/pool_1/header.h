#ifndef __POOL_1_H__
#define __POOL_1_H__

#include "../../utils/struct.h"

void pool_1_intialize(int size);
void pool_1_clean();

handler_p palloc_1();
void pfree_1(handler_p h);

#endif
