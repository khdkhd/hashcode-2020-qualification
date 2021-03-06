// Best so far

#include <books.hpp>

#include <algorithm>
#include <bitset>
#include <numeric>
#include <bitset>
#include <cmath>
#include <iostream>
#include <set>

#include <algorithm>
#include <bitset>
#include <iostream>
#include <numeric>

using namespace std;

struct BookStatistics
{
    long double aggregatedScore;

    double score;
    double idf;

    double rawIdf;
    unsigned int rawScore;
    unsigned int libraryCount;
};

struct LibraryStatistics
{
    unsigned int libraryId;
    long double aggregatedScore;

    double scorePotential;
    double signUpTime; // (!) More is better (!)
    double throughput;
    double rareBooks;

    double rawBookScoreTotal;
    unsigned int rawRareBookCount;
    double rawAverageBookScore;
    double rawAverageScorePerDay;
    unsigned int rawMaxOperatingDays;
    double rawScorePotential;
};

template <typename T>
class Statistics
{
public:
    unsigned int counter;
    T sum;
    T min;
    T max;

    Statistics()
    {
        this->min = std::numeric_limits<T>::max();
        this->max = std::numeric_limits<T>::min();
        this->counter = 0;
        this->sum = 0;
    }

    void count(const T value)
    {
        this->counter++;
        this->sum += value;
        if (value < this->min)
        {
            this->min = value;
        }
        if (value > this->max)
        {
            this->max = value;
        }
    }

    double average()
    {
        return (double)sum / counter;
    }

    double regularize(const T value)
    {
        if (this->min == this->max)
        {
            return 0.5;
        }
        return ((double)value - this->min) / (this->max - this->min);
    }
};

template <typename Iter_T>
long double vectorNorm(Iter_T first, Iter_T last)
{
    return sqrt(inner_product(first, last, first, 0.0L));
}

void computeLibraryStatistics(std::vector<LibraryStatistics> &statistics, const Problem &problem, std::vector<BookStatistics> &bookStatistics)
{
    Statistics<double> potentialScoreStats;
    Statistics<int> signUpStats;
    Statistics<int> rareBooksStats;
    Statistics<int> throughputStats;

    int index = 0;
    for (const auto &library : problem.libraries)
    {
        statistics[index].libraryId = library.id;
        for (const auto &book : library.books)
        {
            statistics[index].rawBookScoreTotal += bookStatistics[book].aggregatedScore;
            if (bookStatistics[book].idf > 0.99)
            {
                statistics[index].rawRareBookCount++;
            }
        }
        statistics[index].rawAverageBookScore = statistics[index].rawBookScoreTotal / library.bookCount;
        statistics[index].rawAverageScorePerDay = library.throughput * statistics[index].rawAverageBookScore;
        statistics[index].rawMaxOperatingDays = library.bookCount / library.throughput;

        if (statistics[index].rawMaxOperatingDays > (problem.dayCount - library.signUpTime))
        {
            statistics[index].rawMaxOperatingDays = problem.dayCount - library.signUpTime;
        }
        statistics[index].rawScorePotential = statistics[index].rawAverageScorePerDay * statistics[index].rawMaxOperatingDays;

        signUpStats.count(problem.libraries[index].signUpTime);
        potentialScoreStats.count(statistics[index].rawScorePotential);
        rareBooksStats.count(statistics[index].rawRareBookCount);
        throughputStats.count(problem.libraries[index].throughput);

        index++;
    }

    index = 0;
    cerr << "-----------------" << endl;
    for (const auto &library : problem.libraries)
    {
        statistics[index].scorePotential = potentialScoreStats.regularize(statistics[index].rawScorePotential);
        statistics[index].signUpTime = 1 - signUpStats.regularize(problem.libraries[index].signUpTime);
        statistics[index].throughput = throughputStats.regularize(problem.libraries[index].throughput);
        statistics[index].rareBooks = rareBooksStats.regularize(statistics[index].rawRareBookCount);

        index++;
    }
}

void computeLibraryScores(std::vector<LibraryStatistics> &statistics, double scorePotentialWeight = 2, double signUpTimeWeight = 3, double rareBooksWeight = 0, double throughputWeight = 0)
{
    int index = 0;
    cerr << "-----------------" << endl;
    for (auto &statistic : statistics)
    {
        double composants[] = {
            scorePotentialWeight * statistic.scorePotential,
            signUpTimeWeight * statistic.signUpTime,
            rareBooksWeight * statistic.rareBooks,
            throughputWeight * statistic.throughput};
        statistic.aggregatedScore = vectorNorm(
            composants, composants + 4);

        cerr << "library=" << statistic.libraryId
             << ";rawScorePotential=" << statistic.rawScorePotential
             << ";signUpTime=" << statistic.signUpTime
             << ";throughput=" << statistic.throughput
             << ";rareBooksCount=" << statistic.rawRareBookCount
             << ";rareBooks=" << statistic.rareBooks
             << ";aggregatedScore=" << statistic.aggregatedScore << endl;
        index++;
    }
}

void computeBookStatistics(std::vector<BookStatistics> &statistics, const Problem &problem)
{
    Statistics<unsigned int> scoreStats;
    Statistics<double> idfStats;

    for (const auto &library : problem.libraries)
    {
        for (const auto &book : library.books)
        {
            statistics[book].libraryCount++;
            statistics[book].rawScore = problem.bookScores[book];
            statistics[book].rawIdf = (double)problem.libraryCount / statistics[book].libraryCount;
            statistics[book].score = problem.bookScores[book];
            scoreStats.count(statistics[book].score);
            idfStats.count(statistics[book].rawIdf);
        }
    }

    int index = 0;
    for (auto &statistic : statistics)
    {
        statistic.score = scoreStats.regularize(problem.bookScores[index]);
        if (statistics[index].libraryCount > 0)
        {
            statistic.idf = idfStats.regularize(statistics[index].rawIdf);
        }
        index++;
    }
}

void computeBookScores(std::vector<BookStatistics> &statistics, double scoreWeight = 1, double idfWeight = 0)
{
    int index = 0;
    for (auto &statistic : statistics)
    {
        double composants[] = {
            scoreWeight * statistic.score,
            idfWeight * statistic.idf};
        statistic.aggregatedScore = vectorNorm(
            composants, composants + 2);

        cerr << "book=" << index
             << ";rawScore=" << statistic.rawScore
             << ";regularizedScore=" << statistic.score
             << ";idf=" << statistic.rawIdf
             << ";regularizedIdf=" << statistic.idf
             << ";regularizedScore=" << statistic.aggregatedScore << endl;
        index++;
    }
}

Problem prepareProblem2(const Problem &input, const std::vector<BookStatistics> &bookStatistics, std::vector<LibraryStatistics> &libraryStatistics, vector<unsigned int> &rankedLibraries)
{
    Problem preparedProblem = input;

    for (int i = 0; i < input.libraryCount; i++)
    {
        rankedLibraries[i] = i;
    }

    // Sort library ID
    std::cerr << "Sort libraries" << std::endl;
    std::sort(
        rankedLibraries.begin(),
        rankedLibraries.end(),
        [&libraryStatistics](const auto library1, const auto library2) {
            return libraryStatistics[library1].aggregatedScore > libraryStatistics[library2].aggregatedScore;
        });
    std::cerr << "Sort libraries - done" << std::endl;

    // Sort libraries books by score
    std::cerr << "Sort libraries books" << std::endl;
    for (auto &library : preparedProblem.libraries)
    {
        std::sort(
            library.books.begin(),
            library.books.end(),
            [&bookStatistics](auto book_id1, auto book_id2) {
                return bookStatistics[book_id1].aggregatedScore > bookStatistics[book_id2].aggregatedScore;
            });
    }
    std::cerr << "Sort libraries books - done" << std::endl;

    return preparedProblem;
}

Problem prepareProblem(const Problem &input, const std::vector<BookStatistics> &bookStatistics, std::vector<LibraryStatistics> &libraryStatistics)
{
    Problem preparedProblem = input;

    // Sort libraries by signUpTime
    std::cerr << "Sort libraries" << std::endl;
    std::sort(
        preparedProblem.libraries.begin(),
        preparedProblem.libraries.end(),
        [&libraryStatistics](const auto &library1, const auto &library2) {
            return libraryStatistics[library1.id].aggregatedScore > libraryStatistics[library2.id].aggregatedScore;
        });
    std::cerr << "Sort libraries - done" << std::endl;

    // Sort libraries books by score
    std::cerr << "Sort libraries books" << std::endl;
    for (auto &library : preparedProblem.libraries)
    {
        std::sort(
            library.books.begin(),
            library.books.end(),
            [&bookStatistics](auto book_id1, auto book_id2) {
                return bookStatistics[book_id1].aggregatedScore > bookStatistics[book_id2].aggregatedScore;
            });
    }
    std::cerr << "Sort libraries books - done" << std::endl;

    return preparedProblem;
}

Solution buildSolution2(const Problem &problem, const bitset<30000> &ignoredLibraries, const vector<unsigned int> &rankedLibraries)
{
    std::bitset<1000000> scanned;

    cerr << "starting computation" << endl;

    Solution solution;

    unsigned int score = 0;

    int lastActiveLibrary = 0;
    const Library &firstLibrary = problem.libraries[lastActiveLibrary];
    unsigned int nextSignUpCountDown = firstLibrary.signUpTime;

    vector<int> currentBookByLibrary(problem.libraryCount);
    for (int day = 0; day < problem.dayCount; day++)
    {
        // cerr << "before sending books" << endl;
        for (auto &subscription : solution.subscriptions)
        {
            const auto lib = subscription.libraryId;
            // cerr << "sending books to " << lib << endl;
            const auto &library = problem.libraries[lib];
            const auto &books = library.books;
            const auto throughput = library.throughput;

            // cerr << "where were we?" << endl;
            int &bs = currentBookByLibrary[lib];
            // int bs = 0;
            int count = 0;
            // cerr << "here: " << bs << endl;
            // cerr << books.size() << endl;
            for (; bs < books.size() && count < throughput; bs++)
            {
                // cerr << bs << "/" << books.size() << endl;
                const auto bookId = books[bs];
                // cerr << "Book: " << bookId << endl;
                if (!scanned[bookId])
                {
                    // cerr << "Book: " << bookId << " sent" << endl;
                    score += problem.bookScores[bookId];
                    subscription.bookIds.push_back(bookId);
                    //solution.subscriptions[];
                    scanned[bookId] = true;
                    count++;
                    // cerr << "Book: " << bookId << " counted" << endl;
                }
            }
            // cerr << "ok with sending to: " << library.id << endl;
        }

        if (nextSignUpCountDown == day)
        {
            auto nextLibraryId = rankedLibraries[lastActiveLibrary];
            Subscription subscription{nextLibraryId};
            solution.subscriptions.push_back(subscription);
            while (lastActiveLibrary + 1 < problem.libraries.size())
            {
                lastActiveLibrary++;
                if (!ignoredLibraries[rankedLibraries[lastActiveLibrary]])
                {
                    nextSignUpCountDown = day + problem.libraries[rankedLibraries[lastActiveLibrary]].signUpTime;
                    break;
                }
            }
        }
    }

    cerr << "[ON BUILD] score: " << score << endl;

    return solution;
}

Solver max4Solver([](const Problem &input, const Options &) {
    for (const auto &library : input.libraries)
    {
        if (library.id == 3953)
        {
            cerr << "DEBUG LIBRARY INPUT " << library.id << ": ";
            for (const auto &book : library.books)
            {
                cerr << ";" << book;
            }
            cerr << endl;
        }
    }

    vector<BookStatistics> bookStatistics(input.bookCount);
    vector<LibraryStatistics> libraryStatistics(input.libraryCount);

    bitset<30000> ignoredLibraries;
    vector<unsigned int> rankedLibraries(input.libraryCount);

    computeBookStatistics(bookStatistics, input);
    computeBookScores(bookStatistics);
    computeLibraryStatistics(libraryStatistics, input, bookStatistics);
    computeLibraryScores(libraryStatistics);

    const Problem preparedProblem = prepareProblem2(input, bookStatistics, libraryStatistics, rankedLibraries);
    // return buildSolution2(preparedProblem, ignoredLibraries, rankedLibraries);
    bool hasEmptySubscription = true;
    Solution solution;
    while (hasEmptySubscription)
    {
        hasEmptySubscription = false;
        solution = buildSolution2(preparedProblem, ignoredLibraries, rankedLibraries);
        for (const auto &subscription : solution.subscriptions)
        {
            if (subscription.bookIds.empty())
            {
                cerr << "It has empty subscriptions" << endl;
                hasEmptySubscription = true;
                ignoredLibraries[subscription.libraryId] = true;
                break;
            }
        }
    }

    cerr << "No empty subscriptions" << endl;

    return solution;
});
