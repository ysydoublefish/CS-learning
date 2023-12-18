/**
 * File: maze-generator.cpp
 * ------------------------
 * Presents an adaptation of Kruskal's algorithm to generate mazes.
 */

#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>
using namespace std;

#include "console.h"
#include "simpio.h"
#include "maze-graphics.h"
#include <unistd.h>
#include "set.h"
#include "vector.h"
#include "queue.h"

void generateMaze(int dimension);
bool isNecessary(const wall& w, const Set<wall>& walls, const int dimension);
Vector<wall> createAllWalls(int dimension);
static int getMazeDimension(string prompt, int minDimension = 7, int maxDimension = 50);



int main() {
    while (true) {
        int dimension = getMazeDimension("What should the dimension of your maze be [0 to exit]? ");
        if (dimension == 0) break;
        generateMaze(dimension);
        cout << "This is where I'd animate the construction of a maze of dimension " << dimension << "." << endl;
    }

    return 0;
}

static int getMazeDimension(string prompt, int minDimension, int maxDimension) {
    while (true) {
        int response = getInteger(prompt);
        if (response == 0) return response;
        if (response >= minDimension && response <= maxDimension) return response;
        cout << "Please enter a number between "
             << minDimension << " and "
             << maxDimension << ", inclusive." << endl;
    }
}

void generateMaze(int dimension) {
    MazeGeneratorView maze;
    maze.setDimension(dimension);

    Vector<wall> walls = createAllWalls(dimension);

    maze.addAllWalls(walls);
    maze.repaint();

    Vector<int> wallIndexArray;
    for (int i = 0; i < walls.size(); i++) {
        wallIndexArray.add(i);
    }

    random_shuffle(wallIndexArray.begin(), wallIndexArray.end());

    int loopNum = 0;

    Vector<wall> shuffledWalls;

    for (int i = 0; i < walls.size(); i++) {
        shuffledWalls.add(walls[wallIndexArray[i]]);
    }

    Set<wall> retainedWalls;
    for (int i = 0; i < walls.size(); i++) {
        retainedWalls.add(shuffledWalls[i]);
    }

    for (wall w : shuffledWalls) {
        if (!isNecessary(w, retainedWalls, dimension)) {
            maze.removeWall(w);
            retainedWalls.remove(w);
            maze.repaint();
            loopNum++;
        }
        if (loopNum == dimension * dimension - 1) {
            break;
        }
    }

    // For look
    sleep(5);
}


Vector<wall> createAllWalls(int dimension) {
    Vector<cell> hCells;
    Vector<cell> vCells;
    Vector<wall> walls;

    // Loop cells by horizontal and vertical
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            cell hCell {i, j};
            cell Vcell {j, i};
            hCells.add(hCell);
            vCells.add(Vcell);
        }
    }

    // Build a Last cell
    cell lastC {0, 0};

    // Loop by horizontal
    for (cell currentC : hCells) {
        if (currentC.row == 0 && currentC.col == 0) continue;
        if (lastC.row == currentC.row) {
            wall w {lastC, currentC};
            walls.add(w);
        }
        lastC = currentC;
    }

    lastC.row = 0;
    lastC.col = 0;

    // Loop by vertical
    for (cell currentC : vCells) {
        if (currentC.row == 0 && currentC.col == 0) continue;
        if (lastC.col == currentC.col) {
            wall w {lastC, currentC};
            walls.add(w);
        }
        lastC = currentC;
    }

    return walls;
}


bool isNecessary(const wall& w, const Set<wall>& walls, const int dimension) {
    // Get the start and the end cell
    cell start = w.one;
    cell end = w.two;

    Queue<cell> cells;
    Set<cell> passedCells;
    cells.enqueue(start);

    while(!cells.isEmpty()) {
        // Get the top cell of the queue
        cell topC = cells.dequeue();
        // If top is the end cell, the wall is necessary
        if (topC.row == end.row && topC.col == end.col) {
            return true;
        }
        // Get all the around cell if is not blocked

        // Up
        if (topC.row - 1 >= 0) {
            cell aroundC {topC.row - 1, topC.col};
            wall tempW {aroundC, topC};
            if (!walls.contains(tempW) && !passedCells.contains(aroundC)) {
                cells.enqueue(aroundC);
                passedCells.add(aroundC);
            }
        }
        // Down
        if (topC.row + 1 < dimension) {
            cell aroundC {topC.row + 1, topC.col};
            wall tempW {topC, aroundC};
            if (!walls.contains(tempW) && !passedCells.contains(aroundC)) {
                cells.enqueue(aroundC);
                passedCells.add(aroundC);
            }
        }
        // Left
        if (topC.col - 1 >= 0) {
            cell aroundC {topC.row, topC.col - 1};
            wall tempW {aroundC, topC};
            if (!walls.contains(tempW) && !passedCells.contains(aroundC)) {
                cells.enqueue(aroundC);
                passedCells.add(aroundC);
            }
        }
        // Down
        if (topC.col + 1 < dimension) {
            cell aroundC {topC.row, topC.col + 1};
            wall tempW {topC, aroundC};
            if (!walls.contains(tempW) && !passedCells.contains(aroundC)) {
                cells.enqueue(aroundC);
                passedCells.add(aroundC);
            }
        }
    }

    // Cound not find a way from start to end
    return false;
}
