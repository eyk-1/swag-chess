#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Board.h"
#include <unordered_map>
#include <string>

class ChessGUI {
private:
    static sf::Font loadFont() {
        sf::Font font;
        font.openFromFile("arial.ttf"); // Just try to load, handle errors elsewhere
        return font;
    }
    sf::RenderWindow window;
    Game game;
    sf::Font font;

    // Board display
    static const int BOARD_SIZE = 640;
    static const int SQUARE_SIZE = 80;
    static const int BOARD_OFFSET_X = 80;
    static const int BOARD_OFFSET_Y = 80;

    // Piece textures
    std::unordered_map<std::string, sf::Texture> pieceTextures;

    // Game state
    bool pieceSelected;
    int selectedRow, selectedCol;
    bool gameOver;
    bool vsAI;
    bool aiIsWhite;

    // Visual feedback
    sf::RectangleShape highlightSquare;
    std::vector<sf::Vector2i> validMoves;

    // UI Elements
    sf::Text statusText;
    sf::Text turnText;
    sf::RectangleShape newGameButton;
    sf::Text newGameText;
    sf::RectangleShape modeButton;
    sf::Text modeText;


public:
    ChessGUI(bool playVsAI = false, bool aiPlaysWhite = false);
    ~ChessGUI();

    bool initialize();
    void run();

private:
    bool loadPieceTextures();
    void handleEvents();
    void update();
    void render();

    // Board interaction
    sf::Vector2i getBoardPosition(sf::Vector2i mousePos);
    bool isValidBoardPosition(sf::Vector2i pos);
    void selectPiece(int row, int col);
    void movePiece(int toRow, int toCol);
    void clearSelection();

    // AI handling
    void handleAIMove();

    // Drawing functions
    void drawBoard();
    void drawPieces();
    void drawHighlights();
    void drawUI();

    // Utility functions
    std::string getPieceTextureKey(Piece* piece);
    sf::Color getLightSquareColor();
    sf::Color getDarkSquareColor();
    sf::Color getHighlightColor();
    sf::Color getValidMoveColor();

    // Game state
    void updateGameStatus();
    void resetGame();
    void toggleMode();
};