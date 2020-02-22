#include <algorithm>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <memory>

#include <books.hpp>
#include <solver.hpp>

std::intmax_t
getMaxIteration() {
    const auto envvar_value = std::getenv("BOOKS_MAX_ITERATION");
    if (envvar_value != nullptr) {
        std::istringstream iss(envvar_value);
        std::intmax_t max_iteration;
        iss >> max_iteration;
        return max_iteration;
    }
    return 1000;
}

std::string
getOutputDirectory() {
    const auto output_dir = std::getenv("BOOKS_OUTPUT_DIR");
    if (output_dir != nullptr) {
        return output_dir;
    }
    return std::getenv("PWD");
}

std::string
basename(const std::string &filepath) {
    return std::string(
        filepath.begin() + filepath.find_last_of('/') + 1,
        filepath.begin() + filepath.find_last_of('.')
    );
}

std::tuple<std::shared_ptr<std::istream>, std::shared_ptr<std::ostream>>
openStreams(const std::string &input_filepath) {
    if (input_filepath == "") {
        return std::make_tuple(
            std::make_shared<std::istream>(std::cin.rdbuf()),
            std::make_shared<std::ostream>(std::cout.rdbuf())
        );
    }
    const auto output_filepath = getOutputDirectory() + "/" + basename(input_filepath) + ".out";
    const auto in = std::make_shared<std::ifstream>(input_filepath);
    const auto out = std::make_shared<std::ofstream>(output_filepath);

    if (!*in) {
        throw std::ios_base::failure(std::string("Cannot open ") + input_filepath);
    }

    if (!*out) {
        throw std::ios_base::failure(std::string("Cannot open ") + output_filepath);
    }

    return std::make_tuple(in, out);
}


int
main(int argc, char const **argv) {


    try {
        if (argc < 2) {
            std::cerr << "Usage: hascode-2020-books <SOLVER_NAME> [FILE]" << std::endl;
            return 1;
        }

        Solver solve = createSolver(argv[1]);

        std::vector<std::string> input_filepaths;

        if (argc >= 3) {
            std::copy(argv + 2, argv + argc, std::back_inserter(input_filepaths));
        } else {
            input_filepaths.emplace_back("");
        }

        std::vector<std::tuple<std::string, Problem, Solution>> solutions;
        std::transform(
            input_filepaths.begin(),
            input_filepaths.end(),
            std::back_inserter(solutions),
            [&solve](const auto &input_filepath) {
                auto [input, output] = openStreams(input_filepath);

                Problem problem;
                *input >> problem;

                Solution solution = solve(problem, Options{
                    {"max-iteration", getMaxIteration()},
                });

                *output << solution;

                return std::make_tuple(
                    input_filepath.size() == 0 ? "" : basename(input_filepath),
                    problem,
                    solution
                );
            }
        );

        unsigned int total_score = 0;
        for (const auto &[name, problem, solution]: solutions) {
            const auto solution_score = score(problem, solution);
            total_score += solution_score;
            std::cerr << name << " score: " << solution_score << std::endl;
        }
        std::cerr << "total score: " << total_score << std::endl;
    } catch (std::ios_base::failure &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    } catch (std::invalid_argument &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
