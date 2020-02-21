#include <books.hpp>

#include <bitset>
#include <numeric>
#include <iostream>
#include <set>

bool
byPotential(const Library &library1, const Library &library2) {
    const auto score1 = (double)library1.throughput / (double)library1.totalPoints;
    const auto score2 = (double)library2.throughput / (double)library2.totalPoints;
    return score1 > score2;
}

Solver maxSolver([](Problem &problem, const Options &) {
    Solution solution;
    std::bitset<100000> alreadySent;

    for (auto &library: problem.libraries) {
        library.totalPoints = std::accumulate(library.books.begin(), library.books.end(), 0);
    }

    std::sort(problem.libraries.begin(), problem.libraries.end(), byPotential);

    const auto bookByScore = [&problem](unsigned int book1, unsigned int book2) {
        return problem.bookScores[book1] > problem.bookScores[book2];
    };

    for (auto &library : problem.libraries) {
        std::sort(library.books.begin(), library.books.end(), bookByScore);
    }

    auto currentLibrary = 0;
    int remainingDays = problem.libraries[currentLibrary].signUpTime;

    for (auto day = 0; day < problem.dayCount; day++) {
        std::cerr << day << "/" << problem.dayCount << std::endl;

        if (remainingDays == 0) {
            Subscription newSubscription;
            newSubscription.libraryId = problem.libraries[currentLibrary].id;
            solution.subscriptions.push_back(newSubscription);
            currentLibrary++;
            remainingDays = problem.libraries[currentLibrary].signUpTime;
        }

        remainingDays--;

        std::set<unsigned int> emptySubscriptions;

        for (auto &subscription : solution.subscriptions) {

            if (emptySubscriptions.count(subscription.libraryId)) {
                continue;
            }

            const auto &library = problem.libraries[subscription.libraryId];
            for (int out = 0; out < library.throughput; out++) {
                bool hasBeenAdded = false;

                for (auto book : library.books) {
                    if (!alreadySent[book]) {
                        subscription.bookIds.push_back(book);
                        alreadySent[book] = true;
                        hasBeenAdded = true;
                        break;
                    }
                }

                if (!hasBeenAdded) {
                    emptySubscriptions.insert(subscription.libraryId);
                }
            }
        }
    }

    solution.subscriptions.erase(
        std::remove_if(
            solution.subscriptions.begin(),
            solution.subscriptions.end(),
            [](auto &subscription) {
                return subscription.bookIds.size() == 0;
            }
        ),
        solution.subscriptions.end()
    );

    return solution;
});

