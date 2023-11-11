/**
 * File: boggle.cpp
 * ----------------
 * Implements the game of Boggle.
 */

#include <cctype>
#include <cmath>
#include <iostream>
#include <random>
#include <iterator>
#include <ctype.h>
#include "console.h"
#include "gboggle.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "random.h"
#include "grid.h"
#include "map.h"
#include "set.h"
#include "lexicon.h"
#include <typeinfo>
using namespace std;

static const string kStandardCubes[16] = {
   "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
   "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
   "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
   "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

static const string kBigBoggleCubes[25] = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

static const int kMinLength = 4;
static const double kDelayBetweenHighlights = 100;
static const double kDelayAfterAllHighlights = 500;
static const string wordsPath = "res/dictionary.txt";

static void welcome();
static void giveInstructions();
static int getPreferredBoardSize();
static void playBoggle();
string getInputTopChars(int dimension);
string getRandomTopChars(int dimension);
void drawAllChars(string topChars, int dimension);
Grid<char> getCubes(const string& topChars, int dimension);
Map<string, Vector<Vector<int>>> getValidWords(const Lexicon& words, const int dimension, const string& topchars);
void findNextChar(
    const Grid<char>& cubes, Grid<int>& currentPosition, const Lexicon& words, const int dimension,
    Map<string, Vector<Vector<int>>>& validWords, Vector<Vector<int>>& currentPrefixIndex
);
void playGame(const Map<string, Vector<Vector<int>>>& validWords, Lexicon words);

/**
 * Function: main
 * --------------
 * Serves as the entry point to the entire program.
 */
int main() {
    GWindow gw(kBoggleWindowWidth, kBoggleWindowHeight);
    initGBoggle(gw);
    welcome();
    if (getYesOrNo("Do you need instructions?")) giveInstructions();
    do {
        playBoggle();
    } while (getYesOrNo("Would you like to play again?"));
    cout << "Thank you for playing!" << endl;
    shutdownGBoggle();
    return 0;
}

/**
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */
static void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl;
    cout << endl;
}

/**
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */
static void giveInstructions() {
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl;
    cout << endl;
    cout << "Hit return when you're ready...";
    getLine(); // ignore return value
}

/**
 * Function: getPreferredBoardSize
 * -------------------------------
 * Repeatedly prompts the user until he or she responds with one
 * of the two supported Boggle board dimensions.
 */

static int getPreferredBoardSize() {
    cout << "You can choose standard Boggle (4x4 grid) or Big Boggle (5x5 grid)." << endl;
    return getIntegerBetween("Which dimension would you prefer: 4 or 5?", 4, 5);
}

/**
 * Function: playBoggle
 * --------------------
 * Manages all details needed for the user to play one
 * or more games of Boggle.
 */
static void playBoggle() {
    int dimension = getPreferredBoardSize();
    drawBoard(dimension, dimension);
    cout << "This is where you'd play the game of Boggle." << endl;
    string topChars;
    if (getYesOrNo("Do you want to force the board configuration?")) {
        topChars = getInputTopChars(dimension);
    } else {
        topChars = getRandomTopChars(dimension);
    }
    drawAllChars(topChars, dimension);
    Lexicon words(wordsPath);
    Map<string, Vector<Vector<int>>> validWords = getValidWords(words, dimension, topChars);
    playGame(validWords, words);
}

/**
 * Function: getInputTopChars
 * --------------------
 * Prompt the user to input the valid characters for each cube.
 */
string getInputTopChars(int dimension) {
    int cubeNum = dimension * dimension;
    cout << "Enter a " << cubeNum << "-character string to identify which letters you want on the cubes." << endl;
    cout << "The first " << dimension << " characters form the top row, ";
    cout << "the next " << dimension << " characters form the second row, and so forth." << endl;
    while (true) {
        string chars = getLine("Enter a string: ");
        if (chars.length() != cubeNum) {
            cout << "Enter a string that's precisely " << cubeNum << " characters long." << endl;
            continue;
        }
        bool isValid = true;
        for (int i = 0; i < dimension; i++) {
            char c = chars[i];
            if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z')) {
                isValid = false;
                cout << "Enter a string with only alphabetic letters." << endl;
                break;
            }
        }
        if (isValid) return toUpperCase(chars);
    }
}

/**
 * Function: getRandomTopChars
 * --------------------
 * Randomly shake all the cube in the cubes, and randomly choose
 * the upside character.
 */
string getRandomTopChars(int dimension) {
    Vector<string> cubes;
    int cubeNum = dimension * dimension;
    for (int i = 0; i < cubeNum; i++) {
        cubes.add(kStandardCubes[i]);
    }

    Vector<string> randomCubes;
    while (!cubes.isEmpty()) {
        int i = randomInteger(0, cubes.size() - 1);
        randomCubes.add(cubes[i]);
        cubes.remove(i);
    }

    string topChars = "";
    for (string cube : randomCubes) {
        int i = randomInteger(0, 5);
        topChars += cube[i];
    }

    return topChars;
}

/**
 * Function: getCubes
 * --------------------
 * According to all the chars, generate in the grid.
 */
Grid<char> getCubes(const string& topChars, int dimension) {
    Grid<char> cubes(dimension, dimension);
    for (int r = 0; r < dimension; r++) {
        for (int c = 0; c < dimension; c++) {
            int index = r * dimension + c;
            cubes[r][c] = topChars[index];
        }
    }
    return cubes;
}

/**
 * Function: findNextChar
 * --------------------
 * Receive the start char and position, save all the possible char in the cube.
 */
void findNextChar(
    const Grid<char>& cubes, Grid<int>& currentPosition, const Lexicon& words, const int dimension,
    Map<string, Vector<Vector<int>>>& validWords, Vector<Vector<int>>& currentPrefixIndex) {
    // Find the current char and position
    string currentPrefix = "";
    int currentLen = currentPrefixIndex.size();
    for (Vector<int> index : currentPrefixIndex) {
        currentPrefix += cubes[index[0]][index[1]];
    }
    int currentRow = currentPrefixIndex[currentLen - 1][0];
    int currentCol = currentPrefixIndex[currentLen - 1][1];

    // Find the loop range
    int minRow = max(0, currentRow - 1);
    int maxRow = min(dimension - 1, currentRow + 1);
    int minCol = max(0, currentCol - 1);
    int maxCol = min(dimension - 1, currentCol + 1);

    // Loop all the around character
    for (int r = minRow; r <= maxRow; r++) {
        for (int c = minCol; c <= maxCol; c++) {
            // Check if the character has been used
            if (currentPosition[r][c] > 0) {
                continue;
            }
            // the character has not been used
            char nextChar = cubes[r][c];
            string nextPrefix = currentPrefix + nextChar;
            // Check if the prefix is in the lexicon
            if (words.containsPrefix(nextPrefix)) {
                Grid<int> nextPosition = currentPosition;
                nextPosition[r][c] = currentLen + 1;
                Vector<Vector<int>> nextPrefixIndex = currentPrefixIndex;
                nextPrefixIndex.add({r, c});
                // Check if this is a new valid word
                if (nextPrefix.length() >= 4 && words.contains(nextPrefix) && !validWords.containsKey(nextPrefix)) {
                    validWords[nextPrefix] = nextPrefixIndex;
                }
                findNextChar(cubes, nextPosition, words, dimension, validWords, nextPrefixIndex);
            }
        }
    }
}

/**
 * Function: getValidWords
 * --------------------
 * Receive the random or input string, return all the valid words and position.
 */
Map<string, Vector<Vector<int>>> getValidWords(const Lexicon& words, const int dimension, const string& topchars) {
    Grid<char> cubes = getCubes(topchars, dimension);
    Map<string, Vector<Vector<int>>> validWords;
    for (int r = 0; r < dimension; r++) {
        for (int c = 0; c < dimension; c++) {
            Grid<int> currentPosition(dimension, dimension);
            currentPosition[r][c] = 1;
            Vector<Vector<int>> CurrentPefix;
            CurrentPefix.add({r, c});
            findNextChar(cubes, currentPosition, words, dimension, validWords, CurrentPefix);
        }
    }
    return validWords;
}

/**
 * Function: drawAllChars
 * --------------------
 * Receive the valid topChars and draw it on the top of cube.
 */
void drawAllChars(string topChars, int dimension) {
    int n = topChars.length();
    for (int i = 0; i < n; i++) {
        int r = i / dimension;
        int c = i % dimension;
        labelCube(r, c, topChars[i]);
    }
}

/**
 * Function: playGame
 * --------------------
 * Prompt the user to input words and record them, print out the final result.
 */
void playGame(const Map<string, Vector<Vector<int>>>& validWords, Lexicon words) {
    Set<string> usedWords;
    int humanScores = 0;
    int computerScores = 0;

    // Human's turn
    while(true) {
        // Prompt user to input the word
        string word = toUpperCase(getLine("Enter a word: "));

        // Print enter to quit the loop
        if (word == "") break;

        // Check if the word is in the lexicon
        if (!words.contains(word)) {
            cout << "Sorry, that isn't even a word." << endl;
            continue;
        }

        // Check if the word's length is enough
        if (word.length() < 4) {
            cout << "Sorry, that isn't long enough to even be considered." << endl;
            continue;
        }

        // Check if the word is able to formed on the board
        if (!validWords.containsKey(word)) {
            cout << "That word can't be constructed with this board." << endl;
            continue;
        }

        // Check if the word has been guessed
        if (usedWords.contains(word)) {
            cout << "You've already guessed that word." << endl;
            continue;
        }

        // Congratuation! the word is valid, record it
        recordWordForPlayer(word, HUMAN);
        usedWords.add(word);
        humanScores += word.length() - 3;

        // Highlight the character and recover it
        for (Vector<int> index : validWords[word]) {
            highlightCube(index[0], index[1], true);
            pause(250);
        }
        for (Vector<int> index : validWords[word]) {
            highlightCube(index[0], index[1], false);
        }
    }

    // Computer's turn
    Vector<string> allWords = validWords.keys();
    for (string word : allWords) {
        if (!usedWords.contains(word)) {
            recordWordForPlayer(word, COMPUTER);
            computerScores += word.length() - 3;
        }
    }

    // print out the final scores
    cout << "Your score: " << humanScores << endl;
    cout << "Computer score: " << computerScores << endl;

    // Compare the scores and get the result
    if (humanScores > computerScores) {
        cout << "Whoa!!!  You actually beat the computer at its own game." << endl;
        cout << "Excellent Boggle skills, human!" << endl;
    } else {
        cout << "Shocker! The computer player prevailed!" << endl;
    }
}
