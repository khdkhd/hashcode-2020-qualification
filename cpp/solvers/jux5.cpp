#include <books.hpp>

#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>

namespace {
unsigned int
remainingDays(Problem &problem, unsigned int days) {
    if (days >= problem.dayCount) {
        return 0;
    }
    return problem.dayCount - days;
}
}

Solver jux5Solver([](Problem &problem, const Options &) {
    Solution solution;

    std::bitset<1000000> bookState; // 0: already sent, 1: available
    bookState.set();

    // Sort libraries books by their score and remove duplicates
    std::cerr << "sort libraries books" << std::endl;
    for (auto &library: problem.libraries) {
        std::sort(
            library.books.begin(),
            library.books.end(),
            [&problem](auto book_id1, auto book_id2) {
                return problem.bookScores[book_id1] > problem.bookScores[book_id2];
            }
        );
    }
    std::cerr << "sort libraries books - done!" << std::endl;

    // Compute library score
    std::cerr << "compute libraries score" << std::endl;
    std::vector<unsigned int> library_scores;
    std::transform(
        problem.libraries.begin(),
        problem.libraries.end(),
        std::back_inserter(library_scores),
        [&problem](const auto &library) {
            const auto bookCount = (problem.dayCount - library.signUpTime)*library.throughput;
            return std::accumulate(
                library.books.begin(),
                library.books.begin() + std::min<std::size_t>(library.books.size(), bookCount),
                0u,
                [&problem](auto acc, auto book_id) {
                    return acc + problem.bookScores[book_id];
                }
            );
        }
    );
    std::cerr << "compute libraries score" << std::endl;

    // Sort library score
    std::cerr << "sort libraries" << std::endl;
    std::sort(
        problem.libraries.begin(),
        problem.libraries.end(),
        [&library_scores](auto &library1, auto &library2) {
            return library1.signUpTime == library2.signUpTime
                ? library_scores[library1.id] > library_scores[library2.id]
                : library1.signUpTime < library2.signUpTime;
        }
    );
    std::cerr << "sort libraries - done!" << std::endl;

    auto sign_up_offset = 0u;

    // Compute solution
    std::cerr << "compute solution" << std::endl;
    for (const auto &library: problem.libraries) {
        sign_up_offset += library.signUpTime;
        auto sentBookCount = remainingDays(problem, sign_up_offset)*library.throughput;

        Subscription subscription{library.id};

        if (bookState.any()) {
            for (auto book_id: library.books) {
                if (sentBookCount == 0) {
                    break;
                }
                if (bookState[book_id]) {
                    bookState[book_id] = false;
                    sentBookCount = sentBookCount - 1;
                    subscription.bookIds.emplace_back(book_id);
                }
            }
            if (subscription.bookIds.size() > 0) {
                solution.subscriptions.emplace_back(subscription);
            }
        }
    }
    std::cerr << "compute solution - done!" << std::endl;

    return solution;
});

