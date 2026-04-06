# Micro-Benchmarks for Apple M1 (ARM64 PAC)

These benchmarks are designed to run on Apple M1 (or any ARM64 machine that supports PAC instructions).
Each benchmark is compiled twice — once without PAC and once with PAC — so you can inspect and compare the generated assembly.

## Requirements

- Apple M1 (or ARM64 machine with PAC support)
- macOS with Xcode command line tools (`xcode-select --install`)
- `clang` (comes with Xcode)
- `otool` (comes with Xcode)

---

## Benchmarks

### loop.c

```bash
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector loop.c -o loop_nopac
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector -mbranch-protection=pac-ret+leaf loop.c -o loop_pac
```

Verify assembly:
```bash
otool -tvV ./loop_nopac
otool -tvV ./loop_pac
```

---

### loop_nested.c

```bash
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector loop_nested.c -o loop_nested_nopac
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector -mbranch-protection=pac-ret+leaf loop_nested.c -o loop_nested_pac
```

Verify assembly:
```bash
otool -tvV ./loop_nested_nopac
otool -tvV ./loop_nested_pac
```

---

### deep_recursive_return.c

```bash
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector deep_recursive_return.c -o deep_recursive_return_nopac
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector -mbranch-protection=pac-ret+leaf deep_recursive_return.c -o deep_recursive_return_pac
```

---

### deep_recursive_entry.c

```bash
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector deep_recursive_entry.c -o deep_recursive_entry_nopac
clang -O3 -Wall -Wextra -arch arm64 -fno-stack-protector -mbranch-protection=pac-ret+leaf deep_recursive_entry.c -o deep_recursive_entry_pac
```

---

### data_loop.c and data_unrolled32.c

These are compiled without optimization to preserve the data-access structure for inspection.

```bash
clang -O0 -fno-unroll-loops -fno-vectorize -fno-slp-vectorize \
    -fno-inline -fno-builtin -arch arm64 \
    data_loop.c -o data_loop

clang -O0 -fno-unroll-loops -fno-vectorize -fno-slp-vectorize \
    -fno-inline -fno-builtin -arch arm64 \
    data_unrolled32.c -o data_unrolled32
```

Verify assembly:
```bash
otool -tvV ./data_loop
otool -tvV ./data_unrolled32
```

---

## What to Look For

When comparing `_nopac` vs `_pac` binaries with `otool`, look for:
- `paciasp` / `pacibsp` — PAC signing of return address on function entry
- `retaa` / `retab` — authenticated return instead of plain `ret`
- `autiasp` / `autibsp` — PAC authentication before return