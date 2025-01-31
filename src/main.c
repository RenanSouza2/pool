#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
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
typedef void (*setup_f)();
typedef long (*count_t)();

handler_p my_malloc()
{
    return malloc(8);
}

void null_setup() {}

void pool_1_setup()
{
    pool_1_intialize(8);
}

void pool_2_setup()
{
    pool_2_intialize(8, 1e6);
}

STRUCT(lib)
{
    char* tag;
    alloc_t f_alloc;
    free_t f_free;
    setup_f f_setup;
    setup_f f_clean;
    count_t f_count;
};

lib_t lib_malloc = (lib_t)
{
    .tag     = "malloc",
    .f_alloc = my_malloc,
    .f_free  = free,
    .f_setup = null_setup,
    .f_clean = null_setup,
    .f_count = NULL
};

lib_t lib_pool_1 = (lib_t)
{
    .tag     = "pool 1",
    .f_alloc = palloc_1,
    .f_free  = pfree_1,
    .f_setup = pool_1_setup,
    .f_clean = pool_1_clean,
    .f_count = pool_1_count
};

lib_t lib_pool_2 = (lib_t)
{
    .tag     = "pool 2",
    .f_alloc = palloc_2,
    .f_free  = pfree_2,
    .f_setup = pool_2_setup,
    .f_clean = pool_2_clean,
    .f_count = pool_2_count
};



double my_time()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return now.tv_sec + now.tv_nsec*1e-9;
}

float test_time_case(lib_t lib)
{
    long lim = 1e10;
    long target = 1e8;

    printf("\n");
    long n = 0;
    keep_p k = NULL;
    
    lib.f_setup();
    double start = my_time();
    for(long i=0; i<lim; i++)
    {
        float p = p_rand();
        float _threshold = threshold(n, target);

        long interval = lim / 10;
        if(i%interval == interval - 1)
            printf("\n%s %ld/%ld %ld %.2f", lib.tag, (i+1)/interval, lim/interval, n, _threshold);

        if(n == 0 || (p < _threshold))
        {
            int64_p h = lib.f_alloc();
            *h = rand();
            k = keep_create(h, k);
            n++;
            
            continue;
        }

        handler_p h = k->h;
        lib.f_free(h);

        keep_p k_next = k->next;
        free(k);
        k = k_next;
        n--;
    }
    double end = my_time();
    keep_free(k);
    lib.f_clean();

    return end - start;
}

void test_time()
{
    double time_malloc = test_time_case(lib_malloc);
    double time_palloc_1 = test_time_case(lib_pool_1);
    double time_palloc_2 = test_time_case(lib_pool_2);

    printf("\n");
    printf("\ntime malloc  : %.3f", time_malloc);
    printf("\ntime palloc 1: %.3f", time_palloc_1);
    printf("\ntime palloc 2: %.3f", time_palloc_2);
}



typedef handler_p (*f_thread)(handler_p);

long test_space_decreasing_case(lib_t lib)
{
    long lim = 1e9;
    long target = 1e8;

    lib.f_setup();

    printf("\n");
    keep_p k = NULL;
    for(long i=0; i<target; i++)
    {
        int64_p h = lib.f_alloc();
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

        long interval = lim / 10;
        if(i%interval == interval - 1)
            printf("\n%s %ld/%ld %ld %.2f", lib.tag, (i+1)/interval, lim/interval, n, _threshold);

        tot += lib.f_count();
        
        if(n == 0 || (p < _threshold))
        {
            int64_p h = lib.f_alloc();
            *h = rand();
            k = keep_create(h, k);
            n++;
            
            continue;
        }

        handler_p h = k->h;
        lib.f_free(h);

        keep_p k_next = k->next;
        free(k);
        k = k_next;
        n--;
    }
    keep_free(k);
    lib.f_clean();

    return tot;
}

void test_space_decreasing()
{
    long p1_res = test_space_decreasing_case(lib_pool_1);
    long p2_res = test_space_decreasing_case(lib_pool_2);

    printf("\n");
    printf("\npool 1: %ld", p1_res);
    printf("\npool 2: %ld", p2_res);
    printf("\nratio : %.3f", (float) p1_res / p2_res);
}

long test_space_rising_case(lib_t lib)
{
    long lim = 1e9;
    long target = 1e8;

    lib.f_setup();

    printf("\n");
    long n = 0;
    long tot = 0;
    keep_p k = NULL;

    for(long i=0; i<lim; i++)
    {
        float p = p_rand();
        float _threshold = threshold(n, target);

        long interval = lim / 10;
        if(i%interval == interval - 1)
            printf("\n%s %ld/%ld %ld %.2f", lib.tag, (i+1)/interval, lim/interval, n, _threshold);

        tot += lib.f_count();
        
        if(n == 0 || (p < _threshold))
        {
            int64_p h = lib.f_alloc();
            *h = rand();
            k = keep_create(h, k);
            n++;
            
            continue;
        }

        handler_p h = k->h;
        lib.f_free(h);

        keep_p k_next = k->next;
        free(k);
        k = k_next;
        n--;
    }
    keep_free(k);

    return tot;
}

void test_space_rising()
{
    long p1_res = test_space_rising_case(lib_pool_1);
    long p2_res = test_space_rising_case(lib_pool_2);

    printf("\n");
    printf("\npool 1: %ld", p1_res);
    printf("\npool 2: %ld", p2_res);
    printf("\nratio : %.3f", (float) p1_res / p2_res);
}



void* test_thread_case_thread(handler_p _lib)
{
    lib_p lib = (lib_p)_lib;
    long lim = 1e7;
    long target = 1e7;

    printf("\n");
    long n = 0;
    keep_p k = NULL;
    
    for(long i=0; i<lim; i++)
    {
        float p = p_rand();
        float _threshold = threshold(n, target);

        long interval = lim / 10;
        if(i%interval == interval - 1)
            printf("\n%s %ld/%ld %ld %.2f", lib->tag, (i+1)/interval, lim/interval, n, _threshold);

        if(n == 0 || (p < _threshold))
        {
            int64_p h = lib->f_alloc();
            *h = rand();
            k = keep_create(h, k);
            n++;
            
            continue;
        }

        handler_p h = k->h;
        lib->f_free(h);

        keep_p k_next = k->next;
        free(k);
        k = k_next;
        n--;
    }
    keep_free(k);

    printf("\nfinishing");

    return NULL;
}

float test_tread_case(lib_t lib)
{
    int nthreads = 2;
    pthread_t threads[nthreads];

    lib.f_setup();
    
    float start = my_time();

    for(int i=0; i<nthreads; i++)
        pthread_create(&threads[i], NULL, test_thread_case_thread, &lib);

    for(int i=0; i<nthreads; i++)
    {
        printf("\nwaiting %d", i);
        pthread_join(threads[i], NULL);
    }

    float end = my_time();
    lib.f_clean();
    return end - start;
}

void test_tread()
{
    float t_malloc = test_tread_case(lib_malloc);
    float t_pool_1 = test_tread_case(lib_pool_1);
    float t_pool_2 = test_tread_case(lib_pool_2);

    printf("\n");
    printf("\nmalloc: %.3f", t_malloc);
    printf("\npool 1: %.3f", t_pool_1);
    printf("\npool 2: %.3f", t_pool_2);
}


int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    printf("\nTemplate repo");

    srand(time(NULL));

    // test_time();
    // test_space_decreasing();
    // test_space_rising();
    test_tread();
    
    printf("\n");
    return 0;
}
