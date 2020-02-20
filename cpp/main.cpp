#include <iostream>
#include <istream>
#include <vector>

struct Library {
    unsigned int id = 0;
    unsigned int signUpTime;
    unsigned int throughput;
    unsigned int totalPoints;
    std::vector<unsigned int> books;
};

struct Problem {
    unsigned int bookCount;
    unsigned int libraryCount;
    unsigned int dayCount;
    std::vector<unsigned int> bookScores;
    std::vector<Library> libraries;
};

std::istream &
operator>>(std::istream &in, Library &library) {
    unsigned int bookCount;

    in >> bookCount;
    in >> library.signUpTime;
    in >> library.throughput;

    std::copy_n(
        std::istream_iterator<std::uint16_t>(in),
        bookCount,
        std::back_inserter(library.books)
    );

    return in;
}

namespace std {
std::ostream &
operator<<(std::ostream &out, const Library &library) {
    std::cout
        << "        library.id " << library.id << std::endl
        << "library.signUpTime " << library.signUpTime << std::endl
        << "library.throughput " << library.throughput << std::endl
        << "     library.books ";

    std::copy(
        library.books.begin(),
        library.books.end(),
        std::ostream_iterator<unsigned int>(std::cout, " ")
    );

    return out;
}
}


int
main(int argc, char const **argv) {
    Problem problem;

    std::cin >> problem.bookCount;
    std::cin >> problem.libraryCount;
    std::cin >> problem.dayCount;

    std::copy_n(
        std::istream_iterator<std::uint16_t>(std::cin),
        problem.bookCount,
        std::back_inserter(problem.bookScores)
    );

    std::copy_n(
        std::istream_iterator<Library>(std::cin),
        problem.libraryCount,
        std::back_inserter(problem.libraries)
    );

    std::cout
        << "   problem.bookCount " << problem.bookCount << std::endl
        << "problem.libraryCount " << problem.libraryCount << std::endl
        << "    problem.dayCount " << problem.dayCount << std::endl
        << "   problem.bookScore ";

    std::copy(
        problem.bookScores.begin(),
        problem.bookScores.end(),
        std::ostream_iterator<unsigned int>(std::cout, " ")
    );

    std::cout << std::endl;

    std::copy(
        problem.libraries.begin(),
        problem.libraries.end(),
        std::ostream_iterator<Library>(std::cout, "\n")
    );

    std::cout << std::endl;

    return 0;
}
