#include <stdint.h>
#include <stdio.h>          // optional (for debug print)
#include "rocc.h"
#include "pac_runtime.h"

#define GOOD_HI 0x0123456789ABCDEFULL
#define GOOD_LO 0x0FEDCBA987654321ULL

// The actual initializer. Keep it visible and non-inlined.
__attribute__((noinline, used))
static void pac_set_key_early_impl(void) {
  prince_set_key(GOOD_HI, GOOD_LO);
  __asm__ volatile("fence iorw, iorw" ::: "memory");
  // Optional proof it ran:
  printf("pac_runtime: key set (ctor)\n");
}

// 1) Normal constructor path (runs via .init_array)
__attribute__((constructor(50), used, noinline))
static void pac_set_key_early_ctor(void) {
  pac_set_key_early_impl();
}

// 2) Backup: place a pointer in .preinit_array so it runs even earlier.
// Some crt flows call __libc_init_array() which processes preinit+init arrays.
// Mark the array used so it can't be GC'd.
__attribute__((used, section(".preinit_array")))
static void (*const __pac_preinit_entry[])(void) = { pac_set_key_early_impl };

__attribute__((noinline))
uintptr_t prince_sign(uintptr_t plaintext, uint64_t modifier) {
  uintptr_t result = 0;
  ROCC_INSTRUCTION_DSS(1, result, plaintext, modifier, 1);
  return result;
}

__attribute__((noinline))
uintptr_t prince_auth(uintptr_t ciphertext, uint64_t modifier) {
  uintptr_t data_out = 0;
  ROCC_INSTRUCTION_DSS(1, data_out, ciphertext, modifier, 0);
  return data_out;
}

__attribute__((noinline))
void prince_set_key(uint64_t key_hi, uint64_t key_lo) {
  ROCC_INSTRUCTION_SS(1, key_hi, key_lo, 2);
  __asm__ volatile("fence iorw, iorw" ::: "memory");
}





























// #include <stdint.h>
// #include <stdio.h>          // optional (for debug print)
// #include "rocc.h"
// #include "pac_runtime.h"

// #define GOOD_HI 0x0123456789ABCDEFULL
// #define GOOD_LO 0x0FEDCBA987654321ULL

// // The actual initializer. Keep it visible and non-inlined.
// __attribute__((noinline, used))
// static void pac_set_key_early_impl(void) {
//   prince_set_key(GOOD_HI, GOOD_LO);
//   __asm__ volatile("fence iorw, iorw" ::: "memory");
//   // Optional proof it ran:
//   printf("pac_runtime: key set (ctor)\n");
// }

// // 1) Normal constructor path (runs via .init_array)
// __attribute__((constructor(50), used, noinline))
// static void pac_set_key_early_ctor(void) {
//   pac_set_key_early_impl();
// }

// // 2) Backup: place a pointer in .preinit_array so it runs even earlier.
// // Some crt flows call __libc_init_array() which processes preinit+init arrays.
// // Mark the array used so it can't be GC'd.
// __attribute__((used, section(".preinit_array")))
// static void (*const __pac_preinit_entry[])(void) = { pac_set_key_early_impl };

// __attribute__((noinline))
// uintptr_t prince_sign(uintptr_t plaintext,
//                       uint64_t modifier __attribute__((unused))) {
//   uintptr_t result;

//   // Bind this C variable to the stack pointer register x2 ("sp").
//   register uintptr_t sp_reg asm("sp");

//   // RoCC: rs1 = plaintext, rs2 = sp_reg (SP), funct = 1 (encrypt/sign)
//   ROCC_INSTRUCTION_DSS(1, result, plaintext, sp_reg, 1);
//   return result;
// }

// __attribute__((noinline))
// uintptr_t prince_auth(uintptr_t ciphertext,
//                       uint64_t modifier __attribute__((unused))) {
//   uintptr_t data_out;

//   // Again, use SP as the modifier source.
//   register uintptr_t sp_reg asm("sp");

//   // RoCC: rs1 = ciphertext (signed RA), rs2 = sp_reg (SP), funct = 0 (decrypt/auth)
//   ROCC_INSTRUCTION_DSS(1, data_out, ciphertext, sp_reg, 0);
//   return data_out;
// }

// __attribute__((noinline))
// void prince_set_key(uint64_t key_hi, uint64_t key_lo) {
//   ROCC_INSTRUCTION_SS(1, key_hi, key_lo, 2);
//   __asm__ volatile("fence iorw, iorw" ::: "memory");
// }
