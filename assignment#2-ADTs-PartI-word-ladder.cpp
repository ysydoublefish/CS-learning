/**
 * File: word-ladder.cpp
 * ---------------------
 * Implements a program to find word ladders connecting pairs of words.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "lexicon.h"
#include "strlib.h"
#include "simpio.h"
#include "queue.h"
#include "vector.h"
#include "set.h"

static string getWord(const Lexicon& english, const string& prompt);
static void generateLadder(const Lexicon& english, const string& start, const string& end);
static void playWordLadder();
Set<string> getDiffWords(const Lexicon& english, const string& originWord);

static const string kEnglishLanguageDatafile = "res\\dictionary.txt";

int main() {
    cout << "Welcome to the CS106 word ladder application!" << endl << endl;
    playWordLadder();
    cout << "Thanks for playing!" << endl;
    Lexicon english(kEnglishLanguageDatafile);
    playWordLadder();

    return 0;
}

static string getWord(const Lexicon& english, const string& prompt) {
    while (true) {
        string response = trim(toLowerCase(getLine(prompt)));
        if (response.empty() || english.contains(response)) return response;
        cout << "Your response needs to be an Enxglish word, so please try again." << endl;
    }
}

static void generateLadder(const Lexicon& english, const string& start, const string& end) {
    cout << "Here's where you'll search for a word ladder connecting \"" << start << "\" to \"" << end << "\"." << endl;

    if (start == end) {
        cout << "Here is the word ladder: " << start << " -> " << end << endl;
        return;
    }

    Set<string> usedWords;
    Queue<Vector<string>> ladderQueue;

    // Create first word ladder(start) and put it in the queue
    Vector<string> startLadder {start};
    ladderQueue.enqueue(startLadder);

    // Loop while queue is not empty
    while (!ladderQueue.isEmpty()) {
        Vector<string> topLadder = ladderQueue.dequeue();
        // Check if the front word of the vector is the end word
        // If it is, break the loop
        if (topLadder[0] == end) {
            // Loop the top ladder and print it out
            cout << "Found ladder: ";
            for (int i = topLadder.size() - 1; i >= 0; i--) {
                cout << topLadder[i];
                if (i != 0) {
                    cout << " -> ";
                }
            }
            cout << endl;
            return;
        }

        // If not, add new word to the front of the vector
        Set<string> diffWords = getDiffWords(english, topLadder[0]);
        Set<string> retainedWords = diffWords - usedWords;

        // If the Difference set is empty, go to the next loop
        if (retainedWords.isEmpty()) {
            continue;
        }

        // If the Difference set is not empty, add new word to the top and enqueue it to the queue
        for (string word: retainedWords) {
            Vector<string> nextLadder = topLadder;
            nextLadder.insert(0, word);
            ladderQueue.enqueue(nextLadder);
        }

        // put the used words in the set, in case it was uesd in next loop
        usedWords += retainedWords;
    }

    cout << "No word ladder between \"" << start << "\" and \"" << end << "\" could be found." << endl;
}

static void playWordLadder() {
    Lexicon english(kEnglishLanguageDatafile);
    while (true) {
        string start = getWord(english, "Please enter the source word [return to quit]: ");
        if (start.empty()) break;
        string end = getWord(english, "Please enter the destination word [return to quit]: ");
        if (end.empty()) break;
        if (start.length() != end.length()) {
            cout << "The length of the two words is not same, please try again." << endl;
            continue;
        }
        generateLadder(english, start, end);
    }
}

// Generate word that changing one letter from the originWord
Set<string> getDiffWords(const Lexicon& english, const string& originWord) {
    // Declare a set to store all the diff words
    Set<string> diffWords;

    // Loop by the length of the origin word
    for (int i = 0; i < originWord.length(); i++) {
        // Copy a string to be replaced
        string substituteWord = originWord;
        // Loop all the charactor, replace all
        for (char j = 'a';  j <= 'z'; j++) {
            substituteWord[i] = j;
            // Check if the substituted word is in the english lexicon
            if (english.contains(substituteWord)) {
                diffWords.add(substituteWord);
            }
        }
    }

    // Remove the origin word itself
    diffWords.remove(originWord);

    return diffWords;
}
