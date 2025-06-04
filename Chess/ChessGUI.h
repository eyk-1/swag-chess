#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Board.h"
#include "Piece.h"
#include <vector>
#include <string>
#include <map>

class ChessGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Game game;

    // Board display
    static const int BOARD_SIZE = 480;
    static const int SQUARE_SIZE = 60;
    static const int BOARD_OFFSET_X = 50;
    static const int BOARD_OFFSET_Y = 50;
    static const int SIDEBAR_WIDTH = 300;

    // Colors
    sf::Color lightSquareColor = sf::Color(240, 217, 181);
    sf::Color darkSquareColor = sf::Color(181, 136, 99);
    sf::Color highlightColor = sf::Color(255, 255, 0, 100);
    sf::Color moveHighlight = sf::Color(0, 255, 0, 100);
    sf::Color checkHighlight = sf::Color(255, 0, 0, 150);

    // Game state
    bool gameMode; // true for multiplayer, false for AI
    bool isAIWhite;
    bool whiteTurn;
    int selectedRow, selectedCol;
    bool pieceSelected;
    std::vector<std::pair<int, int>> validMoves;
    bool gameOver;
    std::string gameResult;

    // Move history display
    std::vector<std::string> moveHistory;
    int historyScrollOffset;

    // UI Elements
    sf::RectangleShape boardBackground;
    sf::RectangleShape sidebar;
    sf::Text gameStatusText;
    sf::Text turnIndicator;
    sf::Text moveHistoryTitle;
    sf::RectangleShape newGameButton;
    sf::Text newGameText;
    sf::RectangleShape exitButton;
    sf::Text exitText;

    // Piece textures
    std::map<std::string, sf::Texture> pieceTextures;
    std::map<std::string, sf::Sprite> pieceSprites;

public:
    ChessGUI(bool multiplayer);
    ~ChessGUI();

    bool initialize();
    void run();

private:
    void handleEvents();
    void update();
    void render();

    // Event handling
    void handleMouseClick(int x, int y);
    void handleBoardClick(int row, int col);
    void handleUIClick(int x, int y);

    // Drawing functions
    void drawBoard();
    void drawPieces();
    void drawHighlights();
    void drawSidebar();
    void drawMoveHistory();
    void drawGameStatus();
    void drawUI();

    // Utility functions
    std::pair<int, int> pixelToSquare(int x, int y);
    std::pair<int, int> squareToPixel(int row, int col);
    void updateValidMoves();
    void updateGameStatus();
    void addMoveToHistory(const std::string& move);
    void resetGame();
    void makeAIMove();
    void checkGameEnd();

    // Piece management
    bool loadPieceTextures();
    std::string getPieceFilename(char piece, bool isWhite);
    sf::Sprite& getPieceSprite(char piece, bool isWhite);
};