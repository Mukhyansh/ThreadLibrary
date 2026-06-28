## Benchmarks

These benchmarks allow the library to be tested against the standard `pthread` library using the **same source file and function syntax**.

By default, the benchmark uses the self-implemented user-thread library. The mapping between pthread-style function names and the custom library functions is defined in `uthreads.h`.

**Do look at the comments for better understanding.**

=> The benchmark computes the dot product of two vectors using multiple threads and measures the total execution time for:

1. The custom user-level thread library  
2. The standard pthread library  

This makes it possible to compare both implementations under the same workload without rewriting the benchmark code.
