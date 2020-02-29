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

struct BookWeights
{
    double scoreWeight;
    double idfWeight;
};

struct LibraryWeights
{
    double scorePotentialWeight;
    double signUpTimeWeight;
    double throughputWeight;
    double rareBooksWeight;
    double bookCountWeight;
    double rarenessThreshold;

    BookWeights bookWeights;
};

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
    double bookCount;

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

void computeLibraryStatistics(std::vector<LibraryStatistics> &statistics, const Problem &problem, std::vector<BookStatistics> &bookStatistics, double rarenessThreshold = 0.999)
{
    Statistics<double> potentialScoreStats;
    Statistics<int> signUpStats;
    Statistics<int> rareBooksStats;
    Statistics<int> throughputStats;
    Statistics<int> bookCountStats;

    int index = 0;
    for (const auto &library : problem.libraries)
    {
        statistics[index].libraryId = library.id;
        for (const auto &book : library.books)
        {
            statistics[index].rawBookScoreTotal += bookStatistics[book].aggregatedScore;
            if (bookStatistics[book].idf > rarenessThreshold)
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
        bookCountStats.count(problem.libraries[index].bookCount);
        potentialScoreStats.count(statistics[index].rawScorePotential);
        rareBooksStats.count(statistics[index].rawRareBookCount);
        throughputStats.count(problem.libraries[index].throughput);

        index++;
    }

    index = 0;
    for (const auto &library : problem.libraries)
    {
        statistics[index].scorePotential = potentialScoreStats.regularize(statistics[index].rawScorePotential);
        statistics[index].bookCount = bookCountStats.regularize(problem.libraries[index].bookCount);
        statistics[index].signUpTime = 1 - signUpStats.regularize(problem.libraries[index].signUpTime);
        statistics[index].throughput = throughputStats.regularize(problem.libraries[index].throughput);
        statistics[index].rareBooks = rareBooksStats.regularize(statistics[index].rawRareBookCount);

        index++;
    }
}

void computeLibraryScores(std::vector<LibraryStatistics> &statistics, double scorePotentialWeight = 2, double signUpTimeWeight = 3, double rareBooksWeight = 0, double throughputWeight = 0, double bookCountWeight = 0)
{
    int index = 0;
    for (auto &statistic : statistics)
    {
        double composants[] = {
            scorePotentialWeight * statistic.scorePotential,
            signUpTimeWeight * statistic.signUpTime,
            rareBooksWeight * statistic.rareBooks,
            throughputWeight * statistic.throughput,
            bookCountWeight * statistic.bookCount};
        statistic.aggregatedScore = vectorNorm(
            composants, composants + 5);
        /*
        cerr << "library=" << statistic.libraryId
             << ";rawScorePotential=" << statistic.rawScorePotential
             << ";signUpTime=" << statistic.signUpTime
             << ";throughput=" << statistic.throughput
             << ";bookCount=" << statistic.bookCount
             << ";rareBooksCount=" << statistic.rawRareBookCount
             << ";rareBooks=" << statistic.rareBooks
             << ";aggregatedScore=" << statistic.aggregatedScore << endl;
             */
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
        /*
        cerr << "book=" << index
             << ";rawScore=" << statistic.rawScore
             << ";regularizedScore=" << statistic.score
             << ";idf=" << statistic.rawIdf
             << ";regularizedIdf=" << statistic.idf
             << ";regularizedScore=" << statistic.aggregatedScore << endl;*/
        index++;
    }
}

void prepareLibrary(Library &library, const std::vector<BookStatistics> &bookStatistics)
{
    std::sort(
        library.books.begin(),
        library.books.end(),
        [&bookStatistics](auto book_id1, auto book_id2) {
            return bookStatistics[book_id1].aggregatedScore > bookStatistics[book_id2].aggregatedScore;
        });
}

Problem prepareProblem(const Problem &input, const std::vector<BookStatistics> &bookStatistics, std::vector<LibraryStatistics> &libraryStatistics, const unsigned int offset = 0)
{
    Problem preparedProblem = input;

    std::sort(
        preparedProblem.libraries.begin() + offset,
        preparedProblem.libraries.end(),
        [&libraryStatistics](const auto &library1, const auto &library2) {
            return libraryStatistics[library1.id].aggregatedScore > libraryStatistics[library2.id].aggregatedScore;
        });

    for (auto &library : preparedProblem.libraries)
    {
        prepareLibrary(library, bookStatistics);
    }

    return preparedProblem;
}

bool isEmptyLibrary(const Problem &problem, unsigned int library, const std::bitset<1000000> &scanned)
{
    if (library >= problem.libraries.size())
    {
        return false;
    }
    for (const auto book : problem.libraries[library].books)
    {
        if (!scanned[book])
            return false;
    }
    return true;
}

Solution build(const Problem &problem, std::bitset<30000> &ignoredLibraries)
{
    std::bitset<1000000> scanned;

    Solution solution;
    solution.subscriptions.reserve(problem.libraryCount);
    unsigned int score = 0;

    int lastActiveLibrary = 0;
    const Library &firstLibrary = problem.libraries[lastActiveLibrary];
    unsigned int nextSignUpCountDown = firstLibrary.signUpTime;

    vector<tuple<unsigned int, Subscription, unsigned int>> ongoingSubscriptions;

    for (int day = 0; day < problem.dayCount; day++)
    {
        for (auto &ongoingSubscription : ongoingSubscriptions)
        {
            const auto lib = get<0>(ongoingSubscription);
            auto &subscription = get<1>(ongoingSubscription);
            auto &bs = get<2>(ongoingSubscription);

            const auto &library = problem.libraries[lib];
            const auto &throughput = library.throughput;

            //int bs = 0;
            int count = 0;
            for (; bs < library.books.size() && count < throughput; bs++)
            {
                const auto bookId = library.books[bs];
                if (!scanned[bookId])
                {
                    score += problem.bookScores[bookId];
                    subscription.bookIds.push_back(bookId);
                    //solution.subscriptions[];
                    scanned[bookId] = true;
                    count++;
                }
            }
        }

        if (nextSignUpCountDown == day)
        {
            // cerr << "score: adding " << problem.libraries[lastActiveLibrary].id << endl;
            Subscription subscription{problem.libraries[lastActiveLibrary].id};
            ongoingSubscriptions.push_back(make_tuple(lastActiveLibrary, subscription, 0));

            while (++lastActiveLibrary < problem.libraries.size())
            {
                const bool isIgnored = ignoredLibraries[problem.libraries[lastActiveLibrary].id];
                const bool hasBooks = !isEmptyLibrary(problem, lastActiveLibrary, scanned);
                if (hasBooks && !isIgnored)
                {
                    nextSignUpCountDown = day + problem.libraries[lastActiveLibrary].signUpTime;
                    break;
                }
            }
        }
    }

    solution.score = score;
    for (const auto &ongoingSubscription : ongoingSubscriptions)
    {
        solution.subscriptions.push_back(get<1>(ongoingSubscription));
    }

    cerr << "[ON BUILD] score: " << score << endl;

    return solution;
}

unsigned int flagFirstEmptySubscription(const Solution &solution, std::bitset<30000> &ignoredLibraries)
{
    unsigned int index = 0;
    for (const auto &subscription : solution.subscriptions)
    {
        if (subscription.bookIds.size() == 0 && !ignoredLibraries[subscription.libraryId])
        {
            cerr << "Empty library: " << index << endl;
            ignoredLibraries[subscription.libraryId] = true;
            return index;
        }
        index++;
    }
    return -1;
}

Solution solve(const Problem &input, const LibraryWeights &libraryWeights)
{
    std::bitset<30000> ignoredLibraries;
    vector<BookStatistics> bookStatistics(input.bookCount);
    vector<LibraryStatistics> libraryStatistics(input.libraryCount);

    computeBookStatistics(bookStatistics, input);
    computeBookScores(bookStatistics, libraryWeights.bookWeights.scoreWeight, libraryWeights.bookWeights.idfWeight);
    computeLibraryStatistics(libraryStatistics, input, bookStatistics, libraryWeights.rarenessThreshold);
    computeLibraryScores(libraryStatistics, libraryWeights.scorePotentialWeight, libraryWeights.signUpTimeWeight, libraryWeights.rareBooksWeight, libraryWeights.throughputWeight, libraryWeights.bookCountWeight);
    Problem preparedProblem = prepareProblem(input, bookStatistics, libraryStatistics);
    unsigned int empty = 0;
    Solution solution;
    do
    {
        solution = build(preparedProblem, ignoredLibraries);
        empty = flagFirstEmptySubscription(solution, ignoredLibraries);
    } while (empty != -1);
    return solution;
}

Solver maxSolver([](const Problem &input, const Options &) {
    /*
    rarenessThreshold = 0.999
    bookCount = 0
    */

    /*
    BEST FOR
    - B (5813900)
    - F (5247056):
    throughput=0.25 IDF=0 rareBooks=0 bookScore=1 scorePotential=0.5 signUpTime=1

    from cat perl/best_stats.txt|./perl/extract_stats.pl|grep '5247056'|sort
    */
    const LibraryWeights best1{0.5, 1, 0.25, 0, 0, 0.999, {1, 0}};
    const Solution s1 = solve(input, best1);

    /*
    BEST FOR
    - B (5813900)
    - C (5505208)
    - E (5031367)
    throughput=0 IDF=0 rareBooks=0.5 bookScore=1 scorePotential=0.5 signUpTime=1

    from cat perl/best_stats.txt|./perl/extract_stats.pl|grep '5505208'|grep '^3'|sort
    */
    const LibraryWeights best2{0.5, 1, 0, 0.5, 0, 0.999, {1, 0}};
    const Solution s2 = solve(input, best2);

    return (s1.score > s2.score) ? s1 : s2;
});
