#include "../debug.h"
#include "../../../utils/assert.h"
#include "../../../utils/clu/bin/header.h"



void test_pool_2_palloc()
{
    printf("\n\t%s", __func__);

    handler_p h = palloc_2();
    assert(h != NULL);
    assert(pool_2_global.h == NULL);
    free(h);
    pool_2_clean();

    assert(clu_mem_empty());
}

void test_pool_2_pfree(bool show)
{
    printf("\n\t%s", __func__);

    if(show) printf("\n\t\t%s 1", __func__);
    handler_p h1 = palloc_2();
    pfree_2(h1);
    assert(pool_2_global.h == h1);
    assert(NEXT(pool_2_global.h) == NULL);

    if(show) printf("\n\t\t%s 2", __func__);
    handler_p h2 = palloc_2();
    assert(h2 == h1);
    assert(pool_2_global.h == NULL);

    if(show) printf("\n\t\t%s 3", __func__);
    h2 = palloc_2();
    assert(h2 != h1);

    pfree_2(h2);
    pfree_2(h1);
    assert(pool_2_global.h == h1);
    assert(NEXT(pool_2_global.h) == h2);
    assert(NEXT(NEXT(pool_2_global.h)) == NULL);
    pool_2_clean();

    assert(clu_mem_empty());
}



void test_pool_2()
{
    printf("\n%s", __func__);

    test_pool_2_palloc();
    test_pool_2_pfree(false);

    assert(clu_mem_empty());
}


int main()
{
    setbuf(stdout, NULL);
    pool_2_intialize(8, 1e6);
    test_pool_2();
    printf("\n\n\tTest successful\n\n");
    return 0;
}
