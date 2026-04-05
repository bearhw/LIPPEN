// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>  // For uintptr_t
// #include <unistd.h>



// int foo(int a) {
//     printf(" I am foo %p\n", (void *)&foo);     
//     return a * 2;
// }

// int bar(int b) {
//     printf(" I am bar %p\n", (void *)&bar);     
//     return b + 1;
// }

// int main() {
//     int x = 5;
//     int y = foo(x);
//     int z = bar(y);
//     printf("this is z %i", z);
//     return 0;
// }





// #include <stdio.h>
// #include <stdint.h>
// #include <inttypes.h>
// #include "pac_runtime.h"

// #define GOOD_HI 0x0123456789ABCDEFULL
// #define GOOD_LO 0x0FEDCBA987654321ULL
// #define BAD_HI  0xDEADBEEFCAFEBABEULL
// #define BAD_LO  0xBADC0FFEE0DDF00DULL

// // IMPORTANT: volatile so the compiler reloads after the call to foo()
// extern volatile uint64_t pac_ra_shadow_foo  __asm__("__pac_ra_shadow$foo");
// extern volatile uint64_t pac_ret_shadow_foo __asm__("__pac_ret_shadow$foo");

// int foo(int a) {
//     printf(" I am foo %p\n", (void *)&foo);

//     // Manual negative-test (pre-epilogue)
//     uintptr_t ra  = (uintptr_t)__builtin_return_address(0);
//     uint64_t  sig = prince_sign((uint64_t)ra, 0);

//     prince_set_key(BAD_HI, BAD_LO);
//     asm volatile("fence iorw, iorw" ::: "memory");

//     uint64_t dec = prince_auth(sig, 0);
//     printf("debug (manual, pre-epilogue): ra=0x%016" PRIxPTR " auth=0x%016" PRIx64 "\n", ra, dec);

//     return a * 2;
// }

// int bar(int b) {
//     printf(" I am bar %p\n", (void *)&bar);
//     return b + 1;
// }

// int main(void) {
//     prince_set_key(GOOD_HI, GOOD_LO);
//     asm volatile("fence iorw, iorw" ::: "memory");

//     // One-time sanity: addresses should match your objdump comments
//     printf("addrs: &ra_shadow=%p &ret_shadow=%p\n",
//            (void*)&pac_ra_shadow_foo, (void*)&pac_ret_shadow_foo);

//     printf("main: calling foo...\n");
//     int y = foo(5);

//     // Prevent reordering/hoisting across the call
//     asm volatile("" ::: "memory");

//     // NOW the epilogue has run and the pass stored the shadows
//     printf("main: foo shadows AFTER return: ra=0x%016" PRIx64
//            " ret=0x%016" PRIx64 "\n", pac_ra_shadow_foo, pac_ret_shadow_foo);

//     // Restore GOOD so libc keeps working if you continue
//     prince_set_key(GOOD_HI, GOOD_LO);
//     asm volatile("fence iorw, iorw" ::: "memory");

//     printf("main: returned from foo, y=%d\n", y);
//     int z = bar(y);
//     printf("this is z %d\n", z);
//     return 0;
// }






















// #include <stdio.h>
// #include <stdint.h>
// #include "pac_runtime.h"
// #include <inttypes.h>
// #include <unistd.h>

// #define GOOD_HI 0x0123456789ABCDEFULL
// #define GOOD_LO 0x0FEDCBA987654321ULL
// #define BAD_HI  0xDEADBEEFCAFEBABEULL
// #define BAD_LO  0xBADC0FFEE0DDF00DULL

// __attribute__((constructor(50), used, noinline))
// static void pac_set_key_early(void) {
//     prince_set_key(GOOD_HI, GOOD_LO);
//     asm volatile("fence iorw, iorw" ::: "memory");
//     printf("inside pac set key!\n");
// }

// // Make sure these functions are emitted and not inlined away:
// __attribute__((noinline)) int foo(int a) {
//     printf(" I am foo %p\n", (void *)&foo);

//     // // Manual negative-test (pre-epilogue)
//     // uintptr_t ra  = (uintptr_t)__builtin_return_address(0);
//     // uint64_t  sig = prince_sign((uint64_t)ra, 0);

//     prince_set_key(BAD_HI, BAD_LO);
//     asm volatile("fence iorw, iorw" ::: "memory");

//     // uint64_t dec = prince_auth(sig, 0);
//     // printf("debug (manual, pre-epilogue): ra=0x%016" PRIxPTR " auth=0x%016" PRIx64 "\n", ra, dec);

//     return a * 2;
// }

// __attribute__((noinline)) int bar(int b) {
//     printf(" I am bar %p\n", (void *)&bar);
//     return b + 1;
// }

// int main(void) {
//     // Set GOOD key up front so entry signing uses GOOD
//     prince_set_key(GOOD_HI, GOOD_LO);
//     asm volatile("fence iorw, iorw" ::: "memory");

//     printf("main: calling foo...\n");
//     int y = foo(5);                    // <-- real call
//     printf("main: returned from foo, y=%d\n", y);

//     int z = bar(y);                    // <-- real call
//     printf("this is z %d\n", z);

//     _exit(0);
//     //return 0;
// }

























// // test.c
// // RISC-V microbenchmark: return-address protection overhead (via your LLVM MachineFunction pass)
// //
// // Build two versions using your Makefile by swapping LLVM/opt/pass settings:
// //   - unprotected: pass disabled
// //   - protected:   pass enabled
// //
// // Run on your platform:
// //   test.llvm.riscv [iters] [trials]
// //
// // Output reports cycles/(call+ret) using rdcycle.

// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>

// // Serialize a bit around timing.
// // fence rw,rw orders memory ops; fence.i helps keep I-side effects stable for self-modifying code
// // (not needed here, but harmless). You can drop fence.i if you prefer.
// static inline void fence_all(void) {
//     __asm__ __volatile__("fence rw, rw; fence.i" ::: "memory");
// }

// static inline uint64_t rdcycle64(void) {
// #if defined(__riscv) && (__riscv_xlen == 64)
//     uint64_t x;
//     __asm__ __volatile__("rdcycle %0" : "=r"(x));
//     return x;
// #else
//     // RV32 safe pattern (unlikely for your setup, but included for completeness).
//     uint32_t hi0, lo, hi1;
//     __asm__ __volatile__("rdcycleh %0" : "=r"(hi0));
//     __asm__ __volatile__("rdcycle  %0" : "=r"(lo));
//     __asm__ __volatile__("rdcycleh %0" : "=r"(hi1));
//     if (hi0 != hi1) {
//         __asm__ __volatile__("rdcycle %0" : "=r"(lo));
//     }
//     return ((uint64_t)hi1 << 32) | lo;
// #endif
// }

// static int cmp_u64(const void *a, const void *b) {
//     uint64_t ua = *(const uint64_t*)a, ub = *(const uint64_t*)b;
//     return (ua > ub) - (ua < ub);
// }

// static double median_cyc_per_iter(uint64_t *samples_cyc, int n, uint64_t iters) {
//     qsort(samples_cyc, (size_t)n, sizeof(uint64_t), cmp_u64);
//     return (double)samples_cyc[n/2] / (double)iters;
// }

// // Keep a volatile dependency so compiler can’t collapse the loop.
// static volatile uint64_t sink = 1;

// // This is the hot call target. Your MachineFunction pass should instrument the return address here.
// // noinline ensures an actual call/return.
// // The local volatile array forces a real stack frame so RA save/restore behavior is exercised.
// // __attribute__((noinline)) static uint64_t leaf(uint64_t x) {
// //     volatile uint64_t tmp[2];
// //     tmp[0] = x ^ 0x9e3779b97f4a7c15ULL;
// //     tmp[1] = tmp[0] + (uint64_t)(uintptr_t)&tmp;

// //     sink ^= tmp[1];
// //     return sink;
// // }
// __attribute__((noinline)) static uint64_t leaf(uint64_t x) {
//     volatile uint64_t tmp[2];
//     tmp[0] = x ^ (x-5);
//     return tmp[0];
// }

// int main(int argc, char **argv) {
//     uint64_t iters  = (argc > 1) ? strtoull(argv[1], 0, 0) : 200000000ULL;
//     int trials      = (argc > 2) ? atoi(argv[2]) : 7;
//     if (trials < 1) trials = 1;

//     uint64_t *samples = (uint64_t*)calloc((size_t)trials, sizeof(uint64_t));
//     if (!samples) {
//         printf("alloc failed\n");
//         return 1;
//     }

//     // Warm-up (bring code/data into caches, stabilize predictors)
//     for (int i = 0; i < 10000; i++) sink = leaf((uint64_t)i);

//     for (int t = 0; t < trials; t++) {
//         fence_all();
//         uint64_t x  = (uint64_t)t + 1;
//         uint64_t c0 = rdcycle64();

//         for (uint64_t i = 0; i < iters; i++) {
//             // One call + return per iteration.
//             x = leaf(x);

//             // Post-call work to defeat tail-call and keep dependency chain.
//             x ^= (uint64_t)i;
//         }

//         uint64_t c1 = rdcycle64();
//         samples[t] = c1 - c0;

//         // Keep live
//         sink ^= x;
//     }

//     double cyc_call = median_cyc_per_iter(samples, trials, iters);

//     printf("Return-address microbenchmark (calls/returns)\n");
//     printf("iters=%llu trials=%d\n", (unsigned long long)iters, trials);
//     printf("median: %.3f cycles / (call+ret)\n", cyc_call);

//     free(samples);
//     return (int)(sink & 1);
// }
































// test_recursive.c
// RISC-V microbenchmark: recursive return-address protection overhead
//
// Usage:
//   test.llvm.riscv [iters] [trials] [depth]
//
// Example:
//   ./test.llvm.riscv 200000000 7 8
//   ./test.llvm.riscv 200000000 7 16
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
static uint64_t leaf_recursive(uint64_t x, int depth) {
    volatile uint64_t tmp[2];

    tmp[0] = x ^ 0x9e3779b97f4a7c15ULL;
    tmp[1] = tmp[0] + (uint64_t)(uintptr_t)&tmp;

    sink ^= tmp[1];

    if (depth > 0) {
        // Prevent tail-call elimination by using result afterward
        uint64_t r = leaf_recursive(sink, depth - 1);
        sink ^= r;
        return sink;
    }

    return sink;
}
// __attribute__((noinline))
// static uint64_t leaf_recursive(uint64_t x, int depth) {
//     volatile uint64_t tmp[2];
//     sink = x ^ (x-5);

//     if (depth > 0) {
//         uint64_t r = leaf_recursive(sink, depth - 1);
//         sink ^= r;
//         return sink;
//     }

//     return sink;
// }

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






































// // test_returns_only.c
// // RISC-V microbenchmark: return-only overhead (recursive calls with timing around returns)
// //
// // Build two versions:
// //   - unprotected: PAC pass disabled
// //   - protected:   PAC pass enabled
// //
// // Run on your RISC-V platform:
// //   test_returns_only.riscv [depth] [trials]
// //
// // Output reports cycles/return using rdcycle.

// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>

// /* ------------------------------------------------------------ */
// /* Low-level helpers                                            */
// /* ------------------------------------------------------------ */

// static inline void fence_all(void) {
//     __asm__ __volatile__("fence rw, rw; fence.i" ::: "memory");
// }

// static inline uint64_t rdcycle64(void) {
// #if defined(__riscv) && (__riscv_xlen == 64)
//     uint64_t x;
//     __asm__ __volatile__("rdcycle %0" : "=r"(x));
//     return x;
// #else
//     // RV32 safe pattern
//     uint32_t hi0, lo, hi1;
//     __asm__ __volatile__("rdcycleh %0" : "=r"(hi0));
//     __asm__ __volatile__("rdcycle  %0" : "=r"(lo));
//     __asm__ __volatile__("rdcycleh %0" : "=r"(hi1));
//     if (hi0 != hi1) {
//         __asm__ __volatile__("rdcycle %0" : "=r"(lo));
//     }
//     return ((uint64_t)hi1 << 32) | lo;
// #endif
// }

// static volatile uint64_t sink = 1;

// /* ------------------------------------------------------------ */
// /* Recursive function (returns-only timing)                     */
// /* ------------------------------------------------------------ */
// /*
//  * Timing model:
//  *  - At deepest frame (depth == max_depth):
//  *      fence; t_start = rdcycle64(); fence
//  *      return
//  *  - All returns propagate upward
//  *  - In main, immediately after recurse returns:
//  *      fence; t_stop = rdcycle64(); fence
//  *
//  * Timed region = ONLY returns + epilogues
//  */
// __attribute__((noinline))
// static uint64_t recurse(int depth,
//                         int max_depth,
//                         uint64_t *t_start)
// {
//     /* Force real stack frame and epilogue */
//     volatile uint64_t local[2];
//     local[0] = (uint64_t)depth * 0x9e3779b97f4a7c15ULL;
//     local[1] = local[0] ^ (uint64_t)(uintptr_t)&local;

//     if (depth == max_depth) {
//         fence_all();
//         *t_start = rdcycle64();
//         fence_all();
//         return local[1];
//     }

//     uint64_t x = recurse(depth + 1, max_depth, t_start);

//     // Prevent tail-call elimination if needed
//     // x ^= local[1];
//     // sink ^= x;

//     return x;
// }

// /* ------------------------------------------------------------ */

// static int cmp_u64(const void *a, const void *b) {
//     uint64_t ua = *(const uint64_t*)a;
//     uint64_t ub = *(const uint64_t*)b;
//     return (ua > ub) - (ua < ub);
// }

// int main(int argc, char **argv) {
//     int depth   = (argc > 1) ? atoi(argv[1]) : 40960;
//     int trials  = (argc > 2) ? atoi(argv[2]) : 11;

//     if (depth < 1) depth = 1;
//     if (trials < 1) trials = 1;

//     uint64_t *samples = calloc((size_t)trials, sizeof(uint64_t));
//     if (!samples) {
//         fprintf(stderr, "alloc failed\n");
//         return 1;
//     }

//     /* Warm-up */
//     for (int i = 0; i < 1000; i++) {
//         uint64_t ts = 0;
//         sink ^= recurse(0, depth, &ts);
//     }

//     for (int t = 0; t < trials; t++) {
//         uint64_t t_start = 0;

//         recurse(0, depth, &t_start);

//         fence_all();
//         uint64_t t_stop = rdcycle64();
//         fence_all();

//         samples[t] = t_stop - t_start;
//     }

//     qsort(samples, (size_t)trials, sizeof(uint64_t), cmp_u64);

//     uint64_t median_cyc = samples[trials / 2];
//     double median_per_ret = (double)median_cyc / (double)depth;

//     printf("PAC-RET returns-only microbenchmark (rdcycle)\n");
//     printf("depth=%d trials=%d\n\n", depth, trials);

//     printf("per-trial results:\n");
//     for (int t = 0; t < trials; t++) {
//         double per_ret = (double)samples[t] / (double)depth;
//         printf("  trial %2d: total=%llu cycles  per-ret=%.3f cycles\n",
//                t,
//                (unsigned long long)samples[t],
//                per_ret);
//     }

//     printf("\nmedian:\n");
//     printf("  total:   %llu cycles\n",
//            (unsigned long long)median_cyc);
//     printf("  per-ret: %.3f cycles\n",
//            median_per_ret);

//     free(samples);
//     return (int)(sink & 1);
// }




























// // test_entries_only.c
// // RISC-V microbenchmark: PAC entry (signing) overhead - times only function prologues
// //
// // Build two versions:
// //   - unprotected: PAC pass disabled
// //   - protected:   PAC pass enabled (signs return address in prologue)
// //
// // Run on your RISC-V platform:
// //   test_entries_only.riscv [depth] [trials]
// //
// // Output reports cycles/entry using rdcycle.

// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>

// /* ------------------------------------------------------------ */
// /* Low-level helpers                                            */
// /* ------------------------------------------------------------ */

// static inline void fence_all(void) {
//     __asm__ __volatile__("fence rw, rw; fence.i" ::: "memory");
// }

// static inline uint64_t rdcycle64(void) {
// #if defined(__riscv) && (__riscv_xlen == 64)
//     uint64_t x;
//     __asm__ __volatile__("rdcycle %0" : "=r"(x));
//     return x;
// #else
//     // RV32 safe pattern
//     uint32_t hi0, lo, hi1;
//     __asm__ __volatile__("rdcycleh %0" : "=r"(hi0));
//     __asm__ __volatile__("rdcycle  %0" : "=r"(lo));
//     __asm__ __volatile__("rdcycleh %0" : "=r"(hi1));
//     if (hi0 != hi1) {
//         __asm__ __volatile__("rdcycle %0" : "=r"(lo));
//     }
//     return ((uint64_t)hi1 << 32) | lo;
// #endif
// }

// static volatile uint64_t sink = 1;

// /* ------------------------------------------------------------ */
// /* Recursive function (entry-only timing)                       */
// /* ------------------------------------------------------------ */
// /*
//  * Timing model:
//  *  - In main:
//  *      fence; t_start = rdcycle64(); fence
//  *  - Recurse down to max_depth:
//  *      each entry executes PAC signing (if enabled)
//  *  - At deepest frame:
//  *      fence; t_stop = rdcycle64(); fence
//  *      return
//  *
//  * Timed region = ONLY function entries (PAC signing in prologue)
//  */
// __attribute__((noinline))
// static uint64_t recurse_entry(int depth,
//                               int max_depth,
//                               uint64_t *t_stop)
// {
//     if (depth == max_depth) {
//         fence_all();
//         *t_stop = rdcycle64();
//         fence_all();

//         volatile uint64_t local[2];
//         local[0] = (uint64_t)depth;
//         local[1] = local[0] ^ (uint64_t)(uintptr_t)&local;
//         return local[1];
//     } else {
//         uint64_t x = recurse_entry(depth + 1, max_depth, t_stop);

//         /* Force a real stack frame so PAC signing is emitted */
//         volatile uint64_t local[2];
//         local[0] = (uint64_t)depth;
//         local[1] = local[0] ^ (uint64_t)(uintptr_t)&local;

//         (void)x; /* silence unused warning if optimized */

//         return local[1];
//     }
// }

// /* ------------------------------------------------------------ */

// static int cmp_u64(const void *a, const void *b) {
//     uint64_t ua = *(const uint64_t*)a;
//     uint64_t ub = *(const uint64_t*)b;
//     return (ua > ub) - (ua < ub);
// }

// int main(int argc, char **argv) {
//     int depth   = (argc > 1) ? atoi(argv[1]) : 4096;
//     int trials  = (argc > 2) ? atoi(argv[2]) : 11;

//     if (depth < 1) depth = 1;
//     if (trials < 1) trials = 1;

//     uint64_t *samples = calloc((size_t)trials, sizeof(uint64_t));
//     if (!samples) {
//         fprintf(stderr, "alloc failed\n");
//         return 1;
//     }

//     /* Warm-up */
//     for (int i = 0; i < 1000; i++) {
//         uint64_t te = 0;
//         sink ^= recurse_entry(0, depth, &te);
//     }

//     for (int t = 0; t < trials; t++) {
//         uint64_t t_start, t_stop = 0;

//         fence_all();
//         t_start = rdcycle64();
//         fence_all();

//         recurse_entry(0, depth, &t_stop);

//         samples[t] = t_stop - t_start;
//     }

//     qsort(samples, (size_t)trials, sizeof(uint64_t), cmp_u64);

//     uint64_t median_cyc = samples[trials / 2];
//     double cyc_per_entry = (double)median_cyc / (double)depth;

//     printf("PAC entry-only microbenchmark (rdcycle)\n");
//     printf("depth=%d trials=%d\n\n", depth, trials);

//     printf("per-trial results:\n");
//     for (int t = 0; t < trials; t++) {
//         double per_entry = (double)samples[t] / (double)depth;
//         printf("  trial %2d: total=%llu cycles  per-entry=%.3f cycles\n",
//                t,
//                (unsigned long long)samples[t],
//                per_entry);
//     }

//     printf("\nmedian:\n");
//     printf("  total:     %llu cycles\n",
//            (unsigned long long)median_cyc);
//     printf("  per-entry: %.3f cycles\n",
//            cyc_per_entry);

//     free(samples);
//     return (int)(sink & 1);
// }


