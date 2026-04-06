//
//
// 2 leaf functions with different sizes are available. 
// you can use the leaf/leaf2 function in line 54 and 61
#if !defined(__APPLE__) || !defined(__aarch64__)
#error "This benchmark targets Apple Silicon (arm64) on macOS."
#endif
#define _DARWIN_C_SOURCE
#include <mach/mach_time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
static inline void fence(void) { __asm__ __volatile__("dsb sy; isb" ::: "memory"); }
static inline uint64_t nsec_now(void) {
   static mach_timebase_info_data_t tb = {0};
   if (__builtin_expect(tb.denom == 0, 0)) mach_timebase_info(&tb);
   uint64_t t = mach_absolute_time();
   return (t * tb.numer) / tb.denom;
}
static int cmp_u64(const void *a, const void *b) {
   uint64_t ua = *(const uint64_t*)a, ub = *(const uint64_t*)b;
   return (ua > ub) - (ua < ub);
}
static double median_ns_per_iter(uint64_t *samples_ns, int n, uint64_t iters) {
   qsort(samples_ns, (size_t)n, sizeof(uint64_t), cmp_u64);
   return (double)samples_ns[n/2] / (double)iters;
}
// Prevent inlining and prevent tail-call optimization by doing "real" work after call.
// Also keep a volatile dependency so the compiler can’t collapse the loop.
static volatile uint64_t sink = 1;
__attribute__((noinline)) static uint64_t leaf2(uint64_t x) {
   // Force a stack frame and SP-relative addressing.
   volatile uint64_t tmp[2];
   tmp[0] = x ^ 0x9e3779b97f4a7c15ULL;
   tmp[1] = tmp[0] + (uint64_t)(uintptr_t)&tmp;
   sink ^= tmp[1];
    // tmp[0] = x ^ (x-5);
   return sink;
}
__attribute__((noinline)) static uint64_t leaf(uint64_t x) {
    // Force a stack frame and SP-relative addressing.
    volatile uint64_t tmp[2];
    tmp[0] = x ^ (x-5);
  
    return tmp[0];
}
int main(int argc, char **argv) {
   uint64_t iters  = (argc > 1) ? strtoull(argv[1], 0, 0) : 200000000ULL; // 200M default
   int trials      = (argc > 2) ? atoi(argv[2]) : 7;                      // median over 7
   if (trials < 1) trials = 1;
   uint64_t *samples = (uint64_t*)calloc((size_t)trials, sizeof(uint64_t));
   if (!samples) { fprintf(stderr, "alloc failed\n"); return 1; }
   // Warm-up
   for (int i = 0; i < 10000; i++) sink = leaf((uint64_t)i);
   for (int t = 0; t < trials; t++) {
       fence();
       uint64_t x = (uint64_t)t + 1;
       uint64_t n0 = nsec_now();
       for (uint64_t i = 0; i < iters; i++) {
           // One call + return per iteration.
           x = leaf(x);
           // Post-call work to defeat tail-call and keep dependency chain.
           x ^= (uint64_t)i;
       }
       uint64_t n1 = nsec_now();
       samples[t] = n1 - n0;
       // Keep live
       sink ^= x;
   }
   double ns_call = median_ns_per_iter(samples, trials, iters);
   printf("Return-address microbenchmark (calls/returns)\n");
   printf("iters=%llu trials=%d\n", (unsigned long long)iters, trials);
   printf("median: %.3f ns / (call+ret)\n", ns_call);
   free(samples);
   return (int)(sink & 1);
}

