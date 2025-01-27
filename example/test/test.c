#include "../debug.h"
#include "../../../utils/assert.h"
#include "../../../utils/clu/bin/header.h"



void test_hello()
{
    printf("\n\t%s", __func__);

    hello_debug();
    hello_example();
}

void test_malloc()
{
    printf("\n\t%s", __func__);

    handler_p h = hello_malloc();

    // Uncomment line under to see assertion fail
    // assert(clu_mem_empty());

    printf("\nThis prints the current allocated pointers");
    printf("\n");
    clu_mem_report("TAG");

    free(h);

    assert(clu_mem_empty());
}

void test_example()
{
    printf("\n%s", __func__);

    test_hello();
    test_malloc();

    assert(clu_mem_empty());
}


int main() 
{
    setbuf(stdout, NULL);
    test_example();
    printf("\n\n\tTest successful\n\n");
    return 0;
}
