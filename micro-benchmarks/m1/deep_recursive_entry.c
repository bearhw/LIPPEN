// Apple Silicon (macOS): PAC-RET *entry* (PACIASP) microbenchmark
// Times only function prologues (no returns).
// Uses mach_absolute_time() and reports nanoseconds.
#if !defined(__APPLE__) || !defined(__aarch64__)
#error "This benchmark targets Apple Silicon (arm64) on macOS."
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach_time.h>
/* ------------------------------------------------------------ */
/* Low-level helpers                                            */
/* ------------------------------------------------------------ */
static inline void fence(void) {
   __asm__ __volatile__("dsb sy; isb" ::: "memory");
}
static inline uint64_t nsec_now(void) {
   static mach_timebase_info_data_t tb = {0};
   if (__builtin_expect(tb.denom == 0, 0))
       mach_timebase_info(&tb);
   uint64_t t = mach_absolute_time();
   return (t * tb.numer) / tb.denom;
}
static volatile uint64_t sink = 1;
/* ------------------------------------------------------------ */
/* Recursive function (entry-only timing)                       */
/* ------------------------------------------------------------ */
/*
* Timing model:
*  - In main:
*      fence; t_start = nsec_now(); fence
*  - Recurse down to max_depth:
*      each entry executes PACIASP
*  - At deepest frame:
*      fence; t_stop = nsec_now(); fence
*      return
*
* Timed region = ONLY function entries (PACIASP)
*/
__attribute__((noinline))
static uint64_t recurse_entry(int depth,
                             int max_depth,
                             uint64_t *t_stop)
{
   if (depth == max_depth) {
       fence();
       *t_stop = nsec_now();
       fence();
       volatile uint64_t local[2];
       local[0] = (uint64_t)depth;
       local[1] = local[0] ^ (uint64_t)(uintptr_t)&local;
       return local[1];
   } else {
       uint64_t x = recurse_entry(depth + 1, max_depth, t_stop);
       /* Force a real stack frame so paciasp is emitted */
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
       fence();
       t_start = nsec_now();
       fence();
       recurse_entry(0, depth, &t_stop);
       samples[t] = t_stop - t_start;
   }
   qsort(samples, (size_t)trials, sizeof(uint64_t), cmp_u64);
   uint64_t median_ns = samples[trials / 2];
   double ns_per_entry = (double)median_ns / (double)depth;
   printf("PACIASP entry-only microbenchmark (mach_absolute_time)\n");
   printf("depth=%d trials=%d\n\n", depth, trials);
   printf("per-trial results:\n");
   for (int t = 0; t < trials; t++) {
       double per_entry = (double)samples[t] / (double)depth;
       printf("  trial %2d: total=%llu ns  per-entry=%.3f ns\n",
              t,
              (unsigned long long)samples[t],
              per_entry);
   }
   printf("\nmedian:\n");
   printf("  total:     %llu ns\n",
          (unsigned long long)median_ns);
   printf("  per-entry: %.3f ns\n",
          ns_per_entry);
   free(samples);
   return (int)(sink & 1);
}

