
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
/* PRINCE SIGN (modifier = 0)                                   */
/* ------------------------------------------------------------ */
static inline uintptr_t rocc_sign_mod0_inline(uintptr_t val) {
    register uint64_t rd  asm("x10");
    register uint64_t rs1 asm("x11") = (uint64_t)val;
    register uint64_t rs2 asm("x12") = 0;

    asm volatile(
        ".word " STR(CUSTOMX(PRINCE_OPCODE, 1, 1, 1, 10, 11, 12, 1))
        : "=r"(rd)
        : "r"(rs1), "r"(rs2)
        : "memory"
    );
    return (uintptr_t)rd;
}

static inline uintptr_t rocc_sign_modia_inline(uintptr_t val, uintptr_t mod) {
    register uint64_t rd  asm("x10");
    register uint64_t rs1 asm("x11") = (uint64_t)val;
    register uint64_t rs2 asm("x12") = (uint64_t)mod;

    asm volatile(
        ".word " STR(CUSTOMX(PRINCE_OPCODE, 1, 1, 1, 10, 11, 12, 1))
        : "=r"(rd)
        : "r"(rs1), "r"(rs2)
        : "memory"
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
        /* stack layout (40 bytes)
         *  0(sp):  unused
         *  8(sp):  i
         * 16(sp):  saved start (like AArch64 str x0,[sp,#0x10])
         * 24(sp):  addr slot   (like AArch64 [sp,#0x18])
         * 32(sp):  iters slot  (AArch64 loaded it via fp-relative; we keep it here)
         */
        "addi sp, sp, -40\n\t"
        "fence iorw, iorw\n\t"

        /* ---- NOT TIMED: required init so loop is correct ---- */
        "sd   %[start], 24(sp)\n\t"      /* addr slot init (not shown in your mac snippet) */
        "sd   %[iters], 32(sp)\n\t"      /* iters in memory */

        /* ---- TIMING START (now only 2 stores like your mac trace) ---- */
        "rdcycle t2\n\t"                 /* c0 */

        /* matches: str x0,[sp,#0x10] ; str xzr,[sp,#0x8] */
        "sd   %[start], 16(sp)\n\t"      /* store #1 */
        "sd   zero,      8(sp)\n\t"      /* store #2 */

        /* matches: b loop_header */
        "j    1f\n\t"

        /* ---- LOOP HEADER ---- */
        "1:\n\t"
        /* matches: ldr x8,[sp,#0x8] ; ldur x9,[x29,#-0x10] */
        "ld   t0, 8(sp)\n\t"             /* load #1: i */
        "ld   t1, 32(sp)\n\t"            /* load #2: iters */
        "bge  t0, t1, 3f\n\t"            /* if (i >= iters) exit */

        /* ---- BODY ---- */
        /* matches: ldr x8,[sp,#0x18] ; ldr x8,[x8] ; str x8,[sp,#0x18] */
        "ld   t3, 24(sp)\n\t"            /* load #3: addr slot */
        "ld   t3, 0(t3)\n\t"             /* load #4: *addr */
        "sd   t3, 24(sp)\n\t"            /* store #3: addr slot */

        /* matches: ldr x8,[sp,#0x8] ; add ; str x8,[sp,#0x8] */
        "ld   t0, 8(sp)\n\t"             /* load #5: reload i */
        "addi t0, t0, 1\n\t"
        "sd   t0, 8(sp)\n\t"             /* store #4: i */

        "j    1b\n\t"

        /* ---- EXIT ---- */
        "3:\n\t"
        "rdcycle t4\n\t"                 /* c1 */
        "sub  %[ret], t4, t2\n\t"

        "addi sp, sp, 40\n\t"
        : [ret] "=r"(ret)
        : [start] "r"(start),
          [iters] "r"(iters)
        : "t0", "t1", "t2", "t3", "t4", "memory"
    );
    return ret;
}




/* ============================================================ */
/* Apple-shaped PAC (LD + PRINCE AUTH)                          */
/* ============================================================ */
__attribute__((noinline))
uint64_t run_pac(uintptr_t start, uint64_t iters) {
    uint64_t ret;
    asm volatile(
        /* stack layout identical to run_nopac */
        "addi sp, sp, -40\n\t"

        /* ---- TIMING START ---- */
        "fence iorw, iorw\n\t"
        "sd   zero, 0(sp)\n\t"         /* i = 0 */
        "rdcycle t2\n\t"              /* c0 */

        /* Apple-timed prologue */
        
        "sd   %[start], 8(sp)\n\t"     /* addr = start */
        "sd   %[iters], 32(sp)\n\t"    /* iters */
        "j    1f\n\t"

        /* ---- LOOP ---- */
        "1:\n\t"
        "ld   t0, 0(sp)\n\t"
        "ld   t6, 32(sp)\n\t"
        "bge  t0, t6, 3f\n\t"
        // "j    2f\n\t"

        /* ---- BODY ---- */
        // "2:\n\t"
        "ld   t1, 8(sp)\n\t"
        "ld   t3, 0(t1)\n\t"


        /* PRINCE AUTH (modifier = 0) */
        ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,0,0)) "\n\t"

        /* spill / reload (Apple-style) */
        "sd   t3, 16(sp)\n\t"
        "ld   t3, 16(sp)\n\t"
        "sd   t3, 8(sp)\n\t"

        /* i++ */
        "ld   t0, 0(sp)\n\t"
        "addi t0, t0, 1\n\t"
        "sd   t0, 0(sp)\n\t"
        "j    1b\n\t"

        /* ---- EXIT ---- */
        "3:\n\t"
        "rdcycle t4\n\t"
        "sub  %[ret], t4, t2\n\t"

        "addi sp, sp, 40\n\t"
        : [ret] "=r"(ret)
        : [start] "r"(start),
          [iters] "r"(iters)
        : "t0","t1","t2","t3","t4","t6","memory"
    );
    return ret;
}


/* ============================================================ */
/* Apple-shaped PAC (LD + PRINCE AUTH) with imidiate modifier   */
/* ============================================================ */
__attribute__((noinline))
uint64_t run_pac_ia(uintptr_t start, uint64_t iters, uintptr_t mod) {
uint64_t ret;
asm volatile(
        /* stack layout (48 bytes) - EXPANDED
         *  0(sp):  modlocal    
         *  8(sp):  i           
         * 16(sp):  saved start 
         * 24(sp):  tmp         
         * 32(sp):  addrslot    
         * 40(sp):  iters       <- NEW
         */
"addi sp, sp, -48\n\t"          /* CHANGED: 48 instead of 40 */
"fence iorw, iorw\n\t"
        /* ---- NOT TIMED: init locals ---- */
"sd   %[mod],   0(sp)\n\t"      
"sd   %[start], 32(sp)\n\t"     
"sd   %[iters], 40(sp)\n\t"     /* NEW: store iters */
        /* ---- TIMING START ---- */
"rdcycle t2\n\t"
"sd   %[start], 16(sp)\n\t"     
"sd   zero,      8(sp)\n\t"     
"j    1f\n\t"
        /* ---- LOOP HEADER ---- */
"1:\n\t"
"ld   t0, 8(sp)\n\t"            
"ld   t6, 40(sp)\n\t"           /* CHANGED: load from stack instead of mv */
"bge  t0, t6, 3f\n\t"
        /* ---- BODY ---- */
"ld   t1, 32(sp)\n\t"           
"ld   t5,  0(sp)\n\t"           
"ld   t3, 0(t1)\n\t"            
        /* AUTHIA ptr=t3 with modifier=t5    ".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"*/
".word " STR(CUSTOMX(PRINCE_OPCODE,1,1,1,28,28,30,0)) "\n\t"
"sd   t3, 24(sp)\n\t"           
"ld   t3, 24(sp)\n\t"           
"sd   t3, 32(sp)\n\t"           
        /* ---- i++ ---- */
"ld   t0, 8(sp)\n\t"            
"addi t0, t0, 1\n\t"
"sd   t0, 8(sp)\n\t"            
"j    1b\n\t"
        /* ---- EXIT ---- */
"3:\n\t"
"rdcycle t4\n\t"
"sub  %[ret], t4, t2\n\t"
"addi sp, sp, 48\n\t"           /* CHANGED: 48 instead of 40 */
        : [ret] "=r"(ret)
        : [start] "r"(start),
          [iters] "r"(iters),
          [mod] "r"(mod)
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
    size_t   ring   = (argc > 2) ? (size_t)strtoull(argv[2], 0, 0) : 2048;
    int      trials = (argc > 3) ? atoi(argv[3]) : 15;

    if (ring < 2) ring = 2;
    if (trials < 1) trials = 1;

    uintptr_t *nodes_raw =
        (uintptr_t *)malloc(ring * sizeof(uintptr_t));
    uintptr_t *nodes_signed =
        (uintptr_t *)malloc(ring * sizeof(uintptr_t));
    uintptr_t *nodes_signed_ia =
        (uintptr_t *)malloc(ring * sizeof(uintptr_t));

    for (size_t i = 0; i < ring; i++) {
        nodes_raw[i] = (uintptr_t)&nodes_raw[(i + 1) % ring];
        nodes_signed[i] =
            rocc_sign_mod0_inline((uintptr_t)&nodes_signed[(i + 1) % ring]);
        nodes_signed_ia[i] =
            rocc_sign_modia_inline((uintptr_t)&nodes_signed_ia[(i + 1) % ring], 0x12345678ULL);
    }

    run_nopac((uintptr_t)&nodes_raw[0], ring * 16);
    run_pac((uintptr_t)&nodes_signed[0], ring * 16);
    run_pac_ia((uintptr_t)&nodes_signed_ia[0], ring * 16, 0x12345678ULL);

    uint64_t *nopac = malloc(trials * sizeof(uint64_t));
    uint64_t *pac   = malloc(trials * sizeof(uint64_t));
    uint64_t *pac_ia   = malloc(trials * sizeof(uint64_t));

    printf("iters=%llu ring=%zu trials=%d (Apple-shaped RISC-V)\n\n",
           (unsigned long long)iters, ring, trials);

    for (int t = 0; t < trials; t++) {
        nopac[t] = run_nopac((uintptr_t)&nodes_raw[0], iters);
        pac[t]   = run_pac((uintptr_t)&nodes_signed[0], iters);
        pac_ia[t]= run_pac_ia((uintptr_t)&nodes_signed_ia[0], iters, 0x12345678ULL);

        printf("trial %2d: nopac=%llu pac=%llu pac_ia=%llu delta=%+lld\n",
        // printf("trial %2d: nopac=%llu pac=%llu delta=%+lld\n",
               t,
               (unsigned long long)nopac[t],
               (unsigned long long)pac[t],
               (unsigned long long)pac_ia[t],
               (long long)(pac[t] - nopac[t]));
    }

    printf("\nmedian delta = %+lld cycles\n",
           (long long)(median_u64(pac, trials) -
                       median_u64(nopac, trials)));

    return (int)(sink & 1);
}
