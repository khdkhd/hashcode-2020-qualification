#include <vector>

#include <iostream>
#include <istream>
#include <ostream>
#include <map>
#include <bitset>

#include "structures.cpp"

struct Book
{
        unsigned id;
        unsigned score;
};

struct Library
{
        std::vector<Book> books;

        unsigned id;
        unsigned signUpTime;
        unsigned output;
        unsigned totalPoints;
};

struct Subscription
{
        unsigned libraryId;
        std::vector<unsigned> bookIds;
};

struct Solution
{
        std::vector<Subscription> subscriptions;
};

bool byScore(const Book &book1, const Book &book2)
{
        return book1.score - book2.score;
}

bool byOutput(const Library &library1, const Library &library2)
{
        return library1.output - library2.output < 0;
}

bool byPotential(const Library &library1, const Library &library2)
{
        const auto score1 = (double)library1.output / library1.totalPoints;
        const auto score2 = (double)library2.output / library2.totalPoints;
        return score1 - score2 < 0;
}

Solution solveNaive(const std::vector<Library> &libraries, unsigned dayCount)
{
        Solution solution;
        std::bitset<100000> alreadySent;
        std::vector<Library> sortedLibraries = libraries;

        std::sort(sortedLibraries.begin(), sortedLibraries.end(), byPotential);

        for (auto library : sortedLibraries)
        {
                std::sort(library.books.begin(), library.books.end(), byScore);
        }

        auto currentLibrary = 0;
        int remainingDays = sortedLibraries[currentLibrary].signUpTime;
        for (auto day = 0; day < dayCount; day++)
        {
                if (remainingDays == 0)
                {
                        Subscription newSubscription;
                        newSubscription.libraryId = sortedLibraries[currentLibrary].id;
                        solution.subscriptions.push_back(newSubscription);

                        currentLibrary++;
                        remainingDays = sortedLibraries[currentLibrary].signUpTime;
                }

                remainingDays--;

                for (auto &subscription : solution.subscriptions)
                {
                        const auto &library = libraries[subscription.libraryId];
                        for (int out = 0; out < library.output; out++)
                        {
                                for (auto book : library.books)
                                {
                                        if (!alreadySent[book.id])
                                        {
                                                subscription.bookIds.push_back(book.id);
                                        }
                                }
                        }
                }
        }

        return solution;
}
