# C--Compiler

C-- compiler implementation for the course "CSIE3230 Introduction to Compilers" 2020 fall.

## Compilation

**Notice**

If you're under Linux environment, remember to uncomment line 8 and comment line 9 in the Makefile.

```
$ cd src/ && make
```

## Usage

```
$ ./parser < [your c-- program]
```

And it will produce RISC-V assembly code `output.s`. Then use qemu-emulator to run the compiled code:

```
$ riscv64-linux-gnu-gcc -O0 -static main.S 
$ qemu-riscv64 ./a.out
```
