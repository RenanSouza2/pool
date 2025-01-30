#include "../debug.h"
#include "../../../utils/assert.h"
#include "../../../utils/clu/bin/header.h"



void test_pool_1_palloc()
{
    printf("\n\t%s", __func__);

    handler_p h = palloc_1();
    assert(h != NULL);
    assert(pool_1_global.h == NULL);
    free(h);
    pool_1_clean();

    assert(clu_mem_empty());
}

void test_pool_1_pfree(bool show)
{
    printf("\n\t%s", __func__);

    if(show) printf("\n\t\t%s 1", __func__);
    handler_p h1 = palloc_1();
    pfree_1(h1);
    assert(pool_1_global.h == h1);
    assert(NEXT(pool_1_global.h) == NULL);

    if(show) printf("\n\t\t%s 2", __func__);
    handler_p h2 = palloc_1();
    assert(h2 == h1);
    assert(pool_1_global.h == NULL);

    if(show) printf("\n\t\t%s 3", __func__);
    h2 = palloc_1();
    assert(h2 != h1);

    pfree_1(h2);
    pfree_1(h1);
    assert(pool_1_global.h == h1);
    assert(NEXT(pool_1_global.h) == h2);
    assert(NEXT(NEXT(pool_1_global.h)) == NULL);
    pool_1_clean();

    assert(clu_mem_empty());
}



void test_pool_1()
{
    printf("\n%s", __func__);

    test_pool_1_palloc();
    test_pool_1_pfree(true);

    assert(clu_mem_empty());
}


int main() 
{
    setbuf(stdout, NULL);
    pool_1_intialize(8);
    test_pool_1();
    printf("\n\n\tTest successful\n\n");
    return 0;
}
