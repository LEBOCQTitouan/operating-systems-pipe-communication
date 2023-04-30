# Operating system : pipes

This package provides a simple way to transmit data between processes using pipes.

## Installation

### Dependencies

* [Make]("https://www.gnu.org/software/make/") / [CMake](https://cmake.org/) : For building the project

### Building

#### Using Make

```bash
make
```

#### Using CMake

```bash
mkdir build
cmake --build build
```

## Project description

This project provides a simple way to transmit data between processes using pipes.

It implements a simple algorithm used to compute prime numbers to a given limit. Here is a description of the algorithm:

### The parent process

1. Creation of the initial data array containing all the numbers from 2 to the limit.
2. Creation of a pipe and a child process
3. Sending data to the child process through the pipe
4. Sending END_SEQUENCE to the child process (end of array)

### The child process

1. Read initial data from the pipe (prime number)
2. While the data is not END_SEQUENCE
   1. If the data is not a multiple of the prime number (new prime number based on current prime number)
      1. If no child created yet
         1. Create a new child process and a new pipe
      2. Send data to the child process