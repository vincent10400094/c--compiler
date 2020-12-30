import sys
prev = "#include <stdio.h>\n#include <stdbool.h>\n#define printf_dec_format(x) _Generic((x), \
    char: \"%c\", \
    signed char: \"%hhd\", \
    unsigned char: \"%hhu\", \
    signed short: \"%hd\", \
    unsigned short: \"%hu\", \
    signed int: \"%d\", \
    unsigned int: \"%u\", \
    long int: \"%ld\", \
    unsigned long int: \"%lu\", \
    long long int: \"%lld\", \
    unsigned long long int: \"%llu\", \
    float: \"%f\", \
    double: \"%f\", \
    long double: \"%Lf\", \
    char *: \"%s\", \
    void *: \"%p\")\n#define write(x) printf(printf_dec_format(x), x)\nint read() {\n  int a;\n  scanf(\"%d\", &a);\n  return a;\n}"

print(prev)
filename = sys.argv[1]
with open(filename+".c", "r") as f:
    lines = f.readlines()

for line in lines:
    if "MAIN" in line:
        print("int main() {", end =" ")
    else :
        print(line, end =" ")
