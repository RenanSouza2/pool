#ifndef __POOL_2_H__
#define __POOL_2_H__

#include "../../utils/struct.h"

void pool_2_intialize(int size, int clean_frequency);
void pool_2_clean();
long pool_2_count();

handler_p palloc_2();
void pfree_2(handler_p h);

#endif
