#include "ChessGUI.h"
#include <iostream>
#include <filesystem>

ChessGUI::ChessGUI(bool playVsAI, bool aiPlaysWhite)
    : window(sf::VideoMode({ 800u, 800u }), "Swag Chess"),
    pieceSelected(false), selectedRow(-1), selectedCol(-1),
    gameOver(false), vsAI(playVsAI), aiIsWhite(aiPlaysWhite),
    statusText(font), turnText(font), newGameText(font), modeText(font),
    fiftyMoveCounter(0) {  // Add aiThinking flag

    // Initialize highlight square
    highlightSquare.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    highlightSquare.setFillColor(getHighlightColor());

    // Initialize UI elements
    newGameButton.setSize(sf::Vector2f(120, 40));
    newGameButton.setPosition(sf::Vector2f(20, 20));
    newGameButton.setFillColor(sf::Color(70, 130, 180));

    modeButton.setSize(sf::Vector2f(120, 40));
    modeButton.setPosition(sf::Vector2f(150, 20));
    modeButton.setFillColor(sf::Color(180, 130, 70));

    // Initialize AI thinking timer
    aiThinkTimer = sf::Time::Zero;
}

ChessGUI::~ChessGUI() {
    // Destructor
}

bool ChessGUI::checkThreefoldRepetition() {
    if (positionHistory.size() < 3) return false;

    // Get current position
    std::string currentPosition = game.getBoard().getSimplePosition(game.isWhiteTurn());

    // Count occurrences of current position
    int count = 0;
    for (const auto& pos : positionHistory) {
        if (pos == currentPosition) {
            count++;
        }
    }

    // Check if this position has occurred twice before (making this the third time)
    return count >= 2;
}

// Add this method to check fifty move rule:
bool ChessGUI::checkFiftyMoveRule() {
    return fiftyMoveCounter >= 50;
}

void ChessGUI::updateDrawConditions() {
    // Add current position to history
    std::string currentPosition = game.getBoard().getSimplePosition(game.isWhiteTurn());
    positionHistory.push_back(currentPosition);

    // Keep position history reasonable size (last 150 positions should be enough)
    if (positionHistory.size() > 150) {
        positionHistory.erase(positionHistory.begin());
    }
}

bool ChessGUI::initialize() {
    // Load font FIRST before setting up any text elements
    std::cout << "Starting initialization..." << std::endl;
    if (!font.openFromFile("arial.ttf")) {
        // Try alternative font locations
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf") &&
            !font.openFromFile("C:/Windows/Fonts/Arial.ttf") &&
            !font.openFromFile("/System/Library/Fonts/Arial.ttf") &&
            !font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") &&
            !font.openFromFile("assets/arial.ttf")) {
            std::cerr << "Could not load any font! Trying to create default font..." << std::endl;
        }
    }
    std::cout << "Font loaded successfully" << std::endl;

    // Setup text elements AFTER font is loaded
    statusText.setFont(font);
    statusText.setCharacterSize(18);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(sf::Vector2f(20, 750));

    turnText.setFont(font);
    turnText.setCharacterSize(16);
    turnText.setFillColor(sf::Color::White);
    turnText.setPosition(sf::Vector2f(400, 750));

    newGameText.setFont(font);
    newGameText.setString("New Game");
    newGameText.setCharacterSize(14);
    newGameText.setFillColor(sf::Color::White);
    newGameText.setPosition(sf::Vector2f(35, 30));

    modeText.setFont(font);
    modeText.setString(vsAI ? "vs AI" : "vs Human");
    modeText.setCharacterSize(14);
    modeText.setFillColor(sf::Color::White);
    modeText.setPosition(sf::Vector2f(175, 30));
    std::cout << "Text elements configured" << std::endl;

    // Load piece textures
    if (!loadPieceTextures()) {
        std::cerr << "Failed to load piece textures!" << std::endl;
        return false;
    }
    std::cout << "Pieces loaded" << std::endl;

    // Initialize game
    game = Game();
    updateGameStatus();

    // Start AI thinking timer if AI goes first
    if (vsAI && aiIsWhite) {
        aiThinking = true;
        aiThinkTimer = sf::Time::Zero;
    }

    std::cout << "Game initialized" << std::endl;
    return true;
}

bool ChessGUI::loadPieceTextures() {
    std::vector<std::string> pieces = { "wp", "wr", "wn", "wb", "wq", "wk",
                                       "bp", "br", "bn", "bb", "bq", "bk" };

    for (const std::string& piece : pieces) {
        sf::Texture texture;
        std::string path = "assets/" + piece + ".png";

        if (!texture.loadFromFile(path)) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return false;
        }

        pieceTextures[piece] = std::move(texture);
    }

    std::cout << "Successfully loaded " << pieceTextures.size() << " piece textures." << std::endl;
    return true;
}

void ChessGUI::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();

        handleEvents();
        update(deltaTime);  // Pass deltaTime to update
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
                sf::Vector2i mousePos = sf::Vector2i(mousePressed->position);

                // Check UI button clicks
                if (newGameButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    resetGame();
                    continue;
                }

                if (modeButton.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    toggleMode();
                    continue;
                }

                // Handle board clicks only if it's human's turn and AI is not thinking
                if (!gameOver && !aiThinking && (!vsAI || (vsAI && game.isWhiteTurn() != aiIsWhite))) {
                    sf::Vector2i boardPos = getBoardPosition(mousePos);

                    if (isValidBoardPosition(boardPos)) {
                        if (!pieceSelected) {
                            selectPiece(boardPos.y, boardPos.x);
                        }
                        else {
                            if (boardPos.x == selectedCol && boardPos.y == selectedRow) {
                                clearSelection();
                            }
                            else {
                                movePiece(boardPos.y, boardPos.x);
                            }
                        }
                    }
                }
            }
        }
    }
}

void ChessGUI::update(sf::Time deltaTime) {
    updateGameStatus();

    // Handle AI thinking with delay
    if (!gameOver && vsAI && game.isWhiteTurn() == aiIsWhite) {
        if (!aiThinking) {
            // Start AI thinking
            aiThinking = true;
            aiThinkTimer = sf::Time::Zero;
        }
        else {
            // Update thinking timer
            aiThinkTimer += deltaTime;

            // Add a small delay to show the turn change and make it feel more natural
            if (aiThinkTimer >= sf::milliseconds(500)) {  // 500ms delay
                handleAIMove();
                aiThinking = false;
            }
        }
    }
}

void ChessGUI::render() {
    window.clear(sf::Color(40, 40, 40));

    drawBoard();
    drawHighlights();
    drawPieces();
    drawUI();

    window.display();
}

sf::Vector2i ChessGUI::getBoardPosition(sf::Vector2i mousePos) {
    int x = (mousePos.x - BOARD_OFFSET_X) / SQUARE_SIZE;
    int y = (mousePos.y - BOARD_OFFSET_Y) / SQUARE_SIZE;
    return sf::Vector2i(x, y);
}

bool ChessGUI::isValidBoardPosition(sf::Vector2i pos) {
    return pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8;
}

void ChessGUI::selectPiece(int row, int col) {
    Board& board = game.getBoard();
    Piece* piece = board.getPiece(row, col);

    if (piece && piece->isWhitePiece() == game.isWhiteTurn()) {
        pieceSelected = true;
        selectedRow = row;
        selectedCol = col;

        // Calculate valid moves
        validMoves.clear();
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                if (piece->isValidMove(row, col, r, c, &board)) {
                    validMoves.push_back(sf::Vector2i(c, r));
                }
            }
        }
    }
}

void ChessGUI::movePiece(int toRow, int toCol) {
    if (pieceSelected) {
        Board& boardRef = game.getBoard();

        // Store the move details before making the move
        Move move;
        move.fromCoord = string(1, 'A' + selectedCol) + to_string(8 - selectedRow);
        move.toCoord = string(1, 'A' + toCol) + to_string(8 - toRow);
        move.player = game.isWhiteTurn() ? "White" : "Black";

        Piece* piece = boardRef.getPiece(selectedRow, selectedCol);
        Piece* capturedPiece = boardRef.getPiece(toRow, toCol);

        if (piece) {
            move.pieceSymbol = piece->getSymbol();
        }

        if (boardRef.movePiece(selectedRow, selectedCol, toRow, toCol, game.isWhiteTurn())) {
            // IMPORTANT: Set the last move for en passant tracking
            boardRef.setLastMove(move);

            // Update fifty move rule counter
            bool isPawnMove = (piece && tolower(piece->getSymbol()) == 'p');
            bool isCapture = (capturedPiece != nullptr);

            if (isPawnMove || isCapture) {
                fiftyMoveCounter = 0;  // Reset counter
            }
            else {
                fiftyMoveCounter++;
            }

            // Update position history for threefold repetition
            updateDrawConditions();

            // Move was successful - toggle turn
            game.toggleTurn();

            // Force immediate GUI update
            render();

            // Clear selection after rendering
            clearSelection();

            // If playing against AI and it's now AI's turn, make it move immediately
            if (vsAI && game.isWhiteTurn() == aiIsWhite) {
                handleAIMove();
                render(); // Update again after AI move
            }
        }
        else {
            // Invalid move
            clearSelection();
        }
    }
}


void ChessGUI::clearSelection() {
    pieceSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    validMoves.clear();
}

void ChessGUI::handleAIMove() {
    std::string bestMove = game.findBestMove(aiIsWhite);

    if (bestMove.empty()) {
        std::cout << "AI couldn't find a valid move!" << std::endl;
        return;
    }

    std::cout << "AI considering move: " << bestMove << std::endl;

    // Handle castling moves first
    if (bestMove == "O-O" || bestMove == "O-O-O") {
        int fromRow = aiIsWhite ? 7 : 0;
        int fromCol = 4; // King starts at e-file
        int toRow = fromRow;
        int toCol = (bestMove == "O-O") ? 6 : 2; // Kingside or queenside

        Board& boardRef = game.getBoard();

        // Create move record for castling
        Move move;
        move.player = aiIsWhite ? "White" : "Black";
        move.fromCoord = string(1, 'A' + fromCol) + to_string(8 - fromRow);
        move.toCoord = string(1, 'A' + toCol) + to_string(8 - toRow);
        move.pieceSymbol = 'K';
        move.isKingsideCastle = (bestMove == "O-O");
        move.isQueensideCastle = (bestMove == "O-O-O");

        if (boardRef.movePiece(fromRow, fromCol, toRow, toCol, aiIsWhite)) {
            boardRef.setLastMove(move);
            game.toggleTurn();
            updateGameStatus();
            std::cout << "AI castled: " << bestMove << std::endl;
        }
        return;
    }

    // Parse regular moves - handle both "A2 E4" and "a2e4" formats
    int fromCol, fromRow, toCol, toRow;

    if (bestMove.length() >= 5 && bestMove[2] == ' ') {
        // Format: "A2 E4" (with space)
        fromCol = toupper(bestMove[0]) - 'A';
        fromRow = 8 - (bestMove[1] - '0');
        toCol = toupper(bestMove[3]) - 'A';
        toRow = 8 - (bestMove[4] - '0');
    }
    else if (bestMove.length() >= 4) {
        // Format: "a2e4" (no space)
        fromCol = tolower(bestMove[0]) - 'a';
        fromRow = 8 - (bestMove[1] - '0');
        toCol = tolower(bestMove[2]) - 'a';
        toRow = 8 - (bestMove[3] - '0');
    }
    else {
        std::cout << "Invalid AI move format: " << bestMove << std::endl;
        return;
    }

    // Validate coordinates
    if (fromCol < 0 || fromCol >= 8 || fromRow < 0 || fromRow >= 8 ||
        toCol < 0 || toCol >= 8 || toRow < 0 || toRow >= 8) {
        std::cout << "AI move coordinates out of bounds: " << bestMove << std::endl;
        return;
    }

    Board& boardRef = game.getBoard();
    Piece* piece = boardRef.getPiece(fromRow, fromCol);

    if (!piece) {
        std::cout << "No piece at AI's from position!" << std::endl;
        return;
    }

    if (piece->isWhitePiece() != aiIsWhite) {
        std::cout << "AI trying to move opponent's piece!" << std::endl;
        return;
    }

    // Create proper move record
    Move move;
    move.player = aiIsWhite ? "White" : "Black";
    move.fromCoord = string(1, 'A' + fromCol) + to_string(8 - fromRow);
    move.toCoord = string(1, 'A' + toCol) + to_string(8 - toRow);
    move.pieceSymbol = piece->getSymbol();

    // Check for capture
    Piece* capturedPiece = boardRef.getPiece(toRow, toCol);
    move.isCapture = (capturedPiece != nullptr);

    // Attempt the move
    if (boardRef.movePiece(fromRow, fromCol, toRow, toCol, aiIsWhite)) {
        // Set the move for game history and en passant tracking
        boardRef.setLastMove(move);

        // Update fifty move rule counter for AI moves
        bool isPawnMove = (piece && tolower(piece->getSymbol()) == 'p');
        bool isCapture = (capturedPiece != nullptr);

        if (isPawnMove || isCapture) {
            fiftyMoveCounter = 0;  // Reset counter
        }
        else {
            fiftyMoveCounter++;
        }

        // Update position history for threefold repetition
        updateDrawConditions();

        game.toggleTurn();
        updateGameStatus();

        std::cout << "AI moved from " << move.fromCoord << " to " << move.toCoord << std::endl;
    }
    else {
        std::cout << "AI move was invalid: " << bestMove << std::endl;
    }
}


void ChessGUI::drawBoard() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            sf::RectangleShape square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            square.setPosition(sf::Vector2f(BOARD_OFFSET_X + col * SQUARE_SIZE,
                BOARD_OFFSET_Y + row * SQUARE_SIZE));

            // Alternate colors
            if ((row + col) % 2 == 0) {
                square.setFillColor(getLightSquareColor());
            }
            else {
                square.setFillColor(getDarkSquareColor());
            }

            window.draw(square);
        }
    }
}

void ChessGUI::drawPieces() {
    Board& board = game.getBoard(); // Reference, not copy!

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = board.getPiece(row, col);
            if (piece) {
                std::string textureKey = getPieceTextureKey(piece);

                if (pieceTextures.find(textureKey) != pieceTextures.end()) {
                    sf::Sprite sprite(pieceTextures[textureKey]);

                    // Scale the sprite to fit the square
                    sf::Vector2u textureSize = pieceTextures[textureKey].getSize();
                    float scaleX = (float)SQUARE_SIZE / textureSize.x;
                    float scaleY = (float)SQUARE_SIZE / textureSize.y;
                    sprite.setScale(sf::Vector2f(scaleX, scaleY));

                    sprite.setPosition(sf::Vector2f(BOARD_OFFSET_X + col * SQUARE_SIZE,
                        BOARD_OFFSET_Y + row * SQUARE_SIZE));

                    window.draw(sprite);
                }
            }
        }
    }
}

void ChessGUI::drawHighlights() {
    // Highlight selected square
    if (pieceSelected) {
        highlightSquare.setPosition(sf::Vector2f(BOARD_OFFSET_X + selectedCol * SQUARE_SIZE,
            BOARD_OFFSET_Y + selectedRow * SQUARE_SIZE));
        window.draw(highlightSquare);
    }

    // Highlight valid moves
    sf::RectangleShape validMoveSquare(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    validMoveSquare.setFillColor(getValidMoveColor());

    for (const sf::Vector2i& move : validMoves) {
        validMoveSquare.setPosition(sf::Vector2f(BOARD_OFFSET_X + move.x * SQUARE_SIZE,
            BOARD_OFFSET_Y + move.y * SQUARE_SIZE));
        window.draw(validMoveSquare);
    }
}

void ChessGUI::drawUI() {
    // Draw buttons
    window.draw(newGameButton);
    window.draw(newGameText);
    window.draw(modeButton);
    window.draw(modeText);

    // Draw status texts
    window.draw(statusText);
    window.draw(turnText);
}

std::string ChessGUI::getPieceTextureKey(Piece* piece) {
    if (!piece) return "";

    char symbol = piece->getSymbol();
    std::string key;

    if (piece->isWhitePiece()) {
        key = "w";
    }
    else {
        key = "b";
    }

    // Convert piece symbol to lowercase
    key += tolower(symbol);

    return key;
}

sf::Color ChessGUI::getLightSquareColor() {
    return sf::Color(240, 217, 181);  // Light beige
}

sf::Color ChessGUI::getDarkSquareColor() {
    return sf::Color(181, 136, 99);   // Dark brown
}

sf::Color ChessGUI::getHighlightColor() {
    return sf::Color(255, 255, 0, 128);  // Semi-transparent yellow
}

sf::Color ChessGUI::getValidMoveColor() {
    return sf::Color(0, 255, 0, 64);     // Semi-transparent green
}

void ChessGUI::updateGameStatus() {
    Board& board = game.getBoard();

    // Update turn text
    if (game.isWhiteTurn()) {
        turnText.setString("White's Turn");
    }
    else {
        turnText.setString("Black's Turn");
    }

    // Check for draws first
    if (checkThreefoldRepetition()) {
        statusText.setString("Draw by Threefold Repetition!");
        gameOver = true;
        return;
    }

    if (checkFiftyMoveRule()) {
        statusText.setString("Draw by Fifty Move Rule!");
        gameOver = true;
        return;
    }

    if (board.insufficientMaterialCheck()) {
        statusText.setString("Draw by Insufficient Material!");
        gameOver = true;
        return;
    }

    // Check game state - FIXED: Check if CURRENT player is in checkmate
    if (board.isCheckmate(game.isWhiteTurn())) {
        statusText.setString(game.isWhiteTurn() ? "Black Wins by Checkmate!" : "White Wins by Checkmate!");
        gameOver = true;
    }
    else if (board.isStalemate(game.isWhiteTurn())) {
        statusText.setString("Stalemate - Draw!");
        gameOver = true;
    }
    else if (board.isInCheck(game.isWhiteTurn())) {
        statusText.setString(game.isWhiteTurn() ? "White in Check!" : "Black in Check!");
    }
    else {
        statusText.setString("Game in Progress");
    }

    // Update mode text
    modeText.setString(vsAI ? "vs AI" : "vs Human");
}
void ChessGUI::resetGame() {
    game = Game();
    pieceSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    gameOver = false;
    validMoves.clear();
    positionHistory.clear();      // Clear position history
    fiftyMoveCounter = 0;         // Reset fifty move counter
    updateGameStatus();
    // Start AI thinking if AI goes first
    if (vsAI && aiIsWhite) {
        aiThinking = true;
        aiThinkTimer = sf::Time::Zero;
    }
}

void ChessGUI::toggleMode() {
    vsAI = !vsAI;
    aiIsWhite = false;  // AI plays black by default
    resetGame();
}