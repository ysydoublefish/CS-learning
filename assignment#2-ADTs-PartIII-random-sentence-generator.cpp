/**
 * File: random-sentence-generator.cpp
 * -----------------------------------
 * Presents a short program capable of reading in
 * context-free grammar files and generating arbitrary
 * sentences from them.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <random>
using namespace std;

#include "console.h"
#include "simpio.h"   // for getLine
#include "strlib.h"   // for toLowerCase, trim
#include "map.h"      // in order to store the definition

static const string kGrammarsDirectory = "res/grammars/";
static const string kGrammarFileExtension = ".g";

static string getNormalizedFilename(string filename);
static bool isValidGrammarFilename(string filename);
static string getFileName();
string getFirstNonterminal(const string& str);
int getRandomInt(int min, int max);
Map<string, Vector<string>> getDefinition(string fileName);
string getRandomExpansion(const Map<string, Vector<string>>& definition, const string nonterminal);
string generateSentence(string fileName);

int main() {
    while (true) {
        string filename = getFileName();
        if (filename.empty()) break;

        for (int i = 1; i <= 3; i++) {
            string sentence = generateSentence(filename);
            cout << i << ".) " << sentence << endl << endl;
        }

        cout << "Here's where you read in the \"" << filename << "\" grammar "
             << "and generate three random sentences." << endl << endl;
    }
    cout << "Thanks for playing!" << endl;

    return 0;
}


static string getNormalizedFilename(string filename) {
    string normalizedFileName = kGrammarsDirectory + filename;
    if (!endsWith(normalizedFileName, kGrammarFileExtension))
        normalizedFileName += kGrammarFileExtension;
    return normalizedFileName;
}

static bool isValidGrammarFilename(string filename) {
    string normalizedFileName = getNormalizedFilename(filename);
    ifstream infile(normalizedFileName.c_str());
    return !infile.fail();
}

static string getFileName() {
    while (true) {
        string filename = trim(getLine("Name of grammar file? [<return> to quit]: "));
        if (filename.empty() || isValidGrammarFilename(filename)) return filename;
        cout << "Failed to open the grammar file named \"" << filename << "\". Please try again...." << endl;
    }
}

// Get the random situation in the place
int getRandomInt(int min, int max) {
    random_device seed;
    ranlux48 engine(seed());
    uniform_int_distribution<> distrib(min, max);
    int random = distrib(engine);

    return random;
}


// Get the first non-terminal word which is with in brackets
string getFirstNonterminal(const string& str) {
    int startIndex = stringIndexOf(str, "<");
    int endIndex = stringIndexOf(str, ">");
    int length = endIndex - startIndex + 1;

    string substr = str.substr(startIndex, length);

    return substr;
}

// Get the map of the nonterminal and the cooperate expansion
Map<string, Vector<string>> getDefinition(string fileName) {
    ifstream input;
    input.open(fileName);
    string line;
    Map<string, Vector<string>> definition;

    while(getline(input,line)) {
        // Start of the definition
        if (startsWith(line, '<') && endsWith(line, '>')) {
            // The key of the definition
            string nonterminal = line;
            // The value of the definition
            Vector<string> expansion;

            // Get the number of possible expansions
            getline(input,line);
            int lineNum = stringToInteger(line);

            // Loop all the possible expansion
            for (int i = 0; i < lineNum; i++) {
                getline(input,line);
                expansion.add(line);
            }

            // Put the key and value to the map
            definition[nonterminal] = expansion;
        }
    }

    input.close();

    return definition;
}

string getRandomExpansion(const Map<string, Vector<string>>& definition, const string nonterminal) {
    Vector<string> expansions = definition[nonterminal];
    int maxIndex = expansions.size() - 1;
    int randomIndex = getRandomInt(0, maxIndex);
    string randomExpansion = expansions[randomIndex];

    return randomExpansion;
}

string generateSentence(string fileName) {
    // Get the whole file path
    string filePath = getNormalizedFilename(fileName);

    // Get all the definition
    Map<string, Vector<string>> definition = getDefinition(filePath);

    // Get the start expansion
    string sentence = getRandomExpansion(definition, "<start>");

    // Generate the random sentence
    while(stringIndexOf(sentence, "<") != -1) {
        string nonterminal = getFirstNonterminal(sentence);
        string expansion = getRandomExpansion(definition, nonterminal);
        string temp = stringReplace(sentence, nonterminal, expansion);
        sentence = temp;
    }

    return sentence;
}
