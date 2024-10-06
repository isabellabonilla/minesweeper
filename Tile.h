//
// Created by Isabella Bonilla on 4/15/24.
//

#ifndef MINESWEEPER_TILE_H
#define MINESWEEPER_TILE_H
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include "TextureManager.h"
using namespace std;

class Tile {
private:
    bool mine;
    bool flagged;
    bool revealed;
    int adjMines;
    bool playPaused;

    sf::Sprite tile;

public:
    Tile();

    bool hasMine() const;
    bool hasFlag() const;
    bool isRevealed() const;
    int getAdjacentMines() const;
    void setAdjacentMines(int amount);

    void flag();
    void reveal();
    void putMine();
    void reset();
    void pause();

    void loadTileTextures();
    void drawTile(sf::RenderWindow &gameWindow, float x, float y);
};

Tile::Tile() {
    this->mine = false;
    this->flagged = false;
    this->revealed = false;
    this->adjMines = 0;
    this->playPaused = false;

}

bool Tile::hasMine() const {
    return this->mine;
}

bool Tile::hasFlag() const {
    return this->flagged;
}

bool Tile::isRevealed() const {
    return this->revealed;
}

int Tile::getAdjacentMines() const {
    return this->adjMines;
}

void Tile::setAdjacentMines(int amount) {
    this->adjMines = amount;
}

void Tile::flag() {
    this->flagged = !this->flagged;
}

void Tile::reveal() {
    this->revealed = !this->revealed;
}

void Tile::putMine() {
    this->mine = true;
}

void Tile::loadTileTextures() {
    //load all tile textures
    TextureManager::loadTexture("hidden", "files/images/tile_hidden.png");
    TextureManager::loadTexture("revealed", "files/images/tile_revealed.png");
    TextureManager::loadTexture("mine", "files/images/mine.png");
    TextureManager::loadTexture("flagged", "files/images/flag.png");
    TextureManager::loadTexture("number1", "files/images/number_1.png");
    TextureManager::loadTexture("number2", "files/images/number_2.png");
    TextureManager::loadTexture("number3", "files/images/number_3.png");
    TextureManager::loadTexture("number4", "files/images/number_4.png");
    TextureManager::loadTexture("number5", "files/images/number_5.png");
    TextureManager::loadTexture("number6", "files/images/number_6.png");
    TextureManager::loadTexture("number7", "files/images/number_7.png");
    TextureManager::loadTexture("number8", "files/images/number_8.png");
}

void Tile::drawTile(sf::RenderWindow &gameWindow, float x, float y) {
    // by default, all tiles are hidden
    tile.setPosition(x, y);

    if(playPaused) {
        tile.setTexture(TextureManager::getTexture("revealed"));
        gameWindow.draw(tile);
        return;
    }

    if (isRevealed()) { // if revealed, revealed sprite is drawn
        tile.setTexture(TextureManager::getTexture("revealed"));
        gameWindow.draw(tile);

        if(hasMine()) { // if there is a mine, mine sprite is drawn on top of revealed sprite
            sf::Sprite mineSprite(TextureManager::getTexture("mine"));
            mineSprite.setPosition(x, y);
            gameWindow.draw(mineSprite);

        } else if (getAdjacentMines() >= 1) { // else if there is adjacent mines, draw number of adjacent mines sprite on top of revealed sprite
            string numAdjacent = "number" + to_string(adjMines);
            sf::Sprite adjSprite(TextureManager::getTexture(numAdjacent));
            adjSprite.setPosition(x, y);
            gameWindow.draw(adjSprite);
        }
    } else {
        tile.setTexture(TextureManager::getTexture("hidden"));
        gameWindow.draw(tile);

        if(hasFlag()) { // if flagged, flag sprite is drawn on top of hidden sprite
            sf::Sprite flagSprite(TextureManager::getTexture("flagged"));
            flagSprite.setPosition(x, y);
            gameWindow.draw(flagSprite);
        }
    }
}

void Tile::reset() {
    mine = false;
    flagged = false;
    revealed = false;
}

void Tile::pause() {
    this->playPaused = !playPaused;
}


#endif //MINESWEEPER_TILE_H
