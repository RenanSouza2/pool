#ifndef __POOL_H__
#define __POOL_H__

#include "../../utils/struct.h"

void pool_intialize(int size, int clean_frequency);
void pool_clean();

handler_p palloc();
void pfree(handler_p h);

#endif
