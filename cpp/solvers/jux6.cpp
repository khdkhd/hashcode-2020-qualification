#include <books.hpp>
#include <solver.hpp>

#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>


Solver jux6Solver([](Problem &problem, const Options &options) {
    std::vector<Solver> solvers{
        createSolver("jux1"),
        createSolver("jux2"),
        createSolver("jux3"),
        createSolver("jux5"),
        createSolver("jux7"),
    };

    return std::reduce(
        solvers.begin(),
        solvers.end(),
        Solution(),
        [&problem, &options](auto best_solution, const auto &solve) {
            auto copy_problem = problem;
            auto solution = solve(copy_problem, options);
            if (score(problem, solution) > score(problem, best_solution)) {
                return solution;
            }
            return best_solution;
        }
    );
});

