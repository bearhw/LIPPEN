// Apple Silicon (macOS): PAC-RET returns-only microbenchmark
// Uses mach_absolute_time() and prints per-trial timing in nanoseconds.
//
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
/* Recursive function (returns-only timing)                     */
/* ------------------------------------------------------------ */
/*
* Timing model:
*  - At deepest frame (depth == max_depth):
*      fence; t_start = nsec_now(); fence
*      return
*  - All returns propagate upward
*  - In main, immediately after recurse returns:
*      fence; t_stop = nsec_now(); fence
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
       fence();
       *t_start = nsec_now();
       fence();
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
       uint64_t ts = 0;
       sink ^= recurse(0, depth, &ts);
   }
   for (int t = 0; t < trials; t++) {
       uint64_t t_start = 0;
       recurse(0, depth, &t_start);
       fence();
       uint64_t t_stop = nsec_now();
       fence();
       samples[t] = t_stop - t_start;
   }
   qsort(samples, (size_t)trials, sizeof(uint64_t), cmp_u64);
   uint64_t median_ns = samples[trials / 2];
   double median_per_ret = (double)median_ns / (double)depth;
   printf("PAC-RET returns-only microbenchmark (mach_absolute_time)\n");
   printf("depth=%d trials=%d\n\n", depth, trials);
   printf("per-trial results:\n");
   for (int t = 0; t < trials; t++) {
       double per_ret = (double)samples[t] / (double)depth;
       printf("  trial %2d: total=%llu ns  per-ret=%.3f ns\n",
              t,
              (unsigned long long)samples[t],
              per_ret);
   }
   printf("\nmedian:\n");
   printf("  total:   %llu ns\n",
          (unsigned long long)median_ns);
   printf("  per-ret: %.3f ns\n",
          median_per_ret);
   free(samples);
   return (int)(sink & 1);
}

