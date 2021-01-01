* Extra credits:

1) Before each function call, dirty S-registers (hold modified data) will be stored to memory (with .offset or .reg attribute recorded in symbol table), and when the function returns, the caller will not immediately reload all S-registers, instead it loads them when needed. For T-registers, we implement the caller-save routine, that is, the caller will save all registers in-use to AR before the function call and restore all of them when the function returns. The number of register storing/saving is much less than the one from the sample output.s.

2) We use register tables to track the usage of registers (register.h, register.c), keeping as much variables in registers as possible. Writes of S-registers occur when there's no spare S-register possible or the program encounters jump instruction (e.g. function call or control statement).

* Some optimizations we implement:

1) Constant folding: Expressions with constants will be evaluated during compile-time.
