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

// bool
// byPotential(const Library &library1, const Library &library2) {
//     const auto score1 = (double)library1.throughput / (double)library1.totalPoints;
//     const auto score2 = (double)library2.throughput / (double)library2.totalPoints;
//     return score1 > score2;
// }

struct BookStatistics
{
        long double aggregatedScore;

        double score;
        double idf;

        double rawIdf;
        unsigned int libraryCount;
};

struct LibraryStatistics
{
        long double aggregatedScore;

        double scorePotential;
        double signUpTime; // (!) More is better (!)

        double rawBookScoreTotal;
        unsigned int rawRareBookCount;
        double rawAverageBookScore;
        double rawAverageScorePerDay;
        unsigned int rawMaxOperatingDays;
        double rawScorePotential;
};

double regularize(double value, double min, double max)
{
        return (value - min) / (max - min);
}

double inverseRegularize(double value, double min, double max)
{
        return regularize(-value, -max, -min);
}

template <typename Iter_T>
long double vectorNorm(Iter_T first, Iter_T last)
{
        return sqrt(inner_product(first, last, first, 0.0L));
}

void computeLibraryStatistics(std::vector<LibraryStatistics> &statistics, const Problem &problem, std::vector<BookStatistics> &bookStatistics)
{
        double minScorePotential = std::numeric_limits<double>::max();
        double maxScorePotential = std::numeric_limits<double>::min();

        double minSignUpTime = problem.libraries[0].signUpTime;
        double maxSignUpTime = problem.libraries[0].signUpTime;

        int index = 0;
        for (const auto &library : problem.libraries)
        {
                for (const auto &book : library.books)
                {
                        statistics[index].rawBookScoreTotal += bookStatistics[book].aggregatedScore;
                        if (bookStatistics[book].idf > 0.9)
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

                if (problem.libraries[index].signUpTime > maxSignUpTime)
                {
                        maxSignUpTime = problem.libraries[index].signUpTime;
                }

                if (problem.libraries[index].signUpTime < minSignUpTime)
                {
                        minSignUpTime = problem.libraries[index].signUpTime;
                }

                if (statistics[index].rawScorePotential > maxScorePotential)
                {
                        maxScorePotential = statistics[index].rawScorePotential;
                }

                if (statistics[index].rawScorePotential < minScorePotential)
                {
                        minScorePotential = statistics[index].rawScorePotential;
                }
                index++;
        }

        index = 0;
        cerr << "-----------------" << endl;
        for (const auto &library : problem.libraries)
        {
                statistics[index].scorePotential = regularize(statistics[index].rawScorePotential, minScorePotential, maxScorePotential);
                statistics[index].signUpTime = inverseRegularize(problem.libraries[index].signUpTime, minSignUpTime, maxSignUpTime);

                cerr << index << ";Rare books = " << statistics[index].rawRareBookCount << "/" << library.bookCount << endl;
                cerr << index << ";Signup time score = " << statistics[index].signUpTime << endl;
                cerr << index << ";Score potential score = " << statistics[index].rawScorePotential << ";" << minScorePotential << endl;
                cerr << index << ";" << statistics[index].scorePotential << ";" << statistics[index].signUpTime << endl;
                index++;
        }
}

void computeLibraryScores(std::vector<LibraryStatistics> &statistics, double scorePotentialWeight = 2, double signUpTimeWeight = 1)
{
        int index = 0;
        cerr << "-----------------" << endl;
        for (auto &statistic : statistics)
        {
                double composants[] = {
                    scorePotentialWeight * statistic.scorePotential,
                    signUpTimeWeight * statistic.signUpTime};
                statistic.aggregatedScore = vectorNorm(
                    composants, composants + 2);
                cerr << "library_score" << index << ";" << statistic.aggregatedScore << endl;
        }
}

void computeBookStatistics(std::vector<BookStatistics> &statistics, const Problem &problem)
{
        unsigned int minScore = problem.bookScores[0];
        unsigned int maxScore = problem.bookScores[0];

        double minIdf = problem.libraryCount;
        double maxIdf = 0;
        for (const auto &library : problem.libraries)
        {
                for (const auto &book : library.books)
                {
                        statistics[book].libraryCount++;
                        statistics[book].rawIdf = (double)problem.libraryCount / statistics[book].libraryCount;
                        statistics[book].score = problem.bookScores[book];
                        if (statistics[book].score > maxScore)
                        {
                                maxScore = problem.bookScores[book];
                        }
                        if (statistics[book].score < minScore)
                        {
                                minScore = problem.bookScores[book];
                        }
                        if (statistics[book].rawIdf > maxIdf)
                        {
                                maxIdf = statistics[book].rawIdf;
                        }
                        if (statistics[book].rawIdf < minIdf)
                        {
                                minIdf = statistics[book].rawIdf;
                        }
                }
        }

        int index = 0;
        cerr << "-----------------" << endl;
        for (auto &statistic : statistics)
        {
                statistic.score = regularize(problem.bookScores[index], minScore, maxScore);
                statistic.idf = regularize(statistics[index].rawIdf, minIdf, maxIdf);
                cerr << "book;" << index << ";" << problem.bookScores[index] << ";score=" << statistic.score << ";" << statistic.rawIdf << ";" << statistic.idf << endl;
                index++;
        }
}

void computeBookScores(std::vector<BookStatistics> &statistics, double scoreWeight = 1, double idfWeight = 1)
{
        int index = 0;
        cerr << "-----------------" << endl;
        for (auto &statistic : statistics)
        {
                double composants[] = {
                    scoreWeight * statistic.score,
                    idfWeight * statistic.idf};
                statistic.aggregatedScore = vectorNorm(
                    composants, composants + 2);
                cerr << "book_score" << index << ";" << statistic.aggregatedScore << endl;
        }
}

Solver maxSolver([](const Problem &problem, const Options &) {
        std::vector<BookStatistics> bookStatistics(problem.bookCount);
        std::vector<LibraryStatistics> libraryStatistics(problem.libraryCount);
        Problem preparedProblem = problem;

        computeBookStatistics(bookStatistics, problem);
        computeBookScores(bookStatistics);
        computeLibraryStatistics(libraryStatistics, problem, bookStatistics);
        computeLibraryScores(libraryStatistics);

        std::cerr << "BOOKS: " << preparedProblem.bookCount << std::endl;

        Solution solution;

        std::bitset<1000000> bookState; // 0: taken, 1: available

        bookState.set();

        // Sort libraries by signUpTime
        std::cerr << "Sort libraries" << std::endl;
        std::sort(
            preparedProblem.libraries.begin(),
            preparedProblem.libraries.end(),
            [&libraryStatistics](const auto &library1, const auto &library2) {
                    return libraryStatistics[library1.id].aggregatedScore < libraryStatistics[library2.id].aggregatedScore;
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

        std::cerr << "Compute solution" << std::endl;
        unsigned int score = 0;
        for (const auto &library : preparedProblem.libraries)
        {
                auto sentBookCount = library.throughput * (preparedProblem.dayCount - library.signUpTime);

                Subscription subscription{library.id};

                if (bookState.any())
                {
                        for (auto it = library.books.begin(), last = library.books.end(); it < last && sentBookCount > 0; ++it)
                        {
                                const auto book_id = *it;
                                if (bookState[book_id])
                                {
                                        bookState[book_id] = false;
                                        score += problem.bookScores[book_id];
                                        subscription.bookIds.emplace_back(book_id);
                                }
                        }

                        if (subscription.bookIds.size() > 0)
                        {
                                solution.subscriptions.emplace_back(subscription);
                        }
                }
        }
        std::cerr << "Compute solution - done: " << score << std::endl;

        return solution;
});
