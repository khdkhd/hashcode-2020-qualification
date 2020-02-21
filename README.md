# hashcode-2020-qualification
Book scanning problem

## How to add a solver

To add a solver named *foo* to the project follow these steps:

1. Create the file `cpp/solvers/foo.cpp` folder,
2. Copy paste the following lines
```cpp
#include <books.hpp>

Solver fooSolver([](Problem &problem, const Options &) {
    Solution solution;

    // YOUR SOLVER IMPLEMENTATION HERE

    return solution;
});
```