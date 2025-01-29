#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#include "../lib/pool_1/header.h"
#include "../lib/pool_2/header.h"
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

double my_time()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return now.tv_sec + now.tv_nsec*1e-9;
}

keep_p test(char *tag, alloc_t f_alloc, free_t f_free)
{
    long lim = 1e10;
    long target = 2e8;
    int ceiling = 1e6;

    printf("\n");
    long n = 0;
    keep_p k = NULL;
    for(long i=0; i<lim; i++)
    {
        float p = (rand() % ceiling) / (float)ceiling;
        float threshold = 0.6 - 0.2 * n / target;

        long interval = 1e7;
        if(i%interval == 0)
            printf("\n%s %ld/%ld %ld %.2f", tag, i/interval, lim/interval, n, threshold);

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
    return k;
}

void keep_free(keep_p k)
{
    while(k)
    {
        free(k->h);
        
        keep_p k_aux = k->next;
        free(k);
        k = k_aux;
    }
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    printf("\nTemplate repo");

    srand(time(NULL));

    double start, end;
    keep_p k;

    start = my_time();
    k = test("malloc", my_malloc, free);
    end = my_time();
    keep_free(k);
    double time_malloc = end - start;

    pool_1_intialize(8);
    start = my_time();
    k = test("palloc_1", palloc_1, pfree_1);
    end = my_time();
    pool_1_clean();
    keep_free(k);
    double time_palloc_1 = end - start;

    pool_2_intialize(8, 1e6);
    start = my_time();
    k = test("palloc_2", palloc_2, pfree_2);
    end = my_time();
    pool_2_clean();
    keep_free(k);
    double time_palloc_2 = end - start;

    printf("\n");
    printf("\ntime malloc  : %.3f", time_malloc);
    printf("\ntime palloc 1: %.3f", time_palloc_1);
    printf("\ntime palloc 2: %.3f", time_palloc_2);
    
    printf("\n");
    return 0;
}

// time malloc  : 325.594
// time palloc 1: 294.143
// time palloc 2: 290.207
