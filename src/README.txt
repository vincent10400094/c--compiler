Additional errors we check: (We have also done bonus errors specified in spec)

1. SYMBOL_REDECLARE_DIFFERENT_KIND:

    example:
        typedef int a;
        int a;

    error: redefinition of 'a' as different kind of symbol

2. VOID_VARIABLE:

    example:
        typedef void VOID;
        VOID a;
    error: variable has incomplete type 'VOID' (aka 'void')

3. IS_TYPE_NOT_VARIABLE:

    example:
        typedef int a;
        int b = a + 3;

    error: unexpected type name 'a': expected expression

4. RETURN_ARRAY:

    example:
        typedef int A[2];
        A function() {}
    error: function cannot return array type 'A' (aka 'int [2]')

5. ARRAY_SIZE_NOT_INT:

    example:
        int A[1.1]; // or
        int A[5 + 3 - 1.1];
    
    error: size of array has non-integer type 'float'

6. NOT_FUNCTION_NAME
   
    example:
        int a;
        a();
    error: called object type 'int' is not a function or function pointer
   