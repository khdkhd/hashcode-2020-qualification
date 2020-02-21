#include <algorithm>
#include <iostream>
#include <istream>
#include <fstream>
#include <exception>

#include <books.hpp>
#include <solver.hpp>

std::istream &
operator>>(std::istream &in, Problem &problem) {
    in >> problem.bookCount;
    in >> problem.libraryCount;
    in >> problem.dayCount;

    std::copy_n(
        std::istream_iterator<unsigned int>(in),
        problem.bookCount,
        std::back_inserter(problem.bookScores)
    );

    std::copy_n(
        std::istream_iterator<Library>(in),
        problem.libraryCount,
        std::back_inserter(problem.libraries)
    );
    
    unsigned int library_index = 0;
    for (auto &library: problem.libraries) {
        library.id = library_index++;
    }

    return in;
}

std::istream &
operator>>(std::istream &in, Library &library) {
    in >> library.bookCount;
    in >> library.signUpTime;
    in >> library.throughput;

    library.books.clear();
    std::copy_n(
        std::istream_iterator<unsigned int>(in),
        library.bookCount,
        std::back_inserter(library.books)
    );

    return in;
}

std::ostream &
operator<<(std::ostream &out, const Library &library) {
    std::cout
        << "        library.id " << library.id << std::endl
        << "library.signUpTime " << library.signUpTime << std::endl
        << "library.throughput " << library.throughput << std::endl
        << " library.bookCount " << library.bookCount << std::endl
        << "     library.books ";

    std::copy(
        library.books.begin(),
        library.books.end(),
        std::ostream_iterator<unsigned int>(std::cout, " ")
    );

    return out;
}

std::ostream &
operator<<(std::ostream &out, const Problem &problem) {
    out
        << "   problem.bookCount " << problem.bookCount << std::endl
        << "problem.libraryCount " << problem.libraryCount << std::endl
        << "    problem.dayCount " << problem.dayCount << std::endl
        << "   problem.bookScore ";

    std::copy(
        problem.bookScores.begin(),
        problem.bookScores.end(),
        std::ostream_iterator<unsigned int>(out, " ")
    );

    out << std::endl;

    std::copy(
        problem.libraries.begin(),
        problem.libraries.end(),
        std::ostream_iterator<Library>(out, "\n")
    );

    return out;
}

std::ostream &
operator<<(std::ostream &out, const Subscription &subscription) {
    out << subscription.libraryId << " " << subscription.bookIds.size() << '\n';
    std::copy(
        subscription.bookIds.begin(),
        subscription.bookIds.end(),
        std::ostream_iterator<unsigned int>(out, " ")
    );
    return out;
}

std::ostream &
operator<<(std::ostream &out, const Solution &solution) {
    out << solution.subscriptions.size() << '\n';
    std::copy(
        solution.subscriptions.begin(),
        solution.subscriptions.end(),
        std::ostream_iterator<Subscription>(out, "\n")
    );
    return out;
}

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

        std::cout << solve(problem, {});
    } catch (std::invalid_argument &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
