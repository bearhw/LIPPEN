
// test_entries_only.c
// RISC-V microbenchmark: PAC entry (signing) overhead - times only function prologues
//
// modify line 98-99, depth-trial, variables if needed
//
// Output reports cycles/entry using rdcycle.

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
/* Recursive function (entry-only timing)                       */
/* ------------------------------------------------------------ */
/*
 * Timing model:
 *  - In main:
 *      fence; t_start = rdcycle64(); fence
 *  - Recurse down to max_depth:
 *      each entry executes PAC signing (if enabled)
 *  - At deepest frame:
 *      fence; t_stop = rdcycle64(); fence
 *      return
 *
 * Timed region = ONLY function entries (PAC signing in prologue)
 */
__attribute__((noinline))
static uint64_t recurse_entry(int depth,
                              int max_depth,
                              uint64_t *t_stop)
{
    if (depth == max_depth) {
        fence_all();
        *t_stop = rdcycle64();
        fence_all();

        volatile uint64_t local[2];
        local[0] = (uint64_t)depth;
        local[1] = local[0] ^ (uint64_t)(uintptr_t)&local;
        return local[1];
    } else {
        uint64_t x = recurse_entry(depth + 1, max_depth, t_stop);

        /* Force a real stack frame so PAC signing is emitted */
        volatile uint64_t local[2];
        local[0] = (uint64_t)depth;
        local[1] = local[0] ^ (uint64_t)(uintptr_t)&local;

        (void)x; /* silence unused warning if optimized */

        return local[1];
    }
}

/* ------------------------------------------------------------ */

static int cmp_u64(const void *a, const void *b) {
    uint64_t ua = *(const uint64_t*)a;
    uint64_t ub = *(const uint64_t*)b;
    return (ua > ub) - (ua < ub);
}

int main(int argc, char **argv) {
    int depth   = (argc > 1) ? atoi(argv[1]) : 4096;  
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
        uint64_t te = 0;
        sink ^= recurse_entry(0, depth, &te);
    }

    for (int t = 0; t < trials; t++) {
        uint64_t t_start, t_stop = 0;

        fence_all();
        t_start = rdcycle64();
        fence_all();

        recurse_entry(0, depth, &t_stop);

        samples[t] = t_stop - t_start;
    }

    qsort(samples, (size_t)trials, sizeof(uint64_t), cmp_u64);

    uint64_t median_cyc = samples[trials / 2];
    double cyc_per_entry = (double)median_cyc / (double)depth;

    printf("PAC entry-only microbenchmark (rdcycle)\n");
    printf("depth=%d trials=%d\n\n", depth, trials);

    printf("per-trial results:\n");
    for (int t = 0; t < trials; t++) {
        double per_entry = (double)samples[t] / (double)depth;
        printf("  trial %2d: total=%llu cycles  per-entry=%.3f cycles\n",
               t,
               (unsigned long long)samples[t],
               per_entry);
    }

    printf("\nmedian:\n");
    printf("  total:     %llu cycles\n",
           (unsigned long long)median_cyc);
    printf("  per-entry: %.3f cycles\n",
           cyc_per_entry);

    free(samples);
    return (int)(sink & 1);
}


