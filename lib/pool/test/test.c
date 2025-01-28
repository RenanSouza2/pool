#include "../debug.h"
#include "../../../utils/assert.h"
#include "../../../utils/clu/bin/header.h"



void test_pool_palloc()
{
    printf("\n\t%s", __func__);

    handler_p h = palloc();
    assert(h != NULL);
    assert(pool_global.h == NULL);
    free(h);
    pool_clean();

    assert(clu_mem_empty());
}

void test_pool_pfree(bool show)
{
    printf("\n\t%s", __func__);

    if(show) printf("\n\t\t%s 1", __func__);
    handler_p h1 = palloc();
    pfree(h1);
    assert(pool_global.h == h1);
    assert(NEXT(pool_global.h) == NULL);

    if(show) printf("\n\t\t%s 2", __func__);
    handler_p h2 = palloc();
    assert(h2 == h1);
    assert(pool_global.h == NULL);

    if(show) printf("\n\t\t%s 3", __func__);
    h2 = palloc();
    assert(h2 != h1);

    pfree(h2);
    pfree(h1);
    assert(pool_global.h == h1);
    assert(NEXT(pool_global.h) == h2);
    assert(NEXT(NEXT(pool_global.h)) == NULL);
    pool_clean();

    assert(clu_mem_empty());
}



void test_pool()
{
    printf("\n%s", __func__);

    test_pool_palloc();
    test_pool_pfree(false);

    assert(clu_mem_empty());
}


int main() 
{
    setbuf(stdout, NULL);
    pool_intialize(8, 1e6);
    test_pool();
    printf("\n\n\tTest successful\n\n");
    return 0;
}
