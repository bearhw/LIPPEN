
// test_returns_only.c
// RISC-V microbenchmark: return-only overhead (recursive calls with timing around returns)
//
// modify line 90-91, depth-trial, variables if needed
//
// Output reports cycles/return using rdcycle.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------ */
/* Low-level helpers                                            */
/* ------------------------------------------------------------ */

static inline void fence_all(void) {
    __asm__ __volatile__("fence rw, rw; fence.i" ::: "memory");
}

static inline uint64_t rdcycle64(void) {
#if defined(__riscv) && (__riscv_xlen == 64)
    uint64_t x;
    __asm__ __volatile__("rdcycle %0" : "=r"(x));
    return x;
#else
    // RV32 safe pattern
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

static volatile uint64_t sink = 1;

/* ------------------------------------------------------------ */
/* Recursive function (returns-only timing)                     */
/* ------------------------------------------------------------ */
/*
 * Timing model:
 *  - At deepest frame (depth == max_depth):
 *      fence; t_start = rdcycle64(); fence
 *      return
 *  - All returns propagate upward
 *  - In main, immediately after recurse returns:
 *      fence; t_stop = rdcycle64(); fence
 *
 * Timed region = ONLY returns + epilogues
 */
__attribute__((noinline))
static uint64_t recurse(int depth,
                        int max_depth,
                        uint64_t *t_start)
{
    /* Force real stack frame and epilogue */
    volatile uint64_t local[2];
    local[0] = (uint64_t)depth * 0x9e3779b97f4a7c15ULL;
    local[1] = local[0] ^ (uint64_t)(uintptr_t)&local;

    if (depth == max_depth) {
        fence_all();
        *t_start = rdcycle64();
        fence_all();
        return local[1];
    }

    uint64_t x = recurse(depth + 1, max_depth, t_start);

    // Prevent tail-call elimination if needed
    // x ^= local[1];
    // sink ^= x;

    return x;
}

/* ------------------------------------------------------------ */

static int cmp_u64(const void *a, const void *b) {
    uint64_t ua = *(const uint64_t*)a;
    uint64_t ub = *(const uint64_t*)b;
    return (ua > ub) - (ua < ub);
}

int main(int argc, char **argv) {
    int depth   = (argc > 1) ? atoi(argv[1]) : 40960;
    int trials  = (argc > 2) ? atoi(argv[2]) : 11;

    if (depth < 1) depth = 1;
    if (trials < 1) trials = 1;

    uint64_t *samples = calloc((size_t)trials, sizeof(uint64_t));
    if (!samples) {
        fprintf(stderr, "alloc failed\n");
        return 1;
    }

    /* Warm-up */
    for (int i = 0; i < 1000; i++) {
        uint64_t ts = 0;
        sink ^= recurse(0, depth, &ts);
    }

    for (int t = 0; t < trials; t++) {
        uint64_t t_start = 0;

        recurse(0, depth, &t_start);

        fence_all();
        uint64_t t_stop = rdcycle64();
        fence_all();

        samples[t] = t_stop - t_start;
    }

    qsort(samples, (size_t)trials, sizeof(uint64_t), cmp_u64);

    uint64_t median_cyc = samples[trials / 2];
    double median_per_ret = (double)median_cyc / (double)depth;

    printf("PAC-RET returns-only microbenchmark (rdcycle)\n");
    printf("depth=%d trials=%d\n\n", depth, trials);

    printf("per-trial results:\n");
    for (int t = 0; t < trials; t++) {
        double per_ret = (double)samples[t] / (double)depth;
        printf("  trial %2d: total=%llu cycles  per-ret=%.3f cycles\n",
               t,
               (unsigned long long)samples[t],
               per_ret);
    }

    printf("\nmedian:\n");
    printf("  total:   %llu cycles\n",
           (unsigned long long)median_cyc);
    printf("  per-ret: %.3f cycles\n",
           median_per_ret);

    free(samples);
    return (int)(sink & 1);
}