// Apple Silicon (macOS): CNTVCT deltas for
//  - nopac:           LDR
//  - pac-fused:       LDRAA
//  - pac-split-da:    LDR + AUTDA   (addr modifier)
//  - pac-split-ia-c:  LDR + AUTIA   (constant modifier)
#if !defined(__APPLE__) || !defined(__aarch64__)
#error "This benchmark targets Apple Silicon (arm64) on macOS."
#endif
#define _DARWIN_C_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/* ------------------------------------------------------------ */
/* Low-level helpers                                            */
/* ------------------------------------------------------------ */
static inline uintptr_t canonical_user_ptr(uintptr_t p) {
   return p & 0x00FFFFFFFFFFFFFFULL;
}
static inline void fence(void) {
   __asm__ __volatile__("dsb sy; isb" ::: "memory");
}
static inline uint64_t cntvct_now(void) {
   uint64_t v;
   __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(v));
   return v;
}
static void *alloc_aligned(size_t align, size_t bytes) {
   void *p = NULL;
   if (posix_memalign(&p, align, bytes) != 0) return NULL;
   return p;
}
static volatile uintptr_t sink = 1;
/* ------------------------------------------------------------ */
/* Statistics helpers                                           */
/* ------------------------------------------------------------ */
static int cmp_u64(const void *a, const void *b) {
   uint64_t ua = *(const uint64_t*)a;
   uint64_t ub = *(const uint64_t*)b;
   return (ua > ub) - (ua < ub);
}
static uint64_t median_u64(uint64_t *xs, int n) {
   qsort(xs, (size_t)n, sizeof(uint64_t), cmp_u64);
   return xs[n / 2];
}
/* ------------------------------------------------------------ */
/* nopac: plain LDR                                             */
/* ------------------------------------------------------------ */
static inline uint64_t run_ldr(uintptr_t start, uint64_t iters) {
   uintptr_t x = start;
   fence();
   uint64_t c0 = cntvct_now();
   for (uint64_t i = 0; i < iters; i++) {
       __asm__ __volatile__(
           "ldr %x0, [%x0]"
           : "+r"(x)
           :
           : "memory"
       );
   }
   uint64_t c1 = cntvct_now();
   sink ^= x;
   return c1 - c0;
}
/* ------------------------------------------------------------ */
/* pac-fused: LDRAA                                             */
/* ------------------------------------------------------------ */
static inline uint64_t run_ldraa(uintptr_t start, uint64_t iters)
{
    uintptr_t addr = *(uintptr_t*)start;
    fence();
    uint64_t c0 = cntvct_now();
    for (uint64_t i = 0; i < iters; i++) {
        __asm__ __volatile__(
            "ldraa %x0, [%x0]\n\t"
            : "+r"(addr)
            :
            : "memory"
        );
    }
    uint64_t c1 = cntvct_now();
    sink ^= addr;
    return c1 - c0;
}
/* ------------------------------------------------------------ */
/* pac-split-da: LDR + AUTDA (addr modifier)                    */
/* ------------------------------------------------------------ */
static inline uint64_t run_ldr_autda(uintptr_t start, uint64_t iters) {
   uintptr_t addr = start;
   uintptr_t x;
   fence();
   uint64_t c0 = cntvct_now();
   for (uint64_t i = 0; i < iters; i++) {
       __asm__ __volatile__(
           "ldr   %x[x], [%x[a]]\n\t"
           "autdza %x[x]\n\t"
           : [x] "=&r"(x)
           : [a] "r"(addr)
           : "memory"
       );
       addr = x;
   }
   uint64_t c1 = cntvct_now();
   sink ^= addr;
   return c1 - c0;
}
/* ------------------------------------------------------------ */
/* pac-split-ia-const: LDR + AUTIA (constant modifier)          */
/* ------------------------------------------------------------ */
static inline uint64_t run_ldr_autia_const(uintptr_t start,
                                         uint64_t iters,
                                         uintptr_t mod) {
   uintptr_t addr = start;
   uintptr_t x;
   fence();
   uint64_t c0 = cntvct_now();
   for (uint64_t i = 0; i < iters; i++) {
       __asm__ __volatile__(
           "ldr   %x[x], [%x[a]]\n\t"
           "autia %x[x], %x[m]\n\t"
           : [x] "=&r"(x)
           : [a] "r"(addr),
             [m] "r"(mod)
           : "memory"
       );
       addr = x;
   }
   uint64_t c1 = cntvct_now();
   sink ^= addr;
   return c1 - c0;
}
/* ------------------------------------------------------------ */
/* main                                                         */
/* ------------------------------------------------------------ */
int main(int argc, char **argv) {
   uint64_t iters  = (argc > 1) ? strtoull(argv[1], 0, 0) : 200000000ULL;
   size_t   N      = (argc > 2) ? (size_t)strtoull(argv[2], 0, 0) : 512;
   int      trials = (argc > 3) ? atoi(argv[3]) : 15;
   printf("Hello I am running ");
   if (N < 2) N = 2;
   if (trials < 1) trials = 1;
   const uintptr_t IA_MOD = 0x12345678ULL;
   const uintptr_t ZERO_MOD = 0x00000000ULL;
   uintptr_t *nodes_raw =
       alloc_aligned(64, N * sizeof(uintptr_t));
   uintptr_t *nodes_signed_da =
       alloc_aligned(64, N * sizeof(uintptr_t));
   uintptr_t *nodes_signed_ia =
       alloc_aligned(64, N * sizeof(uintptr_t));
   uintptr_t *nodes_signed_zero =
       alloc_aligned(64, N * sizeof(uintptr_t));
   if (!nodes_raw || !nodes_signed_da || !nodes_signed_ia) {
       fprintf(stderr, "alloc failed\n");
       return 1;
   }
   /* build rings */
   for (size_t i = 0; i < N; i++) {
       nodes_raw[i]       = (uintptr_t)&nodes_raw[(i + 1) % N];
       nodes_signed_zero[i] = (uintptr_t)&nodes_signed_zero[(i + 1) % N];
       nodes_signed_da[i] = (uintptr_t)&nodes_signed_da[(i + 1) % N];
       nodes_signed_ia[i] = (uintptr_t)&nodes_signed_ia[(i + 1) % N];
   }
   /* sign IA (addr modifier) */
   for (size_t i = 0; i < N; i++) {
      uintptr_t p   = nodes_signed_da[i];
      // uintptr_t mod = (uintptr_t)&nodes_signed_da[i];
      // uintptr_t mod = canonical_user_ptr((uintptr_t)&nodes_signed_da[i]);
  
      __asm__ __volatile__(
          "pacdza %x0"
          : "+r"(p)
          :
          : "memory"
      );
  
      nodes_signed_da[i] = p;
   }
   /* sign IA (constant modifier) */
   for (size_t i = 0; i < N; i++) {
       uintptr_t p = nodes_signed_ia[i];
       __asm__ __volatile__(
           "pacia %x0, %x1"
           : "+r"(p)
           : "r"(IA_MOD)
           : "memory"
       );
       nodes_signed_ia[i] = p;
   }
   /* sign IA (constant modifier) */
   for (size_t i = 0; i < N; i++) {
      uintptr_t next = (uintptr_t)&nodes_signed_zero[(i + 1) % N];
      uintptr_t signed_next = next;
      uintptr_t zero_mod = 0;
      __asm__ __volatile__(
          "pacda %x0, %x1"
          : "+r"(signed_next)
          : "r"(zero_mod)
          : "memory"
      );
  
      nodes_signed_zero[i] = signed_next;
  }
   /* warm-up */
   run_ldr((uintptr_t)&nodes_raw[0], N * 16);
   run_ldraa((uintptr_t)&nodes_signed_zero[0], N * 16);
   run_ldr_autda((uintptr_t)&nodes_signed_da[0], N * 16);
   run_ldr_autia_const((uintptr_t)&nodes_signed_ia[0], N * 16, IA_MOD);
   uint64_t *t_ldr   = calloc(trials, sizeof(uint64_t));
   uint64_t *t_ldraa = calloc(trials, sizeof(uint64_t));
   uint64_t *t_autda = calloc(trials, sizeof(uint64_t));
   uint64_t *t_autia = calloc(trials, sizeof(uint64_t));
   for (int t = 0; t < trials; t++) {
       t_ldr[t]   = run_ldr((uintptr_t)&nodes_raw[0], iters);
       t_ldraa[t] = run_ldraa((uintptr_t)&nodes_signed_zero[0], iters);
       t_autda[t] = run_ldr_autda((uintptr_t)&nodes_signed_da[0], iters);
       t_autia[t] = run_ldr_autia_const(
                        (uintptr_t)&nodes_signed_ia[0], iters, IA_MOD);
       printf("trial %2d: "
              "LDR=%llu  "
              "LDRAA=%llu  "
              "LDR+AUTDA=%llu  "
              "LDR+AUTIA(c)=%llu\n",
              t,
              (unsigned long long)t_ldr[t],
              (unsigned long long)t_ldraa[t],
              (unsigned long long)t_autda[t],
              (unsigned long long)t_autia[t]);
   }
   printf("\nmedian ticks:\n");
   printf("  nopac (LDR):            %llu\n",
          (unsigned long long)median_u64(t_ldr, trials));
   printf("  pac fused (LDRAA):      %llu\n",
          (unsigned long long)median_u64(t_ldraa, trials));
   printf("  pac split (AUTDA):      %llu\n",
          (unsigned long long)median_u64(t_autda, trials));
   printf("  pac split IA (AUTIA c): %llu\n",
          (unsigned long long)median_u64(t_autia, trials));
   return (int)(sink & 1);
}
