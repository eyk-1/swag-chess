#include "ChessGUI.h"
#include "Pawn.h"
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
    if (positionHistory.size() < 8) return false; // Need at least 4 previous positions

    // Get current position
    std::string currentPosition = game.getBoard().getSimplePosition(game.isWhiteTurn());

    // Count occurrences of current position in history
    int count = 1; // Start with 1 for current position
    for (const auto& pos : positionHistory) {
        if (pos == currentPosition) {
            count++;
        }
    }

    // Threefold repetition occurs when position appears 3 or more times
    return count >= 3;
}

// Add this method to check fifty move rule:
bool ChessGUI::checkFiftyMoveRule() {
    return fiftyMoveCounter >= 100;
}

void ChessGUI::updateDrawConditions() {
    // Add current position to history AFTER making the move
    std::string currentPosition = game.getBoard().getSimplePosition(game.isWhiteTurn());
    positionHistory.push_back(currentPosition);

    // Keep position history reasonable size (last 150 positions should be enough)
    if (positionHistory.size() > 150) {
        positionHistory.erase(positionHistory.begin());
    }
}

bool ChessGUI::initialize() {
    // Load font FIRST before setting up any text elements
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
    modeText.setString(vsAI ? "vs Bird" : "vs Human");
    modeText.setCharacterSize(14);
    modeText.setFillColor(sf::Color::White);
    modeText.setPosition(sf::Vector2f(175, 30));
    

    // Load piece textures
    if (!loadPieceTextures()) {
        std::cerr << "Failed to load piece textures!" << std::endl;
        return false;
    }
    

    // Initialize game
    game = Game();
    updateGameStatus();

    // Start AI thinking timer if AI goes first
    if (vsAI && aiIsWhite) {
        aiThinking = true;
        aiThinkTimer = sf::Time::Zero;
    }
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
            // Update status to show AI is thinking
            if (aiIsWhite) {
                statusText.setString("White (Bird) is thinking...");
            }
            else {
                statusText.setString("Black (Bird) is thinking...");
            }
        }
        else {
            // Update thinking timer
            aiThinkTimer += deltaTime;

            // Add a small delay to show the turn change and make it feel more natural
            if (aiThinkTimer >= sf::milliseconds(500)) {  // 500ms delay
                handleAIMove();
                aiThinking = false;
                // Update status after AI move
                updateGameStatus();
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

        // Calculate valid moves - but only show LEGAL moves
        validMoves.clear();
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                // First check if the piece can theoretically move there
                if (piece->isValidMove(row, col, r, c, &board)) {
                    // Then check if this move would be legal (using the same logic as movePiece)

                    // Simulate the move to see if it exposes the king
                    Piece* tempFrom = board.getPiece(row, col);
                    Piece* tempTo = board.getPiece(r, c);

                    // Temporarily make the move
                    board.setPiece(r, c, tempFrom);
                    board.setPiece(row, col, nullptr);

                    // Check if this move leaves our king in check
                    bool exposesKing = board.isInCheck(game.isWhiteTurn());

                    // Restore the board state
                    board.setPiece(row, col, tempFrom);
                    board.setPiece(r, c, tempTo);

                    // Only add to valid moves if it doesn't expose the king
                    if (!exposesKing) {
                        //  capture our own piece check
                        Piece* destPiece = board.getPiece(r, c);
                        if (!destPiece || destPiece->isWhitePiece() != game.isWhiteTurn()) {
                            validMoves.push_back(sf::Vector2i(c, r));
                        }
                    }
                }
            }
        }
    }
}

void ChessGUI::movePiece(int toRow, int toCol) {
    if (pieceSelected) {
        Board& boardRef = game.getBoard();
        Piece* piece = boardRef.getPiece(selectedRow, selectedCol);
        Piece* capturedPiece = boardRef.getPiece(toRow, toCol);
        bool isPromotion = boardRef.isPromotionMove(selectedRow, selectedCol, toRow, toCol, game.isWhiteTurn());
        // Store the move details before making the move
        string fromCoord = string(1, 'A' + selectedCol) + to_string(8 - selectedRow);
        string toCoord = string(1, 'A' + toCol) + to_string(8 - toRow);
        bool isEnPassant = (piece && dynamic_cast<Pawn*>(piece) && selectedCol != toCol && capturedPiece != nullptr);
        bool isKingside = false;
        bool isQueenside = false;
        if (piece && tolower(piece->getSymbol()) == 'k') { // If moving a king
            int colDiff = abs(toCol - selectedCol);
            // Castling detected if king moves 2 squares horizontally on same row
            if (colDiff == 2 && toRow == selectedRow) {
                if (toCol > selectedCol) {
                    // Kingside castling (king moves right)
                    isKingside = true;
                }
                else {
                    // Queenside castling (king moves left)  
                    isQueenside = true;
                }
            }
        }
        if (!dynamic_cast<Pawn*>(piece)) {
            game.AmbiguityCheck(boardRef, game.isWhiteTurn(), selectedRow, selectedCol, toRow, toCol);
        }
        // Move creation
        Move move = {
            game.isWhiteTurn() ? "White" : "Black",
            fromCoord,
            toCoord,
            piece ? piece->getSymbol() : '?',
            capturedPiece != nullptr || isEnPassant,
            isEnPassant,
            isPromotion,
            'Q',
            boardRef.isInCheck(!game.isWhiteTurn()),
            boardRef.isCheckmate(!game.isWhiteTurn()),
            isKingside,
            isQueenside,
            game.getRankCheck(),
            game.getFileCheck()
        };

        if (boardRef.movePiece(selectedRow, selectedCol, toRow, toCol, game.isWhiteTurn())) {
            // IMPORTANT: Set the last move for en passant tracking
            boardRef.setLastMove(move);
            bool isPawnMove = (piece && tolower(piece->getSymbol()) == 'p');
            bool isCapture = (capturedPiece != nullptr);

            if (isPawnMove || isCapture) {
                fiftyMoveCounter = 0;  // Reset counter
                // Also clear position history on irreversible moves
                positionHistory.clear();
            }
            else {
                fiftyMoveCounter++;
            }
            moves++;

            // FEN string creation
            string fen = boardRef.generateFEN(game.isWhiteTurn(), boardRef.getWhiteCanCastleKingside(),
                boardRef.getWhiteCanCastleQueenside(), boardRef.getBlackCanCastleKingside(), boardRef.getBlackCanCastleQueenside(), fiftyMoveCounter, moves);
            // Store moves in the vectors
            game.addMove(move, fen);
            // Store current turn before toggling
            bool wasWhiteTurn = game.isWhiteTurn();

            // Toggle turn
            game.toggleTurn();

            // Update position history AFTER the move and turn toggle
            updateDrawConditions();

            // Check for threefold repetition only after White's move
            // (i.e., when it's now Black's turn, meaning White just moved)
            if (!wasWhiteTurn) { // White just moved, now it's Black's turn
                if (checkThreefoldRepetition()) {
                    gameOver = true;
                    statusText.setString("Draw by Threefold Repetition!");
                    clearSelection();
                    return;
                }
            }

            // Update game status to show it's now AI's turn
            updateGameStatus();

            // Force immediate GUI update to show the turn change
            render();

            // Clear selection after rendering
            clearSelection();

            // If playing against AI and it's now AI's turn, start AI thinking
            if (vsAI && game.isWhiteTurn() == aiIsWhite) {
                aiThinking = true;
                aiThinkTimer = sf::Time::Zero;
                // Don't call handleAIMove() immediately - let update() handle it with delay
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
        return;
    }


    // Handle castling moves first
    if (bestMove == "O-O" || bestMove == "O-O-O") {
        int fromRow = aiIsWhite ? 7 : 0;
        int fromCol = 4; // King starts at e-file
        int toRow = fromRow;
        int toCol = (bestMove == "O-O") ? 6 : 2; // Kingside or queenside

        Board& boardRef = game.getBoard();

        Piece* piece = boardRef.getPiece(selectedRow, selectedCol);
        Piece* capturedPiece = boardRef.getPiece(toRow, toCol);
        // Store the move details before making the move
        string fromCoord = string(1, 'A' + selectedCol) + to_string(8 - selectedRow);
        string toCoord = string(1, 'A' + toCol) + to_string(8 - toRow);
        bool isKingside = false;
        bool isQueenside = false;
        if (piece && tolower(piece->getSymbol()) == 'k') { // If moving a king
            int colDiff = abs(toCol - selectedCol);
            // Castling detected if king moves 2 squares horizontally on same row
            if (colDiff == 2 && toRow == selectedRow) {
                if (toCol > selectedCol) {
                    // Kingside castling (king moves right)
                    isKingside = true;
                }
                else {
                    // Queenside castling (king moves left)  
                    isQueenside = true;
                }
            }
        }
        // Move creation
        Move move = {
            game.isWhiteTurn() ? "White" : "Black",
            fromCoord,
            toCoord,
            piece ? piece->getSymbol() : '?',
            false,
            false,
            false,
            'Q',
            boardRef.isInCheck(!game.isWhiteTurn()),
            boardRef.isCheckmate(!game.isWhiteTurn()),
            isKingside,
            isQueenside,
            false,
            false
        };

        if (boardRef.movePiece(fromRow, fromCol, toRow, toCol, game.isWhiteTurn())) {
            // IMPORTANT: Set the last move for en passant tracking
            boardRef.setLastMove(move);
            fiftyMoveCounter++;
            moves++;
            // FEN string creation
            string fen = boardRef.generateFEN(game.isWhiteTurn(), boardRef.getWhiteCanCastleKingside(),
                boardRef.getWhiteCanCastleQueenside(), boardRef.getBlackCanCastleKingside(), boardRef.getBlackCanCastleQueenside(), fiftyMoveCounter, moves);
            // Store moves in the vectors
            game.addMove(move, fen);
            // Store current turn before toggling
            bool wasWhiteTurn = game.isWhiteTurn();

            game.toggleTurn();
            updateDrawConditions(); // Add position after turn toggle

            // Check for threefold repetition only after White's move
            if (!wasWhiteTurn) { // White just moved
                if (checkThreefoldRepetition()) {
                    gameOver = true;
                    statusText.setString("Draw by Threefold Repetition!");
                    return;
                }
            }

            updateGameStatus();
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
    Piece* capturedPiece = boardRef.getPiece(toRow, toCol);
    bool isPromotion = boardRef.isPromotionMove(fromRow, fromCol, toRow, toCol, game.isWhiteTurn());
    // Store the move details before making the move
    string fromCoord = string(1, 'A' + fromCol) + to_string(8 - fromRow);
    string toCoord = string(1, 'A' + toCol) + to_string(8 - toRow);
    bool isEnPassant = (piece && dynamic_cast<Pawn*>(piece) && fromCol != toCol && capturedPiece != nullptr);
    bool isKingside = false;
    bool isQueenside = false;
    if (!dynamic_cast<Pawn*>(piece)) {
        game.AmbiguityCheck(boardRef, game.isWhiteTurn(), fromRow, fromCol, toRow, toCol);
    }
    // Move creation
    Move move = {
        game.isWhiteTurn() ? "White" : "Black",
        fromCoord,
        toCoord,
        piece ? piece->getSymbol() : '?',
        capturedPiece != nullptr || isEnPassant,
        isEnPassant,
        isPromotion,
        'Q',
        boardRef.isInCheck(!game.isWhiteTurn()),
        boardRef.isCheckmate(!game.isWhiteTurn()),
        isKingside,
        isQueenside,
        game.getRankCheck(),
        game.getFileCheck()
    };

    if (boardRef.movePiece(fromRow, fromCol, toRow, toCol, game.isWhiteTurn())) {
        // IMPORTANT: Set the last move for en passant tracking
        boardRef.setLastMove(move);
        bool isPawnMove = (piece && tolower(piece->getSymbol()) == 'p');
        bool isCapture = (capturedPiece != nullptr);

        if (isPawnMove || isCapture) {
            fiftyMoveCounter = 0;  // Reset counter
            // Also clear position history on irreversible moves
            positionHistory.clear();
        }
        else {
            fiftyMoveCounter++;
        }
        moves++;

        // FEN string creation
        string fen = boardRef.generateFEN(game.isWhiteTurn(), boardRef.getWhiteCanCastleKingside(),
            boardRef.getWhiteCanCastleQueenside(), boardRef.getBlackCanCastleKingside(), boardRef.getBlackCanCastleQueenside(), fiftyMoveCounter, moves);
        // Store moves in the vectors
        game.addMove(move, fen);
        // Store current turn before toggling
        bool wasWhiteTurn = game.isWhiteTurn();

        game.toggleTurn();
        updateDrawConditions(); // Add position after turn toggle

        // Check for threefold repetition only after White's move
        if (!wasWhiteTurn) { // White just moved
            if (checkThreefoldRepetition()) {
                gameOver = true;
                statusText.setString("Draw by Threefold Repetition!");
                return;
            }
        }

        updateGameStatus();
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
    return sf::Color(232, 228, 210);  // Light beige
}

sf::Color ChessGUI::getDarkSquareColor() {
    return sf::Color(70, 108, 142);   // Dark brown
}

sf::Color ChessGUI::getHighlightColor() {
    return sf::Color(255, 255, 0, 128);  // Semi-transparent yellow
}

sf::Color ChessGUI::getValidMoveColor() {
    return sf::Color(0, 255, 0, 64);     // Semi-transparent green
}

void ChessGUI::updateGameStatus() {
    Board& board = game.getBoard();

    // Update turn text - Show AI thinking status during AI turn
    if (aiThinking && vsAI && game.isWhiteTurn() == aiIsWhite) {
        if (aiIsWhite) {
            turnText.setString("White (Bird) Thinking...");
        }
        else {
            turnText.setString("Black (Bird) Thinking...");
        }
    }
    else {
        if (game.isWhiteTurn()) {
            turnText.setString(vsAI && aiIsWhite ? "White (AI) Turn" : "White's Turn");
        }
        else {
            turnText.setString(vsAI && !aiIsWhite ? "Black (AI) Turn" : "Black's Turn");
        }
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
    else if (!aiThinking) {  // Only show "Game in Progress" when AI is not thinking
        statusText.setString("Game in Progress");
    }

    // Update mode text
    modeText.setString(vsAI ? "vs Bird" : "vs Human");
}

void ChessGUI::resetGame() {
    game.printMoveHistory();
    game = Game();
    pieceSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    gameOver = false;
    aiThinking = false;  // Reset AI thinking state
    aiThinkTimer = sf::Time::Zero;
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