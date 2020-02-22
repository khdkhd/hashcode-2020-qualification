const fs = require('fs');
const cluster = require('cluster');

const charset = 'utf-8';
const eol = "\n";
const tab = " ";
const numberFormat = new Intl.NumberFormat('en-IN');

const inputs = [
    "a_example.txt",
    "b_read_on.txt",
    "c_incunabula.txt",
    "d_tough_choices.txt",
    "e_so_many_books.txt",
    "f_libraries_of_the_world.txt"
];

function chunkToNumbers(line) {
    return line.split(tab).map(Number)
}

function parse(filename) {
    const data = fs.readFileSync(filename, charset).split(eol);
    const [totalBooksCount, librariesCount, availableDays] = chunkToNumbers(data.shift());
    const booksScores = chunkToNumbers(data.shift());
    const libraries = [];
    for (let i = 0; i < data.length; i += 2) {
        const id = i / 2;
        const [booksCount, signUpDuration, booksShippedPerDay] = chunkToNumbers(data.shift());
        const bookIds = chunkToNumbers(data.shift()).sort((a, b) => booksScores[b] - booksScores[a]);
        const roughScore = bookIds.reduce((a, i) => a + booksScores[i], 0);
        libraries.push({
            id,
            booksCount,
            signUpDuration,
            booksShippedPerDay,
            bookIds,
            booksScores,
            availableDays,
            roughScore
        })
    }
    return {
        filename,
        totalBooksCount,
        booksScores,
        librariesCount,
        availableDays,
        libraries
    }
}

function chooseBook(bookIds, knownBooks) {
    do {
        const bookId = bookIds.pop();
        if (!knownBooks.includes(bookId)) {
            return bookId;
        }
    } while (bookIds.length >= 0);

    return -1;
}

function formulaEvaluator({ signUpDuration, booksShippedPerDay, roughScore }) {
    return 1 - signUpDuration / (roughScore * booksShippedPerDay );
}

function compareLibraries(a, b) {
    return formulaEvaluator(b) - formulaEvaluator(a);
}


function initializeSubSolution(library) {
    return {
        id: library.id,
        booksCount: 0,
        bookIds: []
    }
}

function initializeSolution(filename) {
    return { filename, subSolutions: [], score: 0 };
}

function solve({ totalBooksCount, booksScores, librariesCount, availableDays, libraries, filename }) {
    const solution = initializeSolution(filename);
    const knownBooks = [];

    dayIterator:
        for (let i = availableDays; i > 0; --i) {
            for (const library of libraries) {
                if (library.signUpDuration > 0) {
                    --library.signUpDuration;
                    continue dayIterator;
                }

                if (library.bookIds.length === 0) {
                    continue;
                }

                let subSolution = solution.subSolutions[library.id];
                if (!subSolution) {
                    subSolution = initializeSubSolution(library);
                    solution.subSolutions[library.id] = subSolution;
                }

                for (let book = library.booksShippedPerDay; book > 0; --book) {
                    const bookId = chooseBook(library.bookIds, knownBooks);
                    if (bookId > 0) {
                        subSolution.bookIds.push(bookId);
                        subSolution.booksCount = subSolution.bookIds.length;
                        solution.score += booksScores[bookId];
                        knownBooks.push(bookId);
                    }
                }
            }
        }
    solution.subSolutions = solution.subSolutions.filter(library => library && library.booksCount > 0);
    return solution;
}

function dump({ subSolutions, filename }) {
    let output = `${ subSolutions.length }\n`;
    for (const library of subSolutions) {
        output += `${ library.id } ${ library.booksCount }\n`;
        output += `${ library.bookIds.join(' ') }\n`
    }
    fs.writeFileSync(`${ filename }.out`, output);
}

function run() {
    let totalScore = 0;
    for (const input of inputs) {
        const instance = parse(input);
        instance.libraries.sort(compareLibraries);
        const solution = solve(instance);
        totalScore += solution.score;
        dump(solution);
        console.log(solution.filename.substring(0, 1), numberFormat.format(solution.score));
    }
    console.log('total score', numberFormat.format(totalScore));
}

let i = 0;
if(cluster.isMaster) {
    let totalScore = 0;
    const start = new Date().getTime();
    for (const input of inputs) {
        const worker = cluster.fork({ input });
        worker.on('message', (solution) => {
            console.log(solution.filename.substring(0, 1), numberFormat.format(solution.score));
            totalScore += solution.score;
            console.log('total score', numberFormat.format(totalScore));
            console.log('time elapsed', (new Date().getTime() - start) / 1000, 'seconds');
        });
    }
} else {
    const input = process.env.input;
    const instance = parse(input);
    instance.libraries.sort(compareLibraries);
    const solution = solve(instance);
    dump(solution);
    process.send(solution)
}
