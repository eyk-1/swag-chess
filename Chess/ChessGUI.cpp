#include "ChessGUI.h"
#include <iostream>
#include <sstream>

ChessGUI::ChessGUI(bool multiplayer)
    : window(sf::VideoMode({ 830u, 580u }), "Swag Chess"),
    game(),
    gameMode(multiplayer),
    isAIWhite(false),
    whiteTurn(true),
    selectedRow(-1),
    selectedCol(-1),
    pieceSelected(false),
    gameOver(false),
    historyScrollOffset(0) {
}

ChessGUI::~ChessGUI() {
}

bool ChessGUI::initialize() {
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return false;
    }

    if (!loadPieceTextures()) {
        std::cerr << "Failed to load piece textures!" << std::endl;
        return false;
    }

    // Initialize UI elements
    boardBackground.setSize(sf::Vector2f(BOARD_SIZE, BOARD_SIZE));
    boardBackground.setPosition(sf::Vector2f(BOARD_OFFSET_X, BOARD_OFFSET_Y));
    boardBackground.setFillColor(sf::Color(139, 69, 19));

    sidebar.setSize(sf::Vector2f(SIDEBAR_WIDTH, 580));
    sidebar.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 10, 0));
    sidebar.setFillColor(sf::Color(50, 50, 50));

    // Game status text
    gameStatusText.setFont(font);
    gameStatusText.setCharacterSize(20);
    gameStatusText.setFillColor(sf::Color::White);
    gameStatusText.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 20, 20));

    // Turn indicator
    turnIndicator.setFont(font);
    turnIndicator.setCharacterSize(18);
    turnIndicator.setFillColor(sf::Color::White);
    turnIndicator.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 20, 50));

    // Move history title
    moveHistoryTitle.setFont(font);
    moveHistoryTitle.setString("Move History:");
    moveHistoryTitle.setCharacterSize(16);
    moveHistoryTitle.setFillColor(sf::Color::White);
    moveHistoryTitle.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 20, 90));

    // New game button
    newGameButton.setSize(sf::Vector2f(120, 40));
    newGameButton.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 20, 480));
    newGameButton.setFillColor(sf::Color(0, 150, 0));

    newGameText.setFont(font);
    newGameText.setString("New Game");
    newGameText.setCharacterSize(14);
    newGameText.setFillColor(sf::Color::White);
    newGameText.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 35, 495));

    // Exit button
    exitButton.setSize(sf::Vector2f(120, 40));
    exitButton.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 150, 480));
    exitButton.setFillColor(sf::Color(150, 0, 0));

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(14);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 185, 495));

    updateGameStatus();

    return true;
}

void ChessGUI::run() {
    if (!initialize()) {
        return;
    }

    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void ChessGUI::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                handleMouseClick(static_cast<int>(mousePressed->position.x),
                    static_cast<int>(mousePressed->position.y));
            }
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Up && historyScrollOffset > 0) {
                historyScrollOffset--;
            }
            if (keyPressed->code == sf::Keyboard::Key::Down &&
                historyScrollOffset < static_cast<int>(moveHistory.size()) - 20) {
                historyScrollOffset++;
            }
        }
    }
}

void ChessGUI::update() {
    updateGameStatus();
    checkGameEnd();

    // Check if it's AI's turn in AI mode
    if (!gameMode && !gameOver) {
        if (whiteTurn == isAIWhite) {
            makeAIMove();
        }
    }
}

void ChessGUI::render() {
    window.clear(sf::Color(30, 30, 30));

    drawBoard();
    drawHighlights();
    drawPieces();
    drawSidebar();
    drawMoveHistory();
    drawGameStatus();
    drawUI();

    window.display();
}

void ChessGUI::handleMouseClick(int x, int y) {
    // Check if click is on the board
    if (x >= BOARD_OFFSET_X && x < BOARD_OFFSET_X + BOARD_SIZE &&
        y >= BOARD_OFFSET_Y && y < BOARD_OFFSET_Y + BOARD_SIZE) {
        auto [row, col] = pixelToSquare(x, y);
        handleBoardClick(row, col);
    }
    else {
        handleUIClick(x, y);
    }
}

void ChessGUI::handleBoardClick(int row, int col) {
    if (gameOver || row < 0 || row > 7 || col < 0 || col > 7) return;

    Board board = game.getBoard(); // Create a copy to avoid reference issues
    Piece* clickedPiece = board.getPiece(row, col);

    if (!pieceSelected) {
        // Select a piece
        if (clickedPiece != nullptr) {
            // Check if it's the correct player's turn
            if (clickedPiece->isWhitePiece() == whiteTurn || gameMode) {
                selectedRow = row;
                selectedCol = col;
                pieceSelected = true;
                updateValidMoves();
            }
        }
    }
    else {
        // Try to move the selected piece
        if (row == selectedRow && col == selectedCol) {
            // Deselect
            pieceSelected = false;
            validMoves.clear();
        }
        else {
            // Check if this is a valid move
            bool validMove = false;
            for (const auto& move : validMoves) {
                if (move.first == row && move.second == col) {
                    validMove = true;
                    break;
                }
            }

            if (validMove) {
                // Make the move
                Board tempBoard = game.getBoard();
                if (tempBoard.movePiece(selectedRow, selectedCol, row, col, whiteTurn)) {
                    // Move was successful - update the game board
                    game = Game(); // This is a placeholder - you'll need proper move integration

                    // Convert coordinates to chess notation
                    char fromFile = 'a' + selectedCol;
                    char toFile = 'a' + col;
                    int fromRank = 8 - selectedRow;
                    int toRank = 8 - row;

                    std::string moveNotation = std::string(1, fromFile) + std::to_string(fromRank) +
                        "-" + std::string(1, toFile) + std::to_string(toRank);
                    addMoveToHistory(moveNotation);

                    // Switch turns
                    whiteTurn = !whiteTurn;
                }
                pieceSelected = false;
                validMoves.clear();
            }
            else {
                // Try to select a new piece
                if (clickedPiece != nullptr) {
                    if (clickedPiece->isWhitePiece() == whiteTurn || gameMode) {
                        selectedRow = row;
                        selectedCol = col;
                        updateValidMoves();
                    }
                    else {
                        pieceSelected = false;
                        validMoves.clear();
                    }
                }
                else {
                    pieceSelected = false;
                    validMoves.clear();
                }
            }
        }
    }
}

void ChessGUI::handleUIClick(int x, int y) {
    sf::Vector2f mousePos(static_cast<float>(x), static_cast<float>(y));

    // Check new game button
    if (newGameButton.getGlobalBounds().contains(mousePos)) {
        resetGame();
    }

    // Check exit button
    if (exitButton.getGlobalBounds().contains(mousePos)) {
        window.close();
    }
}

void ChessGUI::drawBoard() {
    window.draw(boardBackground);

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            square.setPosition(sf::Vector2f(BOARD_OFFSET_X + col * SQUARE_SIZE,
                BOARD_OFFSET_Y + row * SQUARE_SIZE));

            if ((row + col) % 2 == 0) {
                square.setFillColor(lightSquareColor);
            }
            else {
                square.setFillColor(darkSquareColor);
            }

            window.draw(square);
        }
    }
}

void ChessGUI::drawPieces() {
    Board board = game.getBoard();

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = board.getPiece(row, col);
            if (piece != nullptr) {
                sf::Sprite& pieceSprite = getPieceSprite(piece->getSymbol(), piece->isWhitePiece());
                auto [x, y] = squareToPixel(row, col);
                pieceSprite.setPosition(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
                window.draw(pieceSprite);
            }
        }
    }
}

void ChessGUI::drawHighlights() {
    // Highlight selected square
    if (pieceSelected) {
        sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        auto [x, y] = squareToPixel(selectedRow, selectedCol);
        highlight.setPosition(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
        highlight.setFillColor(highlightColor);
        window.draw(highlight);
    }

    // Highlight valid moves
    for (const auto& move : validMoves) {
        sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
        auto [x, y] = squareToPixel(move.first, move.second);
        highlight.setPosition(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
        highlight.setFillColor(moveHighlight);
        window.draw(highlight);
    }

    // Highlight kings in check
    Board board = game.getBoard();
    if (board.isInCheck(true)) {
        // Find white king and highlight
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                Piece* piece = board.getPiece(row, col);
                if (piece && piece->getSymbol() == 'K' && piece->isWhitePiece()) {
                    sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                    auto [x, y] = squareToPixel(row, col);
                    highlight.setPosition(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
                    highlight.setFillColor(checkHighlight);
                    window.draw(highlight);
                    break;
                }
            }
        }
    }

    if (board.isInCheck(false)) {
        // Find black king and highlight
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                Piece* piece = board.getPiece(row, col);
                if (piece && piece->getSymbol() == 'k' && !piece->isWhitePiece()) {
                    sf::RectangleShape highlight(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
                    auto [x, y] = squareToPixel(row, col);
                    highlight.setPosition(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
                    highlight.setFillColor(checkHighlight);
                    window.draw(highlight);
                    break;
                }
            }
        }
    }
}

void ChessGUI::drawSidebar() {
    window.draw(sidebar);
}

void ChessGUI::drawMoveHistory() {
    window.draw(moveHistoryTitle);

    int startY = 120;
    int lineHeight = 18;
    int maxLines = 20;

    for (int i = historyScrollOffset;
        i < static_cast<int>(moveHistory.size()) && i < historyScrollOffset + maxLines;
        i++) {
        sf::Text moveText (font);
        moveText.setFont(font);
        moveText.setString(std::to_string(i + 1) + ". " + moveHistory[i]);
        moveText.setCharacterSize(12);
        moveText.setFillColor(sf::Color::White);
        moveText.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 20,
            startY + (i - historyScrollOffset) * lineHeight));
        window.draw(moveText);
    }

    // Scroll indicator
    if (moveHistory.size() > maxLines) {
        sf::Text scrollHint (font);
        scrollHint.setFont(font);
        scrollHint.setString("Use Up/Down arrows to scroll");
        scrollHint.setCharacterSize(10);
        scrollHint.setFillColor(sf::Color(150, 150, 150));
        scrollHint.setPosition(sf::Vector2f(BOARD_OFFSET_X + BOARD_SIZE + 20,
            startY + maxLines * lineHeight + 10));
        window.draw(scrollHint);
    }
}

void ChessGUI::drawGameStatus() {
    window.draw(gameStatusText);
    window.draw(turnIndicator);
}

void ChessGUI::drawUI() {
    window.draw(newGameButton);
    window.draw(newGameText);
    window.draw(exitButton);
    window.draw(exitText);
}

std::pair<int, int> ChessGUI::pixelToSquare(int x, int y) {
    int col = (x - BOARD_OFFSET_X) / SQUARE_SIZE;
    int row = (y - BOARD_OFFSET_Y) / SQUARE_SIZE;
    return { row, col };
}

std::pair<int, int> ChessGUI::squareToPixel(int row, int col) {
    int x = BOARD_OFFSET_X + col * SQUARE_SIZE;
    int y = BOARD_OFFSET_Y + row * SQUARE_SIZE;
    return { x, y };
}

void ChessGUI::updateValidMoves() {
    validMoves.clear();
    if (!pieceSelected) return;

    Board board = game.getBoard();
    Piece* piece = board.getPiece(selectedRow, selectedCol);
    if (piece == nullptr) return;

    // Check all possible moves
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (piece->isValidMove(selectedRow, selectedCol, row, col, &board)) {
                validMoves.push_back({ row, col });
            }
        }
    }
}

void ChessGUI::updateGameStatus() {
    if (gameMode) {
        gameStatusText.setString("Multiplayer Mode");
        turnIndicator.setString(whiteTurn ? "White's Turn" : "Black's Turn");
    }
    else {
        gameStatusText.setString("vs AI (Bird)");
        if (isAIWhite) {
            turnIndicator.setString(whiteTurn ? "AI's Turn" : "Your Turn");
        }
        else {
            turnIndicator.setString(whiteTurn ? "Your Turn" : "AI's Turn");
        }
    }

    if (gameOver) {
        turnIndicator.setString(gameResult);
    }
}

void ChessGUI::addMoveToHistory(const std::string& move) {
    moveHistory.push_back(move);
    if (moveHistory.size() > 100) { // Limit history size
        moveHistory.erase(moveHistory.begin());
    }
}

void ChessGUI::resetGame() {
    game = Game(); // Reset the game
    gameOver = false;
    gameResult = "";
    whiteTurn = true;
    pieceSelected = false;
    validMoves.clear();
    moveHistory.clear();
    historyScrollOffset = 0;
    updateGameStatus();
}

void ChessGUI::makeAIMove() {
    if (!gameOver) {
        std::string bestMove = game.findBestMove(isAIWhite);
        if (!bestMove.empty()) {
            addMoveToHistory("AI: " + bestMove);
            whiteTurn = !whiteTurn; // Switch turns after AI move
        }
    }
}

void ChessGUI::checkGameEnd() {
    Board board = game.getBoard();

    // Check for checkmate
    if (board.isCheckmate(true)) {
        gameResult = "Black wins by checkmate!";
        gameOver = true;
    }
    else if (board.isCheckmate(false)) {
        gameResult = "White wins by checkmate!";
        gameOver = true;
    }
    // Check for stalemate
    else if (board.isStalemate(true) || board.isStalemate(false)) {
        gameResult = "Stalemate - Draw!";
        gameOver = true;
    }
    // Check for insufficient material
    else if (board.insufficientMaterialCheck()) {
        gameResult = "Draw - Insufficient material!";
        gameOver = true;
    }
}

bool ChessGUI::loadPieceTextures() {
    std::vector<std::pair<char, bool>> pieces = {
        {'k', false}, {'q', false}, {'r', false}, {'b', false}, {'n', false}, {'p', false},
        {'K', true},  {'Q', true},  {'R', true},  {'B', true},  {'N', true},  {'P', true}
    };

    for (const auto& [piece, isWhite] : pieces) {
        std::string filename = getPieceFilename(piece, isWhite);
        std::string key = std::string(1, piece) + (isWhite ? "w" : "b");

        if (!pieceTextures[key].loadFromFile(filename)) {
            std::cerr << "Failed to load " << filename << std::endl;
            return false;
        }

        pieceSprites[key].setTexture(pieceTextures[key]);

        // Scale the sprite to fit the square
        sf::Vector2u textureSize = pieceTextures[key].getSize();
        float scaleX = static_cast<float>(SQUARE_SIZE) / textureSize.x;
        float scaleY = static_cast<float>(SQUARE_SIZE) / textureSize.y;
        pieceSprites[key].setScale(sf::Vector2f(scaleX, scaleY));
    }

    return true;
}

std::string ChessGUI::getPieceFilename(char piece, bool isWhite) {
    char color = isWhite ? 'w' : 'b';
    char pieceChar = std::tolower(piece);

    return std::string(1, color) + std::string(1, pieceChar) + ".png";
}

sf::Sprite& ChessGUI::getPieceSprite(char piece, bool isWhite) {
    std::string key = std::string(1, piece) + (isWhite ? "w" : "b");
    return pieceSprites[key];
}