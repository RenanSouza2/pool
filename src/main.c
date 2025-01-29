#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>

#include "../lib/pool_1/header.h"
#include "../lib/pool_2/header.h"
#include "../utils/assert.h"



float p_rand()
{
    int ceiling = 1e6;
    return (rand() % ceiling) / (float)ceiling;
}

float threshold(long n, long target)
{
    float _threshhold = 0.6 - 0.1 * n / (float)target;
    return _threshhold > 0.1 ? _threshhold : 0.1;
}



typedef int64_t *int64_p;

STRUCT(keep)
{
    int64_p h;
    keep_p next;
};

keep_p keep_create(int64_p h, keep_p next)
{
    keep_p k = malloc(sizeof(keep_t));
    assert(k);

    *k = (keep_t){h, next};
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

float test_time_case(char *tag, alloc_t f_alloc, free_t f_free)
{
    long lim = 1e10;
    long target = 1e8;

    printf("\n");
    long n = 0;
    keep_p k = NULL;
    
    double start = my_time();
    for(long i=0; i<lim; i++)
    {
        float p = p_rand();
        float _threshold = threshold(n, target);

        long interval = lim / 10;
        if(i%interval == interval - 1)
            printf("\n%s %ld/%ld %ld %.2f", tag, (i+1)/interval, lim/interval, n, _threshold);

        if(n == 0 || (p < _threshold))
        {
            int64_p h = f_alloc();
            *h = rand();
            k = keep_create(h, k);
            n++;
            
            continue;
        }

        handler_p h = k->h;
        f_free(h);

        keep_p k_next = k->next;
        free(k);
        k = k_next;
        n--;
    }
    double end = my_time();
    keep_free(k);

    return end - start;
}

void test_time()
{
    double time_malloc = test_time_case("malloc", my_malloc, free);

    pool_1_intialize(8);
    double time_palloc_1 = test_time_case("palloc_1", palloc_1, pfree_1);
    pool_1_clean();

    pool_2_intialize(8, 1e6);
    double time_palloc_2 = test_time_case("palloc_2", palloc_2, pfree_2);
    pool_2_clean();

    printf("\n");
    printf("\ntime malloc  : %.3f", time_malloc);
    printf("\ntime palloc 1: %.3f", time_palloc_1);
    printf("\ntime palloc 2: %.3f", time_palloc_2);
}



typedef long (*count_t)();

long test_space_case(char *tag, alloc_t f_alloc, free_t f_free, count_t f_count)
{
    long lim = 1e9;
    long target = 1e8;

    printf("\n");
    keep_p k = NULL;
    for(long i=0; i<target; i++)
    {
        int64_p h = f_alloc();
        *h = rand();
        k = keep_create(h, k);
    }

    long tot = 0;

    long n = target;
    target /= 10;
    for(long i=0; i<lim; i++)
    {
        float p = p_rand();
        float _threshold = threshold(n, target);

        long interval = lim / 100;
        if(i%interval == interval - 1)
            printf("\n%s %ld/%ld %ld %.2f", tag, (i+1)/interval, lim/interval, n, _threshold);

        tot += f_count();
        
        if(n == 0 || (p < _threshold))
        {
            int64_p h = f_alloc();
            *h = rand();
            k = keep_create(h, k);
            n++;
            
            continue;
        }

        handler_p h = k->h;
        f_free(h);

        keep_p k_next = k->next;
        free(k);
        k = k_next;
        n--;
    }
    keep_free(k);

    return tot;
}

void test_space()
{
    pool_1_intialize(8);
    long p1_res = test_space_case("pool 1", palloc_1, pfree_1, pool_1_count);
    pool_1_clean();

    printf("\n");
    printf("\npool 1: %ld", p1_res);
}



int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    printf("\nTemplate repo");

    srand(time(NULL));

    // test_time();
    test_space();
    
    printf("\n");
    return 0;
}



// time malloc  : 325.594
// time palloc 1: 294.143
// time palloc 2: 290.207

// time malloc  : 394.512
// time palloc 1: 361.110
// time palloc 2: 364.566