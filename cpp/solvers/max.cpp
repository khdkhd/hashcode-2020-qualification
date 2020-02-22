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
        if (min == max)
        {
                return 0.5;
        }
        return (value - min) / (max - min);
}

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
        Statistics<double> scorePotentialStats;
        Statistics<unsigned int> signUpTimeStats;
        Statistics<unsigned int> throughputStats;

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
                statistics[index].signUpTime = 1 - regularize(problem.libraries[index].signUpTime, minSignUpTime, maxSignUpTime);
                /*
                cerr << index << ";Rare books = " << statistics[index].rawRareBookCount << "/" << library.bookCount << endl;
                cerr << index << ";Signup time score = " << statistics[index].signUpTime << endl;
                cerr << index << ";Score potential score = " << statistics[index].rawScorePotential << ";" << minScorePotential << endl;
                cerr << index << ";" << statistics[index].scorePotential << ";" << statistics[index].signUpTime << endl;
                */
                index++;
        }
}

void computeLibraryScores(std::vector<LibraryStatistics> &statistics, double scorePotentialWeight = 2, double signUpTimeWeight = 3)
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
                cerr << "library_score;" << index << ";" << statistic.aggregatedScore << endl;
                index++;
        }
}

void computeBookStatistics(std::vector<BookStatistics> &statistics, const Problem &problem)
{
        Statistics<unsigned int> scoreStats;
        Statistics<unsigned int> idfStats;

        for (const auto &library : problem.libraries)
        {
                for (const auto book : library.books)
                {
                        statistics[book].libraryCount++;
                        statistics[book].rawScore = problem.bookScores[book];
                        scoreStats.count(statistics[book].rawScore);
                }
        }

        int index = 0;
        for (auto &statistic : statistics)
        {
                if (statistics[index].libraryCount > 0)
                {
                        statistic.rawIdf = (double)problem.libraryCount / statistics[index].libraryCount;
                }
                else
                {
                        statistic.rawIdf = 0;
                }
                idfStats.count(statistics[index].rawIdf);
                index++;
        }

        index = 0;
        for (auto &statistic : statistics)
        {
                statistic.score = scoreStats.regularize(statistics[index].rawScore);
                statistic.idf = idfStats.regularize(statistics[index].rawIdf);
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
                     << ";score=" << statistic.score
                     << ";rawIdf=" << statistic.rawIdf
                     << ";idf=" << statistic.idf
                     << ";aggregatedScore=" << statistic.aggregatedScore << endl;

                index++;
        }
}

Solver maxSolver([](const Problem &input, const Options &) {
        Problem preparedProblem = input;

        std::vector<BookStatistics> bookStatistics(preparedProblem.bookCount);
        std::vector<LibraryStatistics> libraryStatistics(preparedProblem.libraryCount);

        computeBookStatistics(bookStatistics, input);
        for (const auto score : input.bookScores)
        {

                cerr << score << " ";
        }
        cerr << endl;
        computeBookScores(bookStatistics);
        computeLibraryStatistics(libraryStatistics, input, bookStatistics);
        computeLibraryScores(libraryStatistics);

        Solution solution;

        std::bitset<1000000> bookState; // 0: taken, 1: available

        bookState.set();

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
                                        subscription.bookIds.emplace_back(book_id);
                                }
                        }

                        if (subscription.bookIds.size() > 0)
                        {
                                solution.subscriptions.emplace_back(subscription);
                        }
                }
        }

        /*
        std::cerr << "Compute solution" << std::endl;
        unsigned int score = 0;
        unsigned int bookCount = 0;

        unsigned int currentLibrary = 0;
        unsigned int remainingDaysBeforeSignUp = preparedProblem.libraries[currentLibrary].signUpTime;

        for (unsigned int day = 0; day < preparedProblem.dayCount; day++)
        {
                for (auto i = 0u; i < solution.subscriptions.size(); i++)
                {
                        auto &subscription = solution.subscriptions[i];
                        const auto &library = preparedProblem.libraries[subscription.libraryId];

                        auto added = 0u;
                        for (auto j = 0u; j < library.bookCount && added < library.throughput; j++)
                        {
                                const auto currentBook = library.books[j];
                                if (bookState[library.books[j]])
                                {

                                        subscription.bookIds.emplace_back(library.books[j]);
                                        //cerr << "Book " << library.books[j] << " added by " << library.id << " on day " << day << endl;
                                        bookState[library.books[j]] = false;

                                        score += preparedProblem.bookScores[library.books[j]];
                                        bookCount++;
                                        added++;
                                }
                        }
                }

                if (remainingDaysBeforeSignUp == 0)
                {
                        Subscription subscription{preparedProblem.libraries[currentLibrary].id};
                        solution.subscriptions.emplace_back(subscription);
                        cerr << "Library " << preparedProblem.libraries[currentLibrary].id << " subscribed on day " << day << ". Output: " << preparedProblem.libraries[currentLibrary].throughput << endl;

                        currentLibrary++;
                        remainingDaysBeforeSignUp = preparedProblem.libraries[currentLibrary].signUpTime;
                }
                remainingDaysBeforeSignUp--;
        }
        std::cerr << "Compute solution - done" << std::endl;
        std::cerr << "SOLUTION_SCORE=" << score << endl;
*/
        return solution;
});
