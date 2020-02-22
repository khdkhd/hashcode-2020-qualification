#include <algorithm>
#include <iostream>
#include <istream>
#include <fstream>
#include <exception>

#include <books.hpp> 
#include <solver.hpp>

int
main(int argc, char const **argv) {
    Problem problem;

    try {
        if (argc < 2) {
            std::cerr << "Usage: hascode-2020-books <SOLVER_NAME> [FILE]" << std::endl;
            return 1;
        }

        Solver solve = createSolver(argv[1]);

        if (argc < 3) {
            std::cin >> problem;
        } else {
            if (auto in = std::ifstream(argv[2])) {
                in >> problem;
            } else {
                std::cerr << "Failed to open: " << std::quoted(argv[2]) << std::endl;
                return 1;
            }
        }

        Solution solution = solve(problem, {});

        std::cout << solution << std::endl;
        std::cerr << "Score: " << score(problem, solution) << std::endl;
    } catch (std::invalid_argument &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
