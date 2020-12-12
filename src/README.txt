Additional errors we check:

1. SYMBOL_REDECLARE_DIFFERENT_KIND:

    example:
        typedef int a;
        int a;

    error: redefinition of 'a' as different kind of symbol

2. VOID_VARIABLE:


3. IS_TYPE_NOT_VARIABLE:

    example:
        typedef int a;
        int b = a + 3;

    error: unexpected type name 'a': expected expression

4. RETURN_ARRAY:


5. ARRAY_SIZE_NOT_INT:

    example:
        int A[1.1]; // or
        int A[5 + 3 - 1.1];
    
    error: size of array has non-integer type 'float'

