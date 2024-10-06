#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include "Board.h"
using namespace std;
// Isabella Bonilla
// Project 3: Minesweeper

void setText(sf::Text &text, float x, float y){ sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                   textRect.top  + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void drawDigit(sf::RenderWindow &window, string digits, int xCoord, int yCoord, sf::Texture &digitsTex) {
    for(int i = 0; i < digits.length(); i++) {
        int digIndex = 0;

        if(digits[i] == '-') {
            digIndex = 10;
        } else {
            digIndex = digits[i] - '0';
        }

        sf::Sprite digit;
        digit.setTexture(digitsTex);
        sf::IntRect textureRect(digIndex * 21, 0, 21, 32);
        digit.setTextureRect(textureRect);
        digit.setPosition(xCoord, yCoord);

        window.draw(digit);

        xCoord += 21;
    }
}

void drawCounter(sf::RenderWindow &window, int minesLeft, int columns, int rows, sf::Texture &digitsTex) {
    int xCoord = 33;
    int yCoord = 32 * (rows + 0.5) + 16;

    string minesStr = to_string(abs(minesLeft));

    if(minesStr.length() == 2) {
        minesStr = "0" + minesStr;
    }
    if(minesStr.length() == 1) {
        minesStr = "00" + minesStr;
    }
    if (minesLeft < 0) {
        minesStr = "-" + minesStr;
        xCoord = 12;
    }

    drawDigit(window, minesStr, xCoord, yCoord, digitsTex);
}

void drawTimer(sf::RenderWindow &window, int columns, int rows, int minutes, int seconds, sf::Texture &digitsTex) {
    string strMinutes = to_string(minutes);
    string strSeconds = to_string(seconds);

    if(strMinutes.length() == 1) {
        strMinutes = "0" + strMinutes;
    }

    if(strSeconds.length() == 1) {
        strSeconds = '0' + strSeconds;
    }

    int xCoord1 = (columns * 32) - 97;
    int yCoord1 = 32 * (rows + 0.5) + 16;

    int xCoord2 = (columns * 32) - 54;
    int yCoord2 = 32 * (rows + 0.5) + 16;

    drawDigit(window, strMinutes, xCoord1, yCoord1, digitsTex);
    drawDigit(window, strSeconds, xCoord2, yCoord2, digitsTex);
}

void leaderboardWindow(int columns, int rows, int minutes, int seconds, string nameInput) {
    int width = columns * 16;
    int height = (rows * 16) + 50;
    sf::RenderWindow window(sf::VideoMode(width, height), "Leaderboard Window", sf::Style::Close);

    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        throw runtime_error("Font failed to load.");
    }

    // drawing leaderboard message
    sf::Text leaderboard;
    leaderboard.setFont(font);
    leaderboard.setString("LEADERBOARD");
    leaderboard.setCharacterSize(20); // in pixels, not points!
    leaderboard.setFillColor(sf::Color::White);
    leaderboard.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(leaderboard, ((float) width) / 2, (((float) height) / 2) - 120);

    // drawing stats
    sf::Text content;
    content.setFont(font);
    content.setCharacterSize(18);
    content.setFillColor(sf::Color::White);
    content.setStyle(sf::Text::Bold);

    // reading leaderboard.txt for stats and adding to vector of pairs
    ifstream file("files/leaderboard.txt");
    vector<pair<string, string>> stats;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string timeStr, playerName;
        if (getline(ss, timeStr, ',') && getline(ss, playerName)) {
            stats.emplace_back(timeStr, playerName);
        }
    }

    // if the user won, format time and name properly
    if (nameInput != "null") {
        string strMin = to_string(minutes);
        string strSec = to_string(seconds);
        if(strMin.length() == 1) {
            strMin = '0' + strMin;
        }
        if(strSec.length() == 1) {
            strSec = '0' + strSec;
        }

        string timeStr = strMin + ":" + strSec;
        pair<string, string> inputStats;
        inputStats.first = timeStr;
        inputStats.second = nameInput + "*";

        // properly place user stat in proper position based on time
        for (auto it = stats.begin(); it != stats.end(); ++it) {
            if (timeStr < it->first) {
                stats.insert(it, inputStats);
                break;
            }
        }

        // write new user data to leaderboard.txt
        ofstream write("files/leaderboard.txt");
        for (auto& stat : stats) {
            write << stat.first << "," << stat.second << endl;
        }
    }

    string statistics;
    for (int i = 0; i < 5; i++) { // only print top 5 to the leaderboard
        statistics += to_string(i + 1) + ".\t" + stats[i].first + "\t" + stats[i].second + "\n\n";
    }

    content.setString(statistics);
    setText(content, ((float) width) / 2, (((float) height) / 2) + 20);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // user manually closes welcome window
            if (event.type == sf::Event::Closed) {
                window.close();
            }

        }

        window.clear(sf::Color::Blue);
        window.draw(leaderboard);
        window.draw(content);
        window.display();
    }

}

void gameWindow(int columns, int rows, int mines, string nameInput) {
    int width = columns * 32; // tile size is 32
    int height = (rows * 32) + 100;
    int minesLeft = mines;
    sf::RenderWindow window(sf::VideoMode(width, height), "Game Window");

    Board board(columns, rows, mines);
    board.initialize();

    // loading font
    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        throw runtime_error("Font failed to load.");
    }

    // loading all button textures
    sf::Texture happyFaceTex, winFaceTex, loseFaceTex, debugTex, playTex, pauseTex, digitsTex, leaderboardTex;

    happyFaceTex.loadFromFile("files/images/face_happy.png");
    winFaceTex.loadFromFile("files/images/face_win.png");
    loseFaceTex.loadFromFile("files/images/face_lose.png");
    debugTex.loadFromFile("files/images/debug.png");
    playTex.loadFromFile("files/images/play.png");
    pauseTex.loadFromFile("files/images/pause.png");
    digitsTex.loadFromFile("files/images/digits.png");
    leaderboardTex.loadFromFile("files/images/leaderboard.png");

    sf::Sprite face, debug, playpause, leaderboard;
    face.setTexture(happyFaceTex);
    debug.setTexture(debugTex);
    playpause.setTexture(pauseTex);
    leaderboard.setTexture(leaderboardTex);

    face.setPosition(((columns / 2.0) * 32) - 32, 32 * (rows + 0.5));
    debug.setPosition((columns * 32) - 304, 32 * (rows + 0.5));
    playpause.setPosition((columns * 32) - 240, 32 * (rows + 0.5));
    leaderboard.setPosition((columns * 32) - 176, 32 * (rows + 0.5));

    auto startTime = std::chrono::high_resolution_clock::now();
    int minutes;
    int seconds;
    bool playing = true;
    bool gameOver = false;
    bool openLeaderboard = false;
    bool updatedLeaderboard = false;

    while(window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i position = sf::Mouse::getPosition(window);

                    // if user left-clicks on tile sprite
                    int mouseX = position.x;
                    int mouseY = position.y;
                    board.leftClick(mouseX, mouseY);

                    auto convertedPosition = window.mapPixelToCoords(position);

                    if(gameOver == false) {
                        if (board.checkWin()) { // functionality after winning
                            face.setTexture(winFaceTex);
                            board.flagAll();
                            playing = false;
                            gameOver = true;
                            updatedLeaderboard = false;
                            openLeaderboard = true;

                        } else if (board.checkLose()) { // functionality after losing
                            face.setTexture(loseFaceTex);
                            playing = false;
                            gameOver = true;
                        }
                    }

                    // if user left-clicks on face sprite (restarts board)
                    if (face.getGlobalBounds().contains(convertedPosition)) {
                        if (gameOver == true) {
                            playing = true;
                        }
                        gameOver = false;
                        playing = true;
                        board.restart();
                        face.setTexture(happyFaceTex);
                        minutes = 0;
                        seconds = 0;
                        startTime = std::chrono::high_resolution_clock::now();
                        minesLeft = mines;
                    }

                    // if user left-clicks on debug sprite (reveals mines)
                    if (debug.getGlobalBounds().contains(convertedPosition)) {
                        if (playing) {
                            board.revealAll();
                        }
                    }

                    // if user left-clicks on playpause sprite
                    if (playpause.getGlobalBounds().contains(convertedPosition)) {
                        if (gameOver == false) {
                            if (playing) {
                                playing = false;
                                playpause.setTexture(playTex);
                                board.pause();

                                auto currTime = std::chrono::high_resolution_clock::now();
                                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
                                        currTime - startTime);

                                minutes = elapsedTime.count() / 60;
                                seconds = elapsedTime.count() % 60;

                            } else {
                                playing = true;
                                playpause.setTexture(pauseTex);
                                board.pause();
                                startTime = std::chrono::high_resolution_clock::now() -
                                            std::chrono::seconds(minutes * 60 + seconds);
                            }
                        }
                    }

                    //if user left-clicks on leaderboard sprite
                    if (leaderboard.getGlobalBounds().contains(convertedPosition)) {
                        openLeaderboard = true;
                        // had to move functionality below draw functions in order for board to properly pause
                        if(playing) {
                            board.pause();
                        }
                    }

                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    sf::Vector2i position = sf::Mouse::getPosition(window);

                    // if user right-clicks on tile sprite (flags)
                    int mouseX = position.x;
                    int mouseY = position.y;
                    board.rightClick(mouseX, mouseY);
                    minesLeft = board.getMineCounter();
                }
            }

            window.clear(sf::Color::White);

            float first_x = 0.0;
            float first_y = 0.0;

            if (playing && gameOver == false) {
                auto currTime = std::chrono::high_resolution_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currTime - startTime);

                minutes = elapsedTime.count() / 60;
                seconds = elapsedTime.count() % 60;
            }

            // drawing all UI elements
            board.drawBoard(window, first_x, first_y);
            drawTimer(window, columns, rows, minutes, seconds, digitsTex);
            window.draw(face);
            window.draw(debug);
            window.draw(playpause);
            window.draw(leaderboard);
            drawCounter(window, minesLeft, columns, rows, digitsTex);

            window.display();

            if(openLeaderboard) { // handling leaderboard button, and leaderboard reveal after winning
                if(gameOver) {
                    if(board.checkWin()) {
                        if(updatedLeaderboard == false) { // updating leaderboard with new user info
                            leaderboardWindow(columns, rows, minutes, seconds, nameInput);
                            updatedLeaderboard = true;
                        }
                        else if(updatedLeaderboard == true) {
                            leaderboardWindow(columns, rows, minutes, seconds, "null");
                        }
                    }
                    else if(board.checkLose()) {
                        leaderboardWindow(columns, rows, minutes, seconds, "null");
                    }
                }
                else if (playing) { // leaderboard button shares similar functionality to playpause
                    playing = false;
                    auto currTime = std::chrono::high_resolution_clock::now();
                    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currTime - startTime);

                    minutes = elapsedTime.count() / 60;
                    seconds = elapsedTime.count() % 60;

                    leaderboardWindow(columns, rows, minutes, seconds, "null");

                    playing = true;
                    board.pause();
                    startTime = std::chrono::high_resolution_clock::now() - std::chrono::seconds(minutes * 60 + seconds);
                }
                else { // if paused and then leaderboard button is pressed
                    leaderboardWindow(columns, rows, minutes, seconds, "null");
                }

                openLeaderboard = false;
            }
        }
    }
}

void welcomeWindow(int columns, int rows, int mines) {
    int width = columns * 32;
    int height = (rows * 32) + 100;
    sf::RenderWindow window(sf::VideoMode(width, height), "Welcome Window", sf::Style::Close);

    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        throw runtime_error("Font failed to load.");
    }

    // drawing welcome message
    sf::Text welcome;
    welcome.setFont(font);
    welcome.setString("WELCOME TO MINESWEEPER!");
    welcome.setCharacterSize(24); // in pixels, not points!
    welcome.setFillColor(sf::Color::White);
    welcome.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcome, ((float) width) / 2, (((float) height) / 2) - 150);

    // drawing enter name message
    sf::Text entername;
    entername.setFont(font);
    entername.setString("Enter your name:");
    entername.setCharacterSize(20);
    entername.setFillColor(sf::Color::White);
    entername.setStyle(sf::Text::Bold);
    setText(entername, ((float) width) / 2, (((float) height) / 2) - 75);

    // drawing name input
    sf::Text name;
    name.setFont(font);
    name.setString("|");
    name.setCharacterSize(18);
    name.setFillColor(sf::Color::Yellow);
    name.setStyle(sf::Text::Bold);
    setText(name, ((float) width) / 2, (((float) height) / 2) - 45);
    string nameinput;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // user manually closes welcome window
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // user enters characters for name, validates the name
            if (event.type == sf::Event::TextEntered) {
                if (isalpha(event.text.unicode)) { // checks if char is in the alphabet
                    char letter = event.text.unicode;

                    if (nameinput.size() < 10) { // name does not exceed 10 characters
                        nameinput += letter;
                        nameinput[0] = toupper(nameinput[0]); // upper case first letter

                        for (int i = 1; i < nameinput.size(); i++) { // lower case every other letter
                            nameinput[i] = tolower(nameinput[i]);
                        }
                    }
                }
                else if (event.text.unicode == '\b' && nameinput.size() > 0) { // implements backspace to remove last letter
                    nameinput.pop_back();
                }

                name.setString(nameinput + "|"); // adding cursor while typing name
                setText(name, ((float) width) / 2, (((float) height) / 2) - 45);
            }
            if (event.type == sf::Event::KeyPressed) {
                if(nameinput.size() >= 1) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                        window.close();
                        gameWindow(columns, rows, mines, nameinput);
                    }
                }
            }
        }

        window.clear(sf::Color::Blue);
        window.draw(welcome);
        window.draw(entername);
        window.draw(name);
        window.display();
    }
}

int main() {
    vector <int> dimensions;

    ifstream file;
    string read;
    file.open("files/config.cfg");

    if (!file) {
        throw runtime_error("File failed to open.");
    }

    while (getline(file, read)) {
        dimensions.push_back(stoi(read));
    }

    file.close();

    int columns = dimensions[0];
    int rows = dimensions[1];
    int mines = dimensions[2];

    welcomeWindow(columns, rows, mines);

    return 0;
}
