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

struct Subscription {
    unsigned int libraryId;
    std::vector<unsigned int> bookIds;
};

struct Solution {
    std::vector<Subscription> subscriptions;
};

std::istream &
operator>>(std::istream &in, Problem &problem) {
    in >> problem.bookCount;
    in >> problem.libraryCount;
    in >> problem.dayCount;

    std::copy_n(
        std::istream_iterator<std::uint16_t>(in),
        problem.bookCount,
        std::back_inserter(problem.bookScores)
    );

    std::copy_n(
        std::istream_iterator<Library>(in),
        problem.libraryCount,
        std::back_inserter(problem.libraries)
    );

    return in;
}

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
        std::ostream_iterator<unsigned int>(std::cout, "\n")
    );
    return out;
}

int
main(int argc, char const **argv) {
    Problem problem;

    std::cin >> problem;
    std::cout << problem << std::endl;

    return 0;
}
