CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Configuration


INTRODUCTION
------------

Calculates number with longest Collatz sequence below (and excluding) single argument to program.
Maximum number allowed in maximum unsigned integer value, typically 2,147,483,647.

Source code
    collatz.cpp
    fast_collatz.cpp

Executables

    collatz
            Compiled with command:      g++ -O3 collatz.cpp -o collatz

    fast_collatz
            Compiled with command:      g++ -O3 fast_collatz.cpp -lpthread -o fast_collatz


CONFIGURATION
-------------

fast_collatz (source: fast_collatz.cpp)

    Program will attempt to sense number of threads available, environment variable NUM_THREADS will override.
