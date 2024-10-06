//
// Created by Isabella Bonilla on 4/23/24.
//

#ifndef MINESWEEPER_TEXTUREMANAGER_H
#define MINESWEEPER_TEXTUREMANAGER_H
#include <iostream>
#include <map>
#include <SFML/Graphics.hpp>
using namespace std;


class TextureManager {
private:
    static map<string, sf::Texture> textures;

public:
    static sf::Texture& getTexture(const string &name);
    static void loadTexture(const string &name, const string &filename);
};

map<string, sf::Texture> TextureManager::textures;

sf::Texture &TextureManager::getTexture(const string &name) {
    return textures[name];
}

void TextureManager::loadTexture(const string &name, const string &filename) {
    sf::Texture texture;

    if(!texture.loadFromFile(filename)) {
        cerr << "Texture failed to load." << endl;
    } else {
        textures[name] = texture;
    }

}


#endif //MINESWEEPER_TEXTUREMANAGER_H
