#ifndef PAC_RUNTIME_H
#define PAC_RUNTIME_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uintptr_t prince_sign(uintptr_t plaintext, uint64_t modifier);
uintptr_t prince_auth(uintptr_t ciphertext, uint64_t modifier);
void prince_set_key(uint64_t key_hi, uint64_t key_lo);

#ifdef __cplusplus
}
#endif
#endif

