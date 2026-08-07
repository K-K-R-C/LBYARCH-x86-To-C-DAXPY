# LBYARCH - DAXPY (C and x86-64 Assembly)

## Contributors
 
- **BENDOL, Trisha Mae R.**
- **CAMATO, Karl Kristoffer R.**

## Video Demo
Link: 

## What this project does
This program implements DAXPY (Z[i] = A * X[i] + Y[i]) in two versions:
1. A plain C kernel (`daxpy_c`)
2. An x86-64 assembly kernel using scalar SIMD instructions (`daxpy`, written in NASM)
Both versions are called from the same C main program. The C version is used as the "answer key" to check if the assembly version is computing the correct result.

## Files
- `daxpy.c` - contains the C kernel, main program, timing, and correctness check
- `daxpy.asm` - contains the assembly kernel using scalar SIMD (movsd, mulsd, addsd)

## Results

### Debug Mode

| Size (n) | C avg time (ms) | ASM avg time (ms) |
|---|---|---|
| 1,048,576 (2^20) | 4.42 | 1.90 |
| 16,777,216 (2^24) | 77.08 | 39.86 |
| 268,435,456 (2^28) | 1357.60 | 970.95 |
 
Correctness check: PASS for all sizes

### Release Mode
 
| Size (n) | C avg time (ms) | ASM avg time (ms) |
|---|---|---|
| 1,048,576 (2^20) | 1.75 | 1.85 |
| 16,777,216 (2^24) | 32.89 | 33.03 |
| 268,435,456 (2^28) | 866.16 | 822.30 |
 
Correctness check: PASS for all sizes

## Analysis
yada yada yada (edit this pls)
 
## Screenshots
 
### Debug Mode Output
Size: 2^20

![C Output](images/debug_20.png)

Size: 2^24

![C Output](images/debug_24.png)

Size: 2^28

![C Output](images/debug_28.png)

----

### Release Mode Output
Size: 2^20

![C Output](images/release_20.png)

Size: 2^24

![C Output](images/release_24.png)

Size: 2^28

![C Output](images/release_28.png)

 
