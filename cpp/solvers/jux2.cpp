#include <books.hpp>

#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>

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
        for (auto &library : problem.libraries)
        {
                std::sort(
                    library.books.begin(),
                    library.books.end(),
                    [&problem](auto book_id1, auto book_id2) {
                            return problem.bookScores[book_id1] > problem.bookScores[book_id2];
                    });
        }
        std::cerr << "Sort libraries books - done" << std::endl;

        std::cerr << "Compute solution" << std::endl;
        for (const auto &library : problem.libraries)
        {
                auto sentBookCount = library.throughput * (problem.dayCount - library.signUpTime);

                Subscription subscription{library.id};

                if (bookState.any())
                {
                        for (auto it = library.books.begin(), last = library.books.end(); it < last && sentBookCount > 0; ++it)
                        {
                                const auto book_id = *it;
                                if (bookState[book_id])
                                {
                                        bookState[book_id] = false;
                                        subscription.bookIds.emplace_back(book_id);
                                }
                        }

                        if (subscription.bookIds.size() > 0)
                        {
                                solution.subscriptions.emplace_back(subscription);
                        }
                }
        }
        std::cerr << "Compute solution - done" << std::endl;

        return solution;
});
