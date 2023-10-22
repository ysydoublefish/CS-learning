/**
 * File: life.cpp
 * --------------
 * Implements the Game of Life.
 */

#include <iostream>  // for cout
#include <fstream>   // for ifstream
#include <unistd.h>  // for Sleep(us)
#include <cmath>     // for min, max
using namespace std;

#include "console.h" // required of all files that contain the main function
#include "simpio.h"  // for getLine
#include "gevent.h" // for mouse event detection
#include "strlib.h"  // for toLowerCase

#include "life-constants.h"  // for kMaxAge
#include "life-graphics.h"   // for class LifeDisplay

// function prototype
static void welcome();
ifstream openFile(LifeDisplay& display);
void getStart(int& row, int& column, Grid<int>& matrix, ifstream& input);
void matrixToDisplay(int row, int column, Grid<int> matrix, LifeDisplay& display);
bool generateToNext(int row, int column, Grid<int>& currentMatrix, Grid<int>& previousMatrix);
int excecutionMode();
bool generationGap(int modeCode);


/**
 * Function: main
 * --------------
 * Provides the entry point of the entire program.
 */
int main() {
    LifeDisplay display;
    display.setTitle("Game of Life");

    welcome();

    int row;
    int column;
    Grid<int> currentMatrix;
    Grid<int> previousMatrix;

    bool isContinued;

    do {
        // get the start status of the simulatiom
        ifstream input = openFile(display);
        getStart(row, column, currentMatrix, input);

        // initianize the board
        display.setDimensions(row, column);

        // show the start paint
        matrixToDisplay(row, column, currentMatrix, display);
        display.repaint();

        // prompt the user to input the speed mode
        int modeCode = excecutionMode();

        // go to the next generation
        while(!generateToNext(row, column, currentMatrix, previousMatrix)) {
            if (!generationGap(modeCode)) {
                break;
            }
            matrixToDisplay(row, column, currentMatrix, display);
            display.repaint();
        }

        // ask the user weather to continue
        isContinued = getYesOrNo("Would you like to run another? ");
    }
    while(isContinued);

    sleep(2);

    return 0;
}


/**
 * Function: welcome
 * -----------------
 * Introduces the user to the Game of Life and its rules.
 */
static void welcome() {
    cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
    cout << "Cells live and die by the following rules:" << endl << endl;
    cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
    cout << "\tLocations with 2 neighbors remain stable" << endl;
    cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
    cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl << endl;
    cout << "In the animation, new cells are dark and fade to gray as they age." << endl << endl;
    getLine("Hit [enter] to continue....   ");
}


ifstream openFile(LifeDisplay& display) {
    string fileName = toLowerCase(getLine("Enter name of conoly file: "));
    string filePathPrefix = "res\\files\\";
    string filePath = filePathPrefix + fileName;
    ifstream input;
    input.open(filePath);
    while (!input.is_open()) {
        cout << "Unable to open the file named \"" << fileName << "\". Please select another file." << endl;
        fileName = toLowerCase(getLine("Enter name of conoly file: "));
        filePath = filePathPrefix + fileName;
        input.open(filePath);
    }
    display.setTitle(fileName);
    return input;
}


void getStart(int& row, int& column, Grid<int>& matrix, ifstream& input) {

    int rowIndex = -2;
    string line;

    while (getline(input, line)) {
        // skip the comment of the file
        if (startsWith(line, "#")) {
            continue;
        }

        // get the grid information
        if (rowIndex == -2) {                       // get the rows
            row = stringToInteger(line);
        } else if (rowIndex == -1) {                // get the columns
            column = stringToInteger(line);
            matrix.resize(row, column);
        } else {                                    // the grid part (0 or 1)
            for (int i = 0; i < column; i++) {
                if (line[i] == '-') {
                    matrix[rowIndex][i] = 0;
                } else {
                    matrix[rowIndex][i] = 1;
                }
            }
        }

        // increase the row index
        rowIndex ++;
    }

    // close the file
    input.close();
}


void matrixToDisplay(int row, int column, Grid<int> matrix, LifeDisplay& display) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            display.drawCellAt(i, j, matrix[i][j]);
        }
    }
}


bool generateToNext(int row, int column, Grid<int>& currentMatrix, Grid<int>& previousMatrix) {
    // initialize the bool and num
    bool isStable = false;
    int changedNum = 0;

    // reassign the martix (a deep, full copy)
    previousMatrix = currentMatrix;

    // the real generrate part (loop each cell)
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            // get the amont of neighbors of the loop cell
            int neighborNum = 0;
            int minRow = max(i - 1, 0);
            int maxRow = min(i + 1, row - 1);
            int minColumn = max(j - 1, 0);
            int maxColumn = min(j + 1, column - 1);
            for (int r = minRow; r <= maxRow; r++) {
                for (int c = minColumn; c <= maxColumn; c++) {
                    if (!(r == i && c == j) && previousMatrix[r][c] > 0) {
                        neighborNum++;
                    }
                }
            }

            // according to the rule, generate the new age of the cell
            if (neighborNum <= 1) {                                                // 0 or 1 neighbors
                currentMatrix[i][j] = 0;
            } else if (neighborNum == 2 && previousMatrix[i][j] > 0) {             // 2 neighbors
                currentMatrix[i][j] = min(previousMatrix[i][j] + 1, kMaxAge);
            } else if (neighborNum == 3) {                                         // 3 neighbors
                currentMatrix[i][j] = min(previousMatrix[i][j] + 1, kMaxAge);
            } else if (neighborNum >= 4 && previousMatrix[i][j] > 0) {             // 4 or more neighbors
                currentMatrix[i][j] = 0;
            }

            // confirm if the cell age is changed
            if (previousMatrix[i][j] != currentMatrix[i][j]) {
                changedNum++;
            }
        }
    }

    // compare the two grid and get the result
    if (changedNum == 0) {
        isStable = true;
    }

    return isStable;
}


int excecutionMode() {
    cout << "You can start your colony with random cells oe read from a prepared file." << endl;
    cout << "You choose how fast to run the simulation." << endl;
    cout << "\t1 = As fast as this chip can go!" << endl;
    cout << "\t2 = Not too fast, this is a school zone." << endl;
    cout << "\t3 = Nice and slow so I can watch everything that happens." << endl;
    cout << "\t4 = Require enter key be pressed before advancing to next generation." << endl;
    int modeCode = getInteger("Your choice: ");
    // prompt the user not to input integer out of the range
    while (!(modeCode >= 1 && modeCode <= 4)) {
        cout << "You can only choose 1 to 4." << endl;
        modeCode = getInteger("Your choice: ");
    }

    return modeCode;
}


bool generationGap(int modeCode) {
    if (modeCode == 1) {                // 1 = As fast as this chip can go!
        usleep(100000);
    } else if (modeCode == 2) {         // 2 = Not too fast, this is a school zone.
        usleep(1000000);
    } else if (modeCode == 3) {         // 3 = Nice and slow so I can watch everything that happens."
        usleep(2500000);
    } else if (modeCode == 4) {         // 4 = Require enter key be pressed before advancing to next generation.
        string actionCommands = getLine("Please return to advance [or type out \"quit\" to end]: ");
        if (actionCommands == "quit") {
            return false;
        }
    }

    return true;
}

