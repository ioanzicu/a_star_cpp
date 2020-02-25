// Maze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const int kEmpty = 0;
const int kObstacle = 1;
const int kClosed = 2;
const int kPath = 3;
const int kStart = 4;
const int kFinish = 5;

enum class State
{
    kEmpty,
    kObstacle,
    kClosed,
    kPath,
    kStart,
    kFinish
};

// directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> ParseLine(string line)
{
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c && c == ',')
    {
        if (n == 0)
        {
            row.push_back(State::kEmpty);
        }
        else
        {
            row.push_back(State::kObstacle);
        }
    }
    return row;
}

string CellString(int cell)
{
    switch (cell)
    {
    // kObstacle
    case 1:
        return "X  ";
    // kPath
    case 3:
        return "-   ";
    // kStart
    case 4:
        return "S   ";
    // kFinish
    case 5:
        return "F   ";
    default:
        return "0   ";
    }
}

void PrintBoard(const vector<vector<int>> board)
{
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            cout << CellString(board[i][j]);
        }
        cout << "\n";
    }
}

// Calculate the manhattan distance
int Heuristic(int x1, int y1, int x2, int y2)
{
    return abs(x2 - x1) + abs(y2 - y1);
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openList, vector<vector<int>> &grid)
{
    vector<int> node{x, y, g, h};
    openList.push_back(node);

    // State::kClosed
    grid[x][y] = kClosed;
}

// Compare the 'f' values of two cells.
bool Compare(const vector<int> node1, const vector<int> node2)
{
    int g1 = node1[2];
    int h1 = node1[3];
    int f1 = g1 + h1;

    int g2 = node2[2];
    int h2 = node2[3];
    int f2 = g2 + h2;

    return f1 > f2;
}

// Sort the two-dimensional vector ints in descending order
void CellSort(vector<vector<int>> *v)
{
    sort(v->begin(), v->end(), Compare);
}

// Check is the cell is valid - was not checked before
bool CheckValidCell(int x, int y, vector<vector<int>> &grid)
{
    // Validate if the x, y are on the grid
    bool x_on_grid = x >= 0 && x < grid.size();
    bool y_on_grid = y >= 0 && y < grid[0].size();

    if (x_on_grid && y_on_grid)
    {
        // Cast explicitly the scoped enum to int value
        return grid[x][y] == kEmpty;
    }
    return false;
}

void ExpandNeighbors(const vector<int> &current,
                     int goal[2],
                     vector<vector<int>> &openList,
                     vector<vector<int>> &grid)
{
    // Get the node's data
    int x = current[0];
    int y = current[1];
    int g = current[2];

    // Loop through current node's potential neighbors.
    for (int i = 0; i < 4; ++i)
    {
        int x2 = x + delta[i][0];
        int y2 = y + delta[i][1];

        // Check that the potential neighbor's x2 and y2 values are on the grid and not closed.
        if (CheckValidCell(x2, y2, grid))
        {
            // Increment g value and add neighbor to open list
            int g2 = g + 1;
            int h2 = Heuristic(x2, y2, goal[0], goal[1]);
            AddToOpen(x2, y2, g2, h2, openList, grid);
        }
    }
}

/*
 *  A* Algorithm 
 */
vector<vector<int>> Search(vector<vector<int>> grid, int start[2], int goal[2])
{
    // Create the vector of open nodes.
    vector<vector<int>> open{};

    int x = start[0];
    int y = start[1];
    int x2 = goal[0];
    int y2 = goal[1];

    // initial cost
    int g = 0;
    int h = Heuristic(x, y, x2, y2);
    AddToOpen(x, y, g, h, open, grid);

    while (open.size() > 0)
    {
        // Sort the vector
        CellSort(&open);
        // get the curent node
        auto current = open.back();
        // remove the node from the open list
        open.pop_back();
        // set kPath
        x = current[0];
        y = current[1];
        grid[x][y] = kPath;

        // Check if we are done
        if (x == x2 && y == y2)
        {
            // Set the init grid cell to kStart, and
            // set the goal grid cell to kFinish before returning the grid.
            grid[start[0]][start[1]] = kStart; // kStart
            grid[goal[0]][goal[1]] = kFinish;  // kFinish

            return grid;
        }

        // Explore new nodes
        ExpandNeighbors(current, goal, open, grid);
    }

    // We've run out of new nodes to explore and haven't found a path.
    cout << "No path found!" << endl;
    return vector<vector<int>>{};
}

int main()
{
    vector<vector<int>> board{
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0}};

    int start[2] = {0, 0};
    int goal[2] = {4, 5};
    auto solution = Search(board, start, goal);

    PrintBoard(solution);
}