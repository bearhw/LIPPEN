// Apple Silicon (macOS): CNTVCT deltas for
// - nopac: LDR
// - pac-fused: LDRAA
// - pac-split-da: LDR + AUTDA (addr modifier)
// - pac-split-ia-c: LDR + AUTIA (constant modifier)
// - pac-split-ia-reload: LDR + AUTIA (32 rotating modifiers, stack-spilled)
//
#if !defined(__APPLE__) || !defined(__aarch64__)
#error "This benchmark targets Apple Silicon (arm64) on macOS."
#endif
#define _DARWIN_C_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/* ------------------------------------------------------------ */
/* 32 distinct modifiers (mirrors RISC-V bench)                 */
/* ------------------------------------------------------------ */
#define NUM_MODS 32
static const uintptr_t modifiers[NUM_MODS] = {
    0x0000000000000001ULL, 0x0000000000000002ULL,
    0x0000000000000003ULL, 0x0000000000000004ULL,
    0x0000000000000005ULL, 0x0000000000000006ULL,
    0x0000000000000007ULL, 0x0000000000000008ULL,
    0x0000000000000009ULL, 0x000000000000000AULL,
    0x000000000000000BULL, 0x000000000000000CULL,
    0x000000000000000DULL, 0x000000000000000EULL,
    0x000000000000000FULL, 0x0000000000000010ULL,
    0x0000000000000011ULL, 0x0000000000000012ULL,
    0x0000000000000013ULL, 0x0000000000000014ULL,
    0x0000000000000015ULL, 0x0000000000000016ULL,
    0x0000000000000017ULL, 0x0000000000000018ULL,
    0x0000000000000019ULL, 0x000000000000001AULL,
    0x000000000000001BULL, 0x000000000000001CULL,
    0x000000000000001DULL, 0x000000000000001EULL,
    0x000000000000001FULL, 0x0000000000000020ULL,
};
/* ------------------------------------------------------------ */
/* Low-level helpers                                            */
/* ------------------------------------------------------------ */
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
    for (uint64_t i = 0; i < iters; i += 32) {
        __asm__ __volatile__(
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
            "ldr %x0, [%x0]\n\t"
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
static inline uint64_t run_ldraa(uintptr_t start, uint64_t iters) {
    uintptr_t addr = *(uintptr_t*)start;
    fence();
    uint64_t c0 = cntvct_now();
    for (uint64_t i = 0; i < iters; i += 32) {
        __asm__ __volatile__(
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
            "ldraa %x0, [%x0]\n\t"
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
/* pac-split-da: LDR + AUTDZA (zero addr modifier)             */
/* ------------------------------------------------------------ */
static inline uint64_t run_ldr_autda(uintptr_t start, uint64_t iters) {
    uintptr_t addr = start;
    fence();
    uint64_t c0 = cntvct_now();
    for (uint64_t i = 0; i < iters; i += 32) {
        __asm__ __volatile__(
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
            "ldr %x0, [%x0]\n\t" "autdza %x0\n\t"
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
/* pac-split-ia-const: LDR + AUTIA (constant modifier)         */
/* ------------------------------------------------------------ */
static inline uint64_t run_ldr_autia_const(uintptr_t start,
                                            uint64_t  iters,
                                            uintptr_t mod) {
    uintptr_t addr = start;
    fence();
    uint64_t c0 = cntvct_now();
    for (uint64_t i = 0; i < iters; i += 32) {
        __asm__ __volatile__(
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            "ldr %x[a], [%x[a]]\n\t" "autia %x[a], %x[m]\n\t"
            : [a] "+r"(addr)
            : [m]  "r"(mod)
            : "memory"
        );
    }
    uint64_t c1 = cntvct_now();
    sink ^= addr;
    return c1 - c0;
}
/* ------------------------------------------------------------ */
/* pac-split-ia-reload: LDR + AUTIA (32 rotating modifiers)    */
/* ------------------------------------------------------------ */
__attribute__((noinline))
uint64_t run_ldr_autia_reload(uintptr_t start,
                               uint64_t  iters,
                               const uintptr_t mods[NUM_MODS]) {
    uint64_t ret;
    __asm__ __volatile__(
        /* ---- prologue: carve out stack frame ---- */
        "sub  sp, sp, #288\n\t"
        "dsb  sy\n\t"
        "isb\n\t"
        /* store addr, counter=0, iters */
        "str  %x[start],  [sp, #0]\n\t"
        "str  xzr,        [sp, #8]\n\t"
        "str  %x[iters],  [sp, #16]\n\t"
        /* copy 32 modifiers onto stack at [sp+24]..[sp+279] */
        "ldr  x9,  [%x[mods], #0]\n\t"   "str  x9,  [sp, #24]\n\t"
        "ldr  x9,  [%x[mods], #8]\n\t"   "str  x9,  [sp, #32]\n\t"
        "ldr  x9,  [%x[mods], #16]\n\t"  "str  x9,  [sp, #40]\n\t"
        "ldr  x9,  [%x[mods], #24]\n\t"  "str  x9,  [sp, #48]\n\t"
        "ldr  x9,  [%x[mods], #32]\n\t"  "str  x9,  [sp, #56]\n\t"
        "ldr  x9,  [%x[mods], #40]\n\t"  "str  x9,  [sp, #64]\n\t"
        "ldr  x9,  [%x[mods], #48]\n\t"  "str  x9,  [sp, #72]\n\t"
        "ldr  x9,  [%x[mods], #56]\n\t"  "str  x9,  [sp, #80]\n\t"
        "ldr  x9,  [%x[mods], #64]\n\t"  "str  x9,  [sp, #88]\n\t"
        "ldr  x9,  [%x[mods], #72]\n\t"  "str  x9,  [sp, #96]\n\t"
        "ldr  x9,  [%x[mods], #80]\n\t"  "str  x9,  [sp, #104]\n\t"
        "ldr  x9,  [%x[mods], #88]\n\t"  "str  x9,  [sp, #112]\n\t"
        "ldr  x9,  [%x[mods], #96]\n\t"  "str  x9,  [sp, #120]\n\t"
        "ldr  x9,  [%x[mods], #104]\n\t" "str  x9,  [sp, #128]\n\t"
        "ldr  x9,  [%x[mods], #112]\n\t" "str  x9,  [sp, #136]\n\t"
        "ldr  x9,  [%x[mods], #120]\n\t" "str  x9,  [sp, #144]\n\t"
        "ldr  x9,  [%x[mods], #128]\n\t" "str  x9,  [sp, #152]\n\t"
        "ldr  x9,  [%x[mods], #136]\n\t" "str  x9,  [sp, #160]\n\t"
        "ldr  x9,  [%x[mods], #144]\n\t" "str  x9,  [sp, #168]\n\t"
        "ldr  x9,  [%x[mods], #152]\n\t" "str  x9,  [sp, #176]\n\t"
        "ldr  x9,  [%x[mods], #160]\n\t" "str  x9,  [sp, #184]\n\t"
        "ldr  x9,  [%x[mods], #168]\n\t" "str  x9,  [sp, #192]\n\t"
        "ldr  x9,  [%x[mods], #176]\n\t" "str  x9,  [sp, #200]\n\t"
        "ldr  x9,  [%x[mods], #184]\n\t" "str  x9,  [sp, #208]\n\t"
        "ldr  x9,  [%x[mods], #192]\n\t" "str  x9,  [sp, #216]\n\t"
        "ldr  x9,  [%x[mods], #200]\n\t" "str  x9,  [sp, #224]\n\t"
        "ldr  x9,  [%x[mods], #208]\n\t" "str  x9,  [sp, #232]\n\t"
        "ldr  x9,  [%x[mods], #216]\n\t" "str  x9,  [sp, #240]\n\t"
        "ldr  x9,  [%x[mods], #224]\n\t" "str  x9,  [sp, #248]\n\t"
        "ldr  x9,  [%x[mods], #232]\n\t" "str  x9,  [sp, #256]\n\t"
        "ldr  x9,  [%x[mods], #240]\n\t" "str  x9,  [sp, #264]\n\t"
        "ldr  x9,  [%x[mods], #248]\n\t" "str  x9,  [sp, #272]\n\t"
        "mrs  x8, cntvct_el0\n\t"         /* t2 = start cycle */
        "b    1f\n\t"
        /* ---- loop header ---- */
        "1:\n\t"
        "ldr  x9,  [sp, #8]\n\t"          /* i */
        "ldr  x11, [sp, #16]\n\t"         /* iters */
        "cmp  x9,  x11\n\t"
        "b.ge 3f\n\t"
        /* ---- body: x10 = current addr, x9 = modifier scratch ---- */
        "ldr  x10, [sp, #0]\n\t"
        /* slot  1: mod[0]  at [sp+24]  */
        "ldr  x9,  [sp, #24]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  2: mod[1]  at [sp+32]  */
        "ldr  x9,  [sp, #32]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  3: mod[2]  at [sp+40]  */
        "ldr  x9,  [sp, #40]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  4: mod[3]  at [sp+48]  */
        "ldr  x9,  [sp, #48]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  5: mod[4]  at [sp+56]  */
        "ldr  x9,  [sp, #56]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  6: mod[5]  at [sp+64]  */
        "ldr  x9,  [sp, #64]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  7: mod[6]  at [sp+72]  */
        "ldr  x9,  [sp, #72]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  8: mod[7]  at [sp+80]  */
        "ldr  x9,  [sp, #80]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot  9: mod[8]  at [sp+88]  */
        "ldr  x9,  [sp, #88]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 10: mod[9]  at [sp+96]  */
        "ldr  x9,  [sp, #96]\n\t"  "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 11: mod[10] at [sp+104] */
        "ldr  x9,  [sp, #104]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 12: mod[11] at [sp+112] */
        "ldr  x9,  [sp, #112]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 13: mod[12] at [sp+120] */
        "ldr  x9,  [sp, #120]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 14: mod[13] at [sp+128] */
        "ldr  x9,  [sp, #128]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 15: mod[14] at [sp+136] */
        "ldr  x9,  [sp, #136]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 16: mod[15] at [sp+144] */
        "ldr  x9,  [sp, #144]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 17: mod[16] at [sp+152] */
        "ldr  x9,  [sp, #152]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 18: mod[17] at [sp+160] */
        "ldr  x9,  [sp, #160]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 19: mod[18] at [sp+168] */
        "ldr  x9,  [sp, #168]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 20: mod[19] at [sp+176] */
        "ldr  x9,  [sp, #176]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 21: mod[20] at [sp+184] */
        "ldr  x9,  [sp, #184]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 22: mod[21] at [sp+192] */
        "ldr  x9,  [sp, #192]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 23: mod[22] at [sp+200] */
        "ldr  x9,  [sp, #200]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 24: mod[23] at [sp+208] */
        "ldr  x9,  [sp, #208]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 25: mod[24] at [sp+216] */
        "ldr  x9,  [sp, #216]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 26: mod[25] at [sp+224] */
        "ldr  x9,  [sp, #224]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 27: mod[26] at [sp+232] */
        "ldr  x9,  [sp, #232]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 28: mod[27] at [sp+240] */
        "ldr  x9,  [sp, #240]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 29: mod[28] at [sp+248] */
        "ldr  x9,  [sp, #248]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 30: mod[29] at [sp+256] */
        "ldr  x9,  [sp, #256]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 31: mod[30] at [sp+264] */
        "ldr  x9,  [sp, #264]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        /* slot 32: mod[31] at [sp+272] */
        "ldr  x9,  [sp, #272]\n\t" "ldr  x10, [x10]\n\t" "autia x10, x9\n\t"
        "str  x10, [sp, #0]\n\t"          /* store updated addr */
        /* i += 32 */
        "ldr  x9,  [sp, #8]\n\t"
        "add  x9,  x9,  #32\n\t"
        "str  x9,  [sp, #8]\n\t"
        "b    1b\n\t"
        /* ---- exit ---- */
        "3:\n\t"
        "mrs  x9, cntvct_el0\n\t"
        "sub  %x[ret], x9, x8\n\t"
        "add  sp, sp, #288\n\t"
        : [ret]   "=r"(ret)
        : [start] "r"(start),
          [iters] "r"(iters),
          [mods]  "r"(mods)
        : "x8", "x9", "x10", "x11", "memory"
    );
    return ret;
}
/* ------------------------------------------------------------ */
/* main                                                         */
/* ------------------------------------------------------------ */
int main(int argc, char **argv) {
    uint64_t iters = (argc > 1) ? strtoull(argv[1], 0, 0) : 200000000ULL;
    size_t   N     = (argc > 2) ? (size_t)strtoull(argv[2], 0, 0) : 512;
    int      trials = (argc > 3) ? atoi(argv[3]) : 15;
    /* N must be a multiple of 32 for modifier rotation to be consistent */
    if (N < 32) N = 32;
    N = (N + 31) & ~(size_t)31;
    if (trials < 1) trials = 1;
    const uintptr_t IA_MOD = 0x12345678ULL;
    uintptr_t *nodes_raw         = alloc_aligned(64, N * sizeof(uintptr_t));
    uintptr_t *nodes_signed_zero = alloc_aligned(64, N * sizeof(uintptr_t));
    uintptr_t *nodes_signed_da   = alloc_aligned(64, N * sizeof(uintptr_t));
    uintptr_t *nodes_signed_ia   = alloc_aligned(64, N * sizeof(uintptr_t));
    uintptr_t *nodes_ia_reload   = alloc_aligned(64, N * sizeof(uintptr_t));
    if (!nodes_raw || !nodes_signed_zero || !nodes_signed_da ||
        !nodes_signed_ia || !nodes_ia_reload) {
        fprintf(stderr, "alloc failed\n");
        return 1;
    }
    /* build raw ring */
    for (size_t i = 0; i < N; i++)
        nodes_raw[i] = (uintptr_t)&nodes_raw[(i + 1) % N];
    /* build zero-modifier DA ring (for LDRAA) */
    for (size_t i = 0; i < N; i++) {
        uintptr_t p    = (uintptr_t)&nodes_signed_zero[(i + 1) % N];
        uintptr_t zero = 0;
        __asm__ __volatile__("pacda %x0, %x1" : "+r"(p) : "r"(zero) : "memory");
        nodes_signed_zero[i] = p;
    }
    /* build DA ring (pacdza / zero modifier) */
    for (size_t i = 0; i < N; i++) {
        uintptr_t p = (uintptr_t)&nodes_signed_da[(i + 1) % N];
        __asm__ __volatile__("pacdza %x0" : "+r"(p) : : "memory");
        nodes_signed_da[i] = p;
    }
    /* build IA ring (constant modifier) */
    for (size_t i = 0; i < N; i++) {
        uintptr_t p = (uintptr_t)&nodes_signed_ia[(i + 1) % N];
        __asm__ __volatile__("pacia %x0, %x1" : "+r"(p) : "r"(IA_MOD) : "memory");
        nodes_signed_ia[i] = p;
    }
    /* build IA reload ring: node i signed with modifiers[i % 32] */
    for (size_t i = 0; i < N; i++) {
        uintptr_t p   = (uintptr_t)&nodes_ia_reload[(i + 1) % N];
        uintptr_t mod = modifiers[i % NUM_MODS];
        __asm__ __volatile__("pacia %x0, %x1" : "+r"(p) : "r"(mod) : "memory");
        nodes_ia_reload[i] = p;
    }
    /* warm-up */
    run_ldr((uintptr_t)&nodes_raw[0],           N * 16);
    run_ldraa((uintptr_t)&nodes_signed_zero[0], N * 16);
    run_ldr_autda((uintptr_t)&nodes_signed_da[0], N * 16);
    run_ldr_autia_const((uintptr_t)&nodes_signed_ia[0], N * 16, IA_MOD);
    run_ldr_autia_reload((uintptr_t)&nodes_ia_reload[0], N * 16, modifiers);
    uint64_t *t_ldr    = calloc(trials, sizeof(uint64_t));
    uint64_t *t_ldraa  = calloc(trials, sizeof(uint64_t));
    uint64_t *t_autda  = calloc(trials, sizeof(uint64_t));
    uint64_t *t_autia  = calloc(trials, sizeof(uint64_t));
    uint64_t *t_reload = calloc(trials, sizeof(uint64_t));
    printf("iters=%llu N=%zu trials=%d (Apple Silicon arm64)\n\n",
           (unsigned long long)iters, N, trials);
    for (int t = 0; t < trials; t++) {
        t_ldr[t]    = run_ldr((uintptr_t)&nodes_raw[0], iters);
        t_ldraa[t]  = run_ldraa((uintptr_t)&nodes_signed_zero[0], iters);
        t_autda[t]  = run_ldr_autda((uintptr_t)&nodes_signed_da[0], iters);
        t_autia[t]  = run_ldr_autia_const((uintptr_t)&nodes_signed_ia[0],
                                           iters, IA_MOD);
        t_reload[t] = run_ldr_autia_reload((uintptr_t)&nodes_ia_reload[0],
                                            iters, modifiers);
        printf("trial %2d: LDR=%llu LDRAA=%llu LDR+AUTDA=%llu "
               "LDR+AUTIA(c)=%llu LDR+AUTIA(reload)=%llu\n",
               t,
               (unsigned long long)t_ldr[t],
               (unsigned long long)t_ldraa[t],
               (unsigned long long)t_autda[t],
               (unsigned long long)t_autia[t],
               (unsigned long long)t_reload[t]);
    }
    printf("\nmedian ticks:\n");
    printf("  nopac        (LDR):             %llu\n",
           (unsigned long long)median_u64(t_ldr, trials));
    printf("  pac fused    (LDRAA):           %llu  delta=%+lld\n",
           (unsigned long long)median_u64(t_ldraa, trials),
           (long long)(median_u64(t_ldraa,  trials) - median_u64(t_ldr, trials)));
    printf("  pac split    (AUTDA):           %llu  delta=%+lld\n",
           (unsigned long long)median_u64(t_autda, trials),
           (long long)(median_u64(t_autda,  trials) - median_u64(t_ldr, trials)));
    printf("  pac split    (AUTIA const):     %llu  delta=%+lld\n",
           (unsigned long long)median_u64(t_autia, trials),
           (long long)(median_u64(t_autia,  trials) - median_u64(t_ldr, trials)));
    printf("  pac split    (AUTIA reload):    %llu  delta=%+lld\n",
           (unsigned long long)median_u64(t_reload, trials),
           (long long)(median_u64(t_reload, trials) - median_u64(t_ldr, trials)));
    free(nodes_raw); free(nodes_signed_zero); free(nodes_signed_da);
    free(nodes_signed_ia); free(nodes_ia_reload);
    free(t_ldr); free(t_ldraa); free(t_autda); free(t_autia); free(t_reload);
    return (int)(sink & 1);
}

