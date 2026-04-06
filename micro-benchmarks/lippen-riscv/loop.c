
//
// 2 leaf functions with different sizes are available. 
// you can use the leaf/leaf2 function in line 74 and 83
//
// Output reports cycles/(call+ret) using rdcycle.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline void fence_all(void) {
    __asm__ __volatile__("fence rw, rw; fence.i" ::: "memory");
}

static inline uint64_t rdcycle64(void) {
#if defined(__riscv) && (__riscv_xlen == 64)
    uint64_t x;
    __asm__ __volatile__("rdcycle %0" : "=r"(x));
    return x;
#else
    uint32_t hi0, lo, hi1;
    __asm__ __volatile__("rdcycleh %0" : "=r"(hi0));
    __asm__ __volatile__("rdcycle  %0" : "=r"(lo));
    __asm__ __volatile__("rdcycleh %0" : "=r"(hi1));
    if (hi0 != hi1) {
        __asm__ __volatile__("rdcycle %0" : "=r"(lo));
    }
    return ((uint64_t)hi1 << 32) | lo;
#endif
}

static int cmp_u64(const void *a, const void *b) {
    uint64_t ua = *(const uint64_t*)a, ub = *(const uint64_t*)b;
    return (ua > ub) - (ua < ub);
}

static double median_cyc_per_iter(uint64_t *samples_cyc, int n, uint64_t iters) {
    qsort(samples_cyc, (size_t)n, sizeof(uint64_t), cmp_u64);
    return (double)samples_cyc[n/2] / (double)iters;
}

static volatile uint64_t sink = 1;

// noinline ensures an actual call/return.
// The local volatile array forces a real stack frame so RA save/restore behavior is exercised.
__attribute__((noinline)) static uint64_t leaf2(uint64_t x) {
    volatile uint64_t tmp[2];
    tmp[0] = x ^ 0x9e3779b97f4a7c15ULL;
    tmp[1] = tmp[0] + (uint64_t)(uintptr_t)&tmp;

    sink ^= tmp[1];
    return sink;
}

__attribute__((noinline)) static uint64_t leaf(uint64_t x) {
    volatile uint64_t tmp[2];
    tmp[0] = x ^ (x-5);
    return tmp[0];
}

int main(int argc, char **argv) {
    uint64_t iters  = (argc > 1) ? strtoull(argv[1], 0, 0) : 200000000ULL;
    int trials      = (argc > 2) ? atoi(argv[2]) : 7;
    if (trials < 1) trials = 1;

    uint64_t *samples = (uint64_t*)calloc((size_t)trials, sizeof(uint64_t));
    if (!samples) {
        printf("alloc failed\n");
        return 1;
    }

    // Warm-up (bring code/data into caches, stabilize predictors)
    for (int i = 0; i < 10000; i++) sink = leaf((uint64_t)i);

    for (int t = 0; t < trials; t++) {
        fence_all();
        uint64_t x  = (uint64_t)t + 1;
        uint64_t c0 = rdcycle64();

        for (uint64_t i = 0; i < iters; i++) {
            // One call + return per iteration.
            x = leaf(x);

            // Post-call work to defeat tail-call and keep dependency chain.
            x ^= (uint64_t)i;
        }

        uint64_t c1 = rdcycle64();
        samples[t] = c1 - c0;

        // Keep live
        sink ^= x;
    }

    double cyc_call = median_cyc_per_iter(samples, trials, iters);

    printf("Return-address microbenchmark (calls/returns)\n");
    printf("iters=%llu trials=%d\n", (unsigned long long)iters, trials);
    printf("median: %.3f cycles / (call+ret)\n", cyc_call);

    free(samples);
    return (int)(sink & 1);
}
