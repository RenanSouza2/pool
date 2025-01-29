#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#include "../lib/pool_1/header.h"
#include "../utils/assert.h"

STRUCT(keep)
{
    handler_p h;
    keep_p next;
};

keep_p keep_create(handler_p h, keep_p next)
{
    keep_p k = malloc(sizeof(keep_t));
    assert(k);

    *k = (keep_t){h, next};
    return k;
}

typedef handler_p (*alloc_t)();
typedef void (*free_t)(handler_p);

handler_p my_malloc()
{
    return malloc(8);
}

void test(long lim, alloc_t f_alloc, free_t f_free)
{
    long n = 0;
    keep_p k = NULL;
    for(long i=0; i<lim; i++)
    {
        float p = (rand() % (int)1e6) / 1e6;
        float threshold = 0.6 - 0.2 * n / 1e9;

        if(i%(long)1e7 == 0)
            printf("\n%ld %ld %.2f", i/(long)1e7, n, threshold);

        if(n == 0 || (p < threshold))
        {
            handler_p h = f_alloc();
            k = keep_create(h, k);
            n++;
            
            continue;
        }

        // int r = rand() % n;
        // keep_p *root = &k;
        // for(int i=0; i<r; i++, root = &((*root)->next));
        
        handler_p h = k->h;
        f_free(h);

        keep_p k_next = k->next;
        free(k);
        k = k_next;
        n--;
    }
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    printf("\nTemplate repo");

    srand(time(NULL));
    // pool_1_intialize(8, 1e6);
    pool_1_intialize(8);
    // test(1e10, palloc, pfree);
    test(1e10, my_malloc, free);
    
    printf("\n");
    return 0;
}

// 30.133
// 39.832

// 82.98s
// 1:54.89

// 5:30.24
// 8:40.08

// 5:27.24

