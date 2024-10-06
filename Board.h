//
// Created by Isabella Bonilla on 4/23/24.
//

#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H
#include <iostream>
#include "Tile.h"
#include <SFML/Graphics.hpp>
#include <vector>
using namespace std;

class Board {
private:
    int rows;
    int columns;
    int mines;
    int mineCounter;
    bool minesRevealed;
    bool paused;

    bool win;
    bool lose;

    vector<vector<Tile>> board;

public:
    Board(int numRows, int numColumns, int numMines);

    void initialize();
    void restart();

    void randomizeMines();
    void FindAdjacentMines();
    int getMineCounter();

    void revealTile(int row, int col);
    void flagTile(int row, int col);
    bool checkWin();
    bool checkLose();
    void revealAll();
    void pause();
    void flagAll();

    void drawBoard(sf::RenderWindow &gameWindow, float first_x, float first_y);
    void leftClick(int mouseX, int mouseY);
    void rightClick(int mouseX, int mouseY);

};

Board::Board(int numRows, int numColumns, int numMines) {
    this->rows = numRows;
    this->columns = numColumns;
    this->mines = numMines;
    this->board = vector<vector<Tile>>(numRows, vector<Tile>(numColumns));
    this->mineCounter = numMines;
    this->minesRevealed = false;
    this->paused = false;
    this->win = false;
    this->lose = false;
}

void Board::randomizeMines() {
    int minesLeft = mines;

    while(minesLeft > 0) {
        int randRow = rand() % rows;
        int randCol = rand() % columns;

        Tile &tile = board[randRow][randCol];

        if(!tile.hasMine()) {
            tile.putMine();
            minesLeft--;
        }
    }
}

void Board::initialize() {
    for(int x = 0; x < rows; x++) {
        for(int y = 0; y < columns; y++) {
            Tile &tile = board[x][y];
            tile.loadTileTextures();
        }
    }

    randomizeMines();
    FindAdjacentMines();
}

void Board::FindAdjacentMines() {
    vector<pair<int, int>> coordDirections = {{-1, -1}, {-1, 0}, {-1, 1},
                                               {0, -1}, {0, 1},
                                               {1, -1}, {1, 0}, {1, 1}};

    for(int x = 0; x < rows; x++) {
        for(int y = 0; y < columns; y++) {
            int adjMines = 0;

            for(auto pair : coordDirections) {
                int row = x + pair.first;
                int col = y + pair.second;

                if((row >= 0) && (row < rows) && (col >= 0) && (col < columns) && (board[row][col].hasMine())) {
                    adjMines++;
                }
            }

            board[x][y].setAdjacentMines(adjMines);
        }
    }
}

void Board::revealTile(int row, int col) {
    if ((row >= 0) && (row < rows) && (col >= 0) && (col < columns)) {
        Tile &tile = board[row][col];

        if (!tile.isRevealed()) {
            if (tile.hasMine()) {
                revealAll();
                lose = true;
            }
            else if (!tile.hasFlag()){
                tile.reveal();

                if (tile.getAdjacentMines() == 0) {
                    vector<pair<int, int>> adjCoords = {{-1, -1}, {-1, 0}, {-1, 1},
                                                        {0,  -1}, {0,  1},
                                                        {1,  -1}, {1,  0}, {1,  1}};

                    for (auto &pair: adjCoords) {
                        int newRow = row + pair.first;
                        int newCol = col + pair.second;

                        revealTile(newRow, newCol);
                    }
                }
            }
        }
    }
}

void Board::flagTile(int row, int col) {
    Tile &tile = board[row][col];

    if(!tile.hasFlag()) {
        tile.flag();
        mineCounter--;
    }
    else if(tile.hasFlag()) {
        tile.flag();
        mineCounter++;
    }
}

void Board::revealAll() {
    for(int x = 0; x < rows; x++) {
        for (int y = 0; y < columns; y++) {
            Tile &tile = board[x][y];
            if (tile.hasMine()) {
                tile.reveal();
            }
        }
    }
}

bool Board::checkWin() {
    for(int x = 0; x < rows; x++) {
        for(int y = 0; y < columns; y++) {
            Tile tile = board[x][y];

            // if all non-mine tiles have not been revealed
            if(!tile.hasMine() && !tile.isRevealed()) {
                return false;
            }
        }
    }
    return true;
}

bool Board::checkLose() {
    for(int x = 0; x < rows; x++) {
        for(int y = 0; y < columns; y++) {
            Tile tile = board[x][y];

            if(lose) {
                return true;
            }
        }
    }
    return false;
}

void Board::drawBoard(sf::RenderWindow &gameWindow, float first_x, float first_y) {
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < columns; y++) {
            float xCoord = first_x + (x * 32);
            float yCoord = first_y + (y * 32);

            Tile &tile = board[x][y];
            tile.drawTile(gameWindow, xCoord, yCoord);
        }
    }
}

void Board::leftClick(int mouseX, int mouseY) {
    int x = mouseX / 32;
    int y = mouseY / 32;

    if (x >= 0 && x < rows && y >= 0 && y < columns) {
        revealTile(x, y);
    }
}

void Board::rightClick(int mouseX, int mouseY) {
    int x = mouseX / 32;
    int y = mouseY / 32;

    if (x >= 0 && x < rows && y >= 0 && y < columns) {
        flagTile(x, y);
    }
}

int Board::getMineCounter() {
    return this->mineCounter;
}

void Board::restart() {
    for(int x = 0; x < rows; x++) {
        for(int y = 0; y < columns; y++) {
            Tile &tile = board[x][y];
            tile.reset();
        }
    }
    this->mineCounter = mines;
    this->win = false;
    this->lose = false;
    initialize();
}

void Board::pause() {
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < columns; y++) {
            Tile &tile = board[x][y];
            tile.pause();
        }
    }
}

void Board::flagAll() {
    for(int x = 0; x < rows; x++) {
        for (int y = 0; y < columns; y++) {
            Tile &tile = board[x][y];
            if (tile.hasMine()) {
                if(!tile.hasFlag())
                tile.flag();
            }
        }
    }
}

#endif //MINESWEEPER_BOARD_H
