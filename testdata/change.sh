#!/bin/bash
files="area              fib-matrix        globals    large-local-arr-2 merge-sort        simple            while
assign-2          fib-recursive     hello-world       int-arith         large-local-arr   nothing           spill-float
assign            fib               if-2              int-cmp           lis               partial-sum       spill
control           float-arith                     io                locals-arr        popcount          sqrt
expr-2            float-cmp         if-4              large-float       locals            scope-2           while-2
expr              floyd             if-5              large-global-arr  long-jump         scope-3           while-3
fact              func              if-while          large-int         long-long-jump    scope             while-4"
for file_ in $files;
do
    echo $file_
    python3 change.py $file_ > tmp.c
    gcc tmp.c
    ./a.out > $file_.output
done
