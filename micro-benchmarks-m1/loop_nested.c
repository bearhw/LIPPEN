//
//
// 2 leaf functions with different sizes are available. 
// you can use the leaf_recursive/leaf_recursive2 function in line 68 and 73
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach_time.h>
static uint64_t timebase_numer = 0;
static uint64_t timebase_denom = 0;
static inline uint64_t nsec_now(void) {
   uint64_t t = mach_absolute_time();
   // Convert to nanoseconds
   return (t * timebase_numer) / timebase_denom;
}
static int cmp_u64(const void *a, const void *b) {
   uint64_t ua = *(const uint64_t*)a;
   uint64_t ub = *(const uint64_t*)b;
   return (ua > ub) - (ua < ub);
}
static double median_per_iter(uint64_t *samples, int n, uint64_t iters) {
   qsort(samples, (size_t)n, sizeof(uint64_t), cmp_u64);
   return (double)samples[n/2] / (double)iters;
}
static volatile uint64_t sink = 1;
// Recursive function
__attribute__((noinline))
static uint64_t leaf_recursive2(uint64_t x, int depth) {
   volatile uint64_t tmp[2];
   tmp[0] = x ^ 0x9e3779b97f4a7c15ULL;
   tmp[1] = tmp[0] + (uint64_t)(uintptr_t)&tmp;
   sink ^= tmp[1];
   if (depth > 0) {
       uint64_t r = leaf_recursive2(sink, depth - 1);
       sink ^= r;          // prevents tail-call elimination
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
       sink ^= r;          // prevents tail-call elimination
       return sink;
   }
   return sink;
}
int main(int argc, char **argv) {
   mach_timebase_info_data_t tb;
   mach_timebase_info(&tb);
   timebase_numer = tb.numer;
   timebase_denom = tb.denom;
   uint64_t iters  = (argc > 1) ? strtoull(argv[1], 0, 0) : 20000000ULL;
   int trials      = (argc > 2) ? atoi(argv[2]) : 7;
   int depth       = (argc > 3) ? atoi(argv[3]) : 38;
   if (trials < 1) trials = 1;
   if (depth < 0) depth = 0;
   uint64_t *samples = calloc((size_t)trials, sizeof(uint64_t));
   if (!samples) {
       printf("alloc failed\n");
       return 1;
   }
   // Warm-up (stabilize caches + branch predictor)
   for (int i = 0; i < 10000; i++)
       sink = leaf_recursive((uint64_t)i, depth);
   for (int t = 0; t < trials; t++) {
       uint64_t x  = (uint64_t)t + 1;
       uint64_t t0 = nsec_now();
       for (uint64_t i = 0; i < iters; i++) {
           x = leaf_recursive(x, depth);
           x ^= (uint64_t)i;   // defeat optimization
       }
       uint64_t t1 = nsec_now();
       samples[t] = t1 - t0;
       sink ^= x;
   }
   double ns_top = median_per_iter(samples, trials, iters);
   double ns_per_call = ns_top / (double)(depth + 1);
   printf("Recursive call/return microbenchmark (macOS / Apple Silicon)\n");
   printf("iters=%llu trials=%d depth=%d\n",
          (unsigned long long)iters, trials, depth);
   printf("median: %.3f ns / (top-level call+ret)\n", ns_top);
   printf("derived: %.3f ns / (individual call+ret)\n", ns_per_call);
   free(samples);
   return (int)(sink & 1);
}

