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

Solver jux2Solver([](Problem &problem, const Options &) {
    Solution solution;

    std::bitset<1000000> bookState; // 0: taken, 1: available
    bookState.set();

    // Sort libraries by signUpTime
    std::cerr << "Sort libraries" << std::endl;
    std::sort(
        problem.libraries.begin(),
        problem.libraries.end(),
        [](const auto &library1, const auto &library2) {
            return library1.signUpTime < library2.signUpTime;
        });
    std::cerr << "Sort libraries - done" << std::endl;

    // Sort libraries books by score
    std::cerr << "Sort libraries books" << std::endl;
    for (auto &library : problem.libraries) {
        std::sort(
            library.books.begin(),
            library.books.end(),
            [&problem](auto book_id1, auto book_id2) {
                return problem.bookScores[book_id1] > problem.bookScores[book_id2];
            });
    }
    std::cerr << "Sort libraries books - done" << std::endl;

    auto sign_up_offset = 0u;

    std::cerr << "Compute solution" << std::endl;
    for (const auto &library : problem.libraries) {
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
    std::cerr << "Compute solution - done" << std::endl;

    return solution;
});
