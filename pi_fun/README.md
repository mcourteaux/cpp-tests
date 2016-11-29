# pi-fun

## Intoduction

A small pi calculating application that tests and compares different techniques.

Most of the source code is taken up by the simple algorithm:

    pi = 4 * atan(1)

This version is available in several flavors:
 - multithreaded, scalar
 - multithreaded, AVX 256 bit, double precision (4-wide)
 - multithreaded, AVX 256 bit, single precision (8-wide)
 - multithreaded, AVX 256 bit, single precision (8-wide), reverse order

A second approach that is taken is the more complex and faster converging
algorithm:

    pi =  176*arctan(1/ 57) + 28*arctan(1/  239)
         - 48*arctan(1/682) + 96*arctan(1/12943)

This is only implemented using scalers, both multithreaded and singlethreaded.

Additionally there is a high precision library ttmath used to calculate Pi with
high precision using the fast converging algorithm.

For ttmath: http://www.ttmath.org/

## Usage

To build:

    make

To run with the default number of cores on your machine:

    ./pi

Or specify how many threads should be used manually:

    ./pi 2
