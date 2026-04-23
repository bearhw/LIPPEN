
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "rocc.h"

#ifndef PRINCE_OPCODE
#define PRINCE_OPCODE 1
#endif

/* ------------------------------------------------------------ */
/* Helpers                                                      */
/* ------------------------------------------------------------ */

static inline void fence_all(void) {
    asm volatile("fence iorw, iorw" ::: "memory");
}

volatile uintptr_t sink = 1;

/* ------------------------------------------------------------ */
/* 32 distinct modifiers                                        */
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
/* PRINCE SIGN                                                  */
/* ------------------------------------------------------------ */
static inline uintptr_t rocc_sign_mod0_inline(uintptr_t val) {
    register uint64_t rd  asm("x10");
    register uint64_t rs1 asm("x11") = (uint64_t)val;
    register uint64_t rs2 asm("x12") = 0;
    asm volatile(
        ".word " STR(CUSTOMX(PRINCE_OPCODE, 1, 1, 1, 10, 11, 12, 1))
        : "=r"(rd) : "r"(rs1), "r"(rs2) : "memory"
    );
    return (uintptr_t)rd;
}

static inline uintptr_t rocc_sign_modia_inline(uintptr_t val, uintptr_t mod) {
    register uint64_t rd  asm("x10");
    register uint64_t rs1 asm("x11") = (uint64_t)val;
    register uint64_t rs2 asm("x12") = (uint64_t)mod;
    asm volatile(
        ".word " STR(CUSTOMX(PRINCE_OPCODE, 1, 1, 1, 10, 11, 12, 1))
        : "=r"(rd) : "r"(rs1), "r"(rs2) : "memory"
    );
    return (uintptr_t)rd;
}

/* ============================================================ */
/* Apple-shaped baseline (LD only)                              */
/* ============================================================ */
__attribute__((noinline))
uint64_t run_nopac(uintptr_t start, uint64_t iters) {
uint64_t ret;
asm volatile(
"addi sp, sp, -40\n\t"
"fence iorw, iorw\n\t"
"sd   %[start], 24(sp)\n\t"
"sd   %[iters], 32(sp)\n\t"
"rdcycle t2\n\t"
"sd   %[start], 16(sp)\n\t"
"sd   zero,      8(sp)\n\t"
"j    1f\n\t"
"1:\n\t"
"ld   t0, 8(sp)\n\t"
"ld   t1, 32(sp)\n\t"
"bge  t0, t1, 3f\n\t"
"ld   t3, 24(sp)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"  "ld   t3, 0(t3)\n\t"
"sd   t3, 24(sp)\n\t"
"ld   t0, 8(sp)\n\t"
"addi t0, t0, 32\n\t"
"sd   t0, 8(sp)\n\t"
"j    1b\n\t"
"3:\n\t"
"rdcycle t4\n\t"
"sub  %[ret], t4, t2\n\t"
"addi sp, sp, 40\n\t"
        : [ret] "=r"(ret)
        : [start] "r"(start), [iters] "r"(iters)
        : "t0", "t1", "t2", "t3", "t4", "memory"
    );
return ret;
}

/* ============================================================ */
/* Apple-shaped PAC (LD + PRINCE AUTH, modifier = 0)            */
/* ============================================================ */
__attribute__((noinline))
uint64_t run_pac(uintptr_t start, uint64_t iters) {
    uint64_t ret;
    asm volatile(
        "addi sp, sp, -40\n\t"
        "fence iorw, iorw\n\t"
        "sd   zero, 0(sp)\n\t"
        "rdcycle t2\n\t"
        "sd   %[start], 8(sp)\n\t"
        "sd   %[iters], 32(sp)\n\t"
        "j    1f\n\t"
        "1:\n\t"
        "ld   t0, 0(sp)\n\t"
        "ld   t6, 32(sp)\n\t"
        "bge  t0, t6, 3f\n\t"
        "ld   t3, 8(sp)\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"
        "sd   t3, 8(sp)\n\t"
        "ld   t0, 0(sp)\n\t"
        "addi t0, t0, 32\n\t"
        "sd   t0, 0(sp)\n\t"
        "j    1b\n\t"
        "3:\n\t"
        "rdcycle t4\n\t"
        "sub  %[ret], t4, t2\n\t"
        "addi sp, sp, 40\n\t"
        : [ret] "=r"(ret)
        : [start] "r"(start), [iters] "r"(iters)
        : "t0","t1","t2","t3","t4","t6","memory"
    );
    return ret;
}

/* ============================================================ */
/* PAC with single modifier loaded once per iteration           */
/* ============================================================ */
__attribute__((noinline))
uint64_t run_pac_ia(uintptr_t start, uint64_t iters, uintptr_t mod) {
uint64_t ret;
asm volatile(
"addi sp, sp, -48\n\t"
"fence iorw, iorw\n\t"
"sd   %[mod],   0(sp)\n\t"
"sd   %[start], 32(sp)\n\t"
"sd   %[iters], 40(sp)\n\t"
"rdcycle t2\n\t"
"sd   %[start], 16(sp)\n\t"
"sd   zero,      8(sp)\n\t"
"j    1f\n\t"
"1:\n\t"
"ld   t0, 8(sp)\n\t"
"ld   t6, 40(sp)\n\t"
"bge  t0, t6, 3f\n\t"
"ld   t3, 32(sp)\n\t"
"ld   t5,  0(sp)\n\t"           /* load modifier once */
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"sd   t3, 32(sp)\n\t"
"ld   t0, 8(sp)\n\t"
"addi t0, t0, 32\n\t"
"sd   t0, 8(sp)\n\t"
"j    1b\n\t"
"3:\n\t"
"rdcycle t4\n\t"
"sub  %[ret], t4, t2\n\t"
"addi sp, sp, 48\n\t"
        : [ret] "=r"(ret)
        : [start] "r"(start), [iters] "r"(iters), [mod] "r"(mod)
        : "t0","t1","t2","t3","t4","t5","t6","memory"
    );
return ret;
}

/* ============================================================ */
/* PAC with 32 distinct rotating modifiers (interleaved ring)   */
/* Node i is signed with modifiers[i % 32].                     */
/* The 32 unrolled slots each load a different modifier from    */
/* the stack before authenticating.                             */
/*                                                              */
/* Stack layout (288 bytes):                                    */
/*   0(sp)        : current addr (pointer into interleaved ring)*/
/*   8(sp)        : loop counter i                              */
/*   16(sp)       : iters                                       */
/*   24(sp)..279  : mods[0..31]  (32 * 8 = 256 bytes)          */
/* ============================================================ */
__attribute__((noinline))
uint64_t run_pac_ia_reload(uintptr_t start, uint64_t iters,
                           const uintptr_t mods[NUM_MODS]) {
uint64_t ret;
asm volatile(
"addi sp, sp, -288\n\t"
"fence iorw, iorw\n\t"

/* store current addr, counter, iters */
"sd   %[start],  0(sp)\n\t"
"sd   zero,      8(sp)\n\t"
"sd   %[iters], 16(sp)\n\t"

/* copy 32 modifiers onto stack at 24..279 */
"ld   t0,   0(%[mods])\n\t"  "sd t0,  24(sp)\n\t"
"ld   t0,   8(%[mods])\n\t"  "sd t0,  32(sp)\n\t"
"ld   t0,  16(%[mods])\n\t"  "sd t0,  40(sp)\n\t"
"ld   t0,  24(%[mods])\n\t"  "sd t0,  48(sp)\n\t"
"ld   t0,  32(%[mods])\n\t"  "sd t0,  56(sp)\n\t"
"ld   t0,  40(%[mods])\n\t"  "sd t0,  64(sp)\n\t"
"ld   t0,  48(%[mods])\n\t"  "sd t0,  72(sp)\n\t"
"ld   t0,  56(%[mods])\n\t"  "sd t0,  80(sp)\n\t"
"ld   t0,  64(%[mods])\n\t"  "sd t0,  88(sp)\n\t"
"ld   t0,  72(%[mods])\n\t"  "sd t0,  96(sp)\n\t"
"ld   t0,  80(%[mods])\n\t"  "sd t0, 104(sp)\n\t"
"ld   t0,  88(%[mods])\n\t"  "sd t0, 112(sp)\n\t"
"ld   t0,  96(%[mods])\n\t"  "sd t0, 120(sp)\n\t"
"ld   t0, 104(%[mods])\n\t"  "sd t0, 128(sp)\n\t"
"ld   t0, 112(%[mods])\n\t"  "sd t0, 136(sp)\n\t"
"ld   t0, 120(%[mods])\n\t"  "sd t0, 144(sp)\n\t"
"ld   t0, 128(%[mods])\n\t"  "sd t0, 152(sp)\n\t"
"ld   t0, 136(%[mods])\n\t"  "sd t0, 160(sp)\n\t"
"ld   t0, 144(%[mods])\n\t"  "sd t0, 168(sp)\n\t"
"ld   t0, 152(%[mods])\n\t"  "sd t0, 176(sp)\n\t"
"ld   t0, 160(%[mods])\n\t"  "sd t0, 184(sp)\n\t"
"ld   t0, 168(%[mods])\n\t"  "sd t0, 192(sp)\n\t"
"ld   t0, 176(%[mods])\n\t"  "sd t0, 200(sp)\n\t"
"ld   t0, 184(%[mods])\n\t"  "sd t0, 208(sp)\n\t"
"ld   t0, 192(%[mods])\n\t"  "sd t0, 216(sp)\n\t"
"ld   t0, 200(%[mods])\n\t"  "sd t0, 224(sp)\n\t"
"ld   t0, 208(%[mods])\n\t"  "sd t0, 232(sp)\n\t"
"ld   t0, 216(%[mods])\n\t"  "sd t0, 240(sp)\n\t"
"ld   t0, 224(%[mods])\n\t"  "sd t0, 248(sp)\n\t"
"ld   t0, 232(%[mods])\n\t"  "sd t0, 256(sp)\n\t"
"ld   t0, 240(%[mods])\n\t"  "sd t0, 264(sp)\n\t"
"ld   t0, 248(%[mods])\n\t"  "sd t0, 272(sp)\n\t"

"rdcycle t2\n\t"
"j    1f\n\t"

/* ---- LOOP HEADER ---- */
"1:\n\t"
"ld   t0,  8(sp)\n\t"
"ld   t6, 16(sp)\n\t"
"bge  t0, t6, 3f\n\t"

/* ---- BODY: single pointer chain, modifier rotates per slot -- */
/* Each slot: load mod[k] into t5, load next ptr, authenticate  */
"ld   t3,  0(sp)\n\t"           /* load current addr */

/* Slot  1: mod[0]  at 24(sp)  */
"ld   t5,  24(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  2: mod[1]  at 32(sp)  */
"ld   t5,  32(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  3: mod[2]  at 40(sp)  */
"ld   t5,  40(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  4: mod[3]  at 48(sp)  */
"ld   t5,  48(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  5: mod[4]  at 56(sp)  */
"ld   t5,  56(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  6: mod[5]  at 64(sp)  */
"ld   t5,  64(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  7: mod[6]  at 72(sp)  */
"ld   t5,  72(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  8: mod[7]  at 80(sp)  */
"ld   t5,  80(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot  9: mod[8]  at 88(sp)  */
"ld   t5,  88(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 10: mod[9]  at 96(sp)  */
"ld   t5,  96(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 11: mod[10] at 104(sp) */
"ld   t5, 104(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 12: mod[11] at 112(sp) */
"ld   t5, 112(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 13: mod[12] at 120(sp) */
"ld   t5, 120(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 14: mod[13] at 128(sp) */
"ld   t5, 128(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 15: mod[14] at 136(sp) */
"ld   t5, 136(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 16: mod[15] at 144(sp) */
"ld   t5, 144(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 17: mod[16] at 152(sp) */
"ld   t5, 152(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 18: mod[17] at 160(sp) */
"ld   t5, 160(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 19: mod[18] at 168(sp) */
"ld   t5, 168(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 20: mod[19] at 176(sp) */
"ld   t5, 176(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 21: mod[20] at 184(sp) */
"ld   t5, 184(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 22: mod[21] at 192(sp) */
"ld   t5, 192(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 23: mod[22] at 200(sp) */
"ld   t5, 200(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 24: mod[23] at 208(sp) */
"ld   t5, 208(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 25: mod[24] at 216(sp) */
"ld   t5, 216(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 26: mod[25] at 224(sp) */
"ld   t5, 224(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 27: mod[26] at 232(sp) */
"ld   t5, 232(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 28: mod[27] at 240(sp) */
"ld   t5, 240(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 29: mod[28] at 248(sp) */
"ld   t5, 248(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 30: mod[29] at 256(sp) */
"ld   t5, 256(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 31: mod[30] at 264(sp) */
"ld   t5, 264(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
/* Slot 32: mod[31] at 272(sp) */
"ld   t5, 272(sp)\n\t" "ld   t3, 0(t3)\n\t" ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"

"sd   t3,  0(sp)\n\t"           /* store updated addr */

/* ---- i += 32 ---- */
"ld   t0,  8(sp)\n\t"
"addi t0, t0, 32\n\t"
"sd   t0,  8(sp)\n\t"
"j    1b\n\t"

/* ---- EXIT ---- */
"3:\n\t"
"rdcycle t4\n\t"
"sub  %[ret], t4, t2\n\t"
"addi sp, sp, 288\n\t"
        : [ret] "=r"(ret)
        : [start] "r"(start),
          [iters] "r"(iters),
          [mods]  "r"(mods)
        : "t0","t1","t2","t3","t4","t5","t6","memory"
    );
return ret;
}

/* ------------------------------------------------------------ */
/* Median helper                                                */
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
/* main                                                         */
/* ------------------------------------------------------------ */
int main(int argc, char **argv) {
    uint64_t iters  = (argc > 1) ? strtoull(argv[1], 0, 0) : 200000000ULL;
    size_t   ring   = (argc > 2) ? (size_t)strtoull(argv[2], 0, 0) : 512;
    int      trials = (argc > 3) ? atoi(argv[3]) : 15;

    /* ring must be a multiple of 32 so the modifier rotation is consistent */
    if (ring < 32) ring = 32;
    ring = (ring + 31) & ~(size_t)31;  /* round up to multiple of 32 */
    if (trials < 1) trials = 1;

    /* --- raw ring --- */
    uintptr_t *nodes_raw = (uintptr_t *)malloc(ring * sizeof(uintptr_t));
    for (size_t i = 0; i < ring; i++)
        nodes_raw[i] = (uintptr_t)&nodes_raw[(i + 1) % ring];

    /* --- single-modifier ring (mod=0) --- */
    uintptr_t *nodes_signed = (uintptr_t *)malloc(ring * sizeof(uintptr_t));
    for (size_t i = 0; i < ring; i++)
        nodes_signed[i] =
            rocc_sign_mod0_inline((uintptr_t)&nodes_signed[(i + 1) % ring]);

    /* --- single-modifier ring (mod=0x12345678) --- */
    uintptr_t *nodes_signed_ia = (uintptr_t *)malloc(ring * sizeof(uintptr_t));
    for (size_t i = 0; i < ring; i++)
        nodes_signed_ia[i] =
            rocc_sign_modia_inline((uintptr_t)&nodes_signed_ia[(i + 1) % ring],
                                   0x12345678ULL);

    /* --- interleaved ring: node i signed with modifiers[i % 32] --- */
    uintptr_t *nodes_interleaved = (uintptr_t *)malloc(ring * sizeof(uintptr_t));
    for (size_t i = 0; i < ring; i++)
        nodes_interleaved[i] =
            rocc_sign_modia_inline((uintptr_t)&nodes_interleaved[(i + 1) % ring],
                                   modifiers[i % NUM_MODS]);

    /* warm up */
    run_nopac((uintptr_t)&nodes_raw[0], ring * 16);
    run_pac((uintptr_t)&nodes_signed[0], ring * 16);
    run_pac_ia((uintptr_t)&nodes_signed_ia[0], ring * 16, 0x12345678ULL);
    run_pac_ia_reload((uintptr_t)&nodes_interleaved[0], ring * 16, modifiers);

    uint64_t *nopac      = malloc(trials * sizeof(uint64_t));
    uint64_t *pac        = malloc(trials * sizeof(uint64_t));
    uint64_t *pac_ia     = malloc(trials * sizeof(uint64_t));
    uint64_t *pac_ia_rel = malloc(trials * sizeof(uint64_t));

    printf("iters=%llu ring=%zu trials=%d (Apple-shaped RISC-V)\n\n",
           (unsigned long long)iters, ring, trials);

    for (int t = 0; t < trials; t++) {
        nopac[t]      = run_nopac((uintptr_t)&nodes_raw[0], iters);
        pac[t]        = run_pac((uintptr_t)&nodes_signed[0], iters);
        pac_ia[t]     = run_pac_ia((uintptr_t)&nodes_signed_ia[0], iters, 0x12345678ULL);
        pac_ia_rel[t] = run_pac_ia_reload((uintptr_t)&nodes_interleaved[0], iters, modifiers);

        printf("trial %2d: nopac=%llu pac=%llu pac_ia=%llu pac_ia_reload=%llu\n",
               t,
               (unsigned long long)nopac[t],
               (unsigned long long)pac[t],
               (unsigned long long)pac_ia[t],
               (unsigned long long)pac_ia_rel[t]);
    }

    printf("\nmedian nopac      = %llu cycles\n",
           (unsigned long long)median_u64(nopac, trials));
    printf("median pac        = %llu cycles  delta=%+lld\n",
           (unsigned long long)median_u64(pac, trials),
           (long long)(median_u64(pac, trials) - median_u64(nopac, trials)));
    printf("median pac_ia     = %llu cycles  delta=%+lld\n",
           (unsigned long long)median_u64(pac_ia, trials),
           (long long)(median_u64(pac_ia, trials) - median_u64(nopac, trials)));
    printf("median pac_ia_rel = %llu cycles  delta=%+lld\n",
           (unsigned long long)median_u64(pac_ia_rel, trials),
           (long long)(median_u64(pac_ia_rel, trials) - median_u64(nopac, trials)));

    free(nodes_raw); free(nodes_signed);
    free(nodes_signed_ia); free(nodes_interleaved);
    free(nopac); free(pac); free(pac_ia); free(pac_ia_rel);

    return (int)(sink & 1);
}
