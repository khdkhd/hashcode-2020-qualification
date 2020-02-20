#include <books.hpp>

#include <bitset>
#include <numeric>

bool
byPotential(const Library &library1, const Library &library2) {
    const auto score1 = (double)library1.throughput / (double)library1.totalPoints;
    const auto score2 = (double)library2.throughput / (double)library2.totalPoints;
    return score1 > score2;
}

Solution
solve(Problem &problem) {
    Solution solution;
    std::bitset<100000> alreadySent;
    std::vector<Library> sortedLibraries = problem.libraries;

    for (auto &library: problem.libraries) {
        library.totalPoints = std::accumulate(library.books.begin(), library.books.end(), 0);
    }

    std::sort(sortedLibraries.begin(), sortedLibraries.end(), byPotential);

    const auto bookByScore = [&problem](unsigned int book1, unsigned int book2) {
        return problem.bookScores[book1] > problem.bookScores[book2];
    };

    for (auto &library : sortedLibraries) {
        std::sort(library.books.begin(), library.books.end(), bookByScore);
    }

    auto currentLibrary = 0;
    int remainingDays = sortedLibraries[currentLibrary].signUpTime;

    for (auto day = 0; day < problem.dayCount; day++) {
        if (remainingDays == 0) {
            Subscription newSubscription;
            newSubscription.libraryId = sortedLibraries[currentLibrary].id;
            solution.subscriptions.push_back(newSubscription);
            currentLibrary++;
            remainingDays = sortedLibraries[currentLibrary].signUpTime;
        }

        remainingDays--;

        for (auto &subscription : solution.subscriptions) {
            const auto &library = problem.libraries[subscription.libraryId];
            for (int out = 0; out < library.throughput; out++) {
                for (auto book : library.books) {
                    if (!alreadySent[book]) {
                        subscription.bookIds.push_back(book);
                        alreadySent[book] = true;
                    }
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
}