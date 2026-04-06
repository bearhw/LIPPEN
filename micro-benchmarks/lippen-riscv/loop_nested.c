
//
// 2 leaf functions with different sizes are available. 
// you can use the leaf_recursive/leaf_recursive2 function in line 99 and 109
//
// Output reports cycles/(top-level call+ret).
// Each iteration performs `depth` nested calls internally.

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

// Recursive version.
// - noinline ensures real call/return
// - volatile local array forces stack frame
// - depth controls recursion depth
__attribute__((noinline))
static uint64_t leaf_recursive2(uint64_t x, int depth) {
    volatile uint64_t tmp[2];

    tmp[0] = x ^ 0x9e3779b97f4a7c15ULL;
    tmp[1] = tmp[0] + (uint64_t)(uintptr_t)&tmp;

    sink ^= tmp[1];

    if (depth > 0) {
        // Prevent tail-call elimination by using result afterward
        uint64_t r = leaf_recursive2(sink, depth - 1);
        sink ^= r;
        return sink;
    }

    return sink;
}

__attribute__((noinline))
static uint64_t leaf_recursive(uint64_t x, int depth) {
    volatile uint64_t tmp[2];
    sink = x ^ (x-5);

    if (depth > 0) {
        uint64_t r = leaf_recursive(sink, depth - 1);
        sink ^= r;
        return sink;
    }

    return sink;
}

int main(int argc, char **argv) {
    uint64_t iters  = (argc > 1) ? strtoull(argv[1], 0, 0) : 20000000ULL;
    int trials      = (argc > 2) ? atoi(argv[2]) : 7;
    int depth       = (argc > 3) ? atoi(argv[3]) : 38;

    if (trials < 1) trials = 1;
    if (depth < 0) depth = 0;

    uint64_t *samples = (uint64_t*)calloc((size_t)trials, sizeof(uint64_t));
    if (!samples) {
        printf("alloc failed\n");
        return 1;
    }

    // Warm-up
    for (int i = 0; i < 10000; i++)
        sink = leaf_recursive((uint64_t)i, depth);

    for (int t = 0; t < trials; t++) {
        fence_all();
        uint64_t x  = (uint64_t)t + 1;
        uint64_t c0 = rdcycle64();

        for (uint64_t i = 0; i < iters; i++) {
            // One top-level call per iteration,
            // but depth nested calls inside.
            x = leaf_recursive(x, depth);

            // Post-call work to defeat tail-call and maintain dependency
            x ^= (uint64_t)i;
        }

        uint64_t c1 = rdcycle64();
        samples[t] = c1 - c0;

        sink ^= x;
    }

    double cyc_per_iter = median_cyc_per_iter(samples, trials, iters);

    printf("Recursive return-address microbenchmark\n");
    printf("iters=%llu trials=%d depth=%d\n",
           (unsigned long long)iters, trials, depth);
    printf("median: %.3f cycles / (top-level call+ret)\n",
           cyc_per_iter);

    free(samples);
    return (int)(sink & 1);
}
