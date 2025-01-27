#include "../debug.h"
#include "../../../utils/assert.h"
#include "../../../utils/clu/bin/header.h"



void test_pool_palloc()
{
    printf("\n\t%s", __func__);

    handler_p h = palloc();
    assert(h);
    free(h);
    pool_clean();

    assert(clu_mem_empty());
}

void test_pool()
{
    printf("\n%s", __func__);

    test_pool_palloc();

    assert(clu_mem_empty());
}


int main() 
{
    setbuf(stdout, NULL);
    pool_intialize(8);
    test_pool();
    printf("\n\n\tTest successful\n\n");
    return 0;
}
