#include "Game.h"
#include "Moves.h"
#include "Rook.h"
#include "Pawn.h"
#include "King.h"
#include "Queen.h"
#include "Knight.h"
#include "Bishop.h"
#include "Board.h"
#include <iostream>
#include <cctype>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <string>
// Piece-square tables (from white's perspective)
static const int pawnTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

static const int knightTable[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

static const int bishopTable[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

static const int rookTable[8][8] = {
    { 0,  0,  0,  5,  5,  0,  0,  0},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

static const int queenTable[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

static const int kingTable[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};
Game::Game() : whiteTurn(true) {
    std::srand(std::time(nullptr));
}

void Game::printPGN(const std::vector<std::string>& pgnMoves) {
    for (size_t i = 0; i < pgnMoves.size(); ++i) {
        if (i % 2 == 0)
            std::cout << (i / 2 + 1) << ". ";
        std::cout << pgnMoves[i] << " ";
        if (i % 2 == 1)
            std::cout << "\n";
    }

    std::cout << "\n";
}

void Game::printFEN(const vector<string>& fenMoves) {
    cout << endl;
    for (size_t i = 0; i < fenMoves.size(); ++i) {
        cout << fenMoves[i] << endl;
    }

    std::cout << "\n";
}

string Game::findBestMove(bool isAIWhite) {
    int bestScore = isAIWhite ? -10000 : 10000;
    string bestMove = "";

    // Search all possible moves for the AI
    for (int fromRow = 0; fromRow < 8; fromRow++) {
        for (int fromCol = 0; fromCol < 8; fromCol++) {
            Piece* piece = board.getPiece(fromRow, fromCol);
            if (piece && piece->isWhitePiece() == isAIWhite) {
                // Try all possible destination squares
                for (int toRow = 0; toRow < 8; toRow++) {
                    for (int toCol = 0; toCol < 8; toCol++) {
                        if (piece->isValidMove(fromRow, fromCol, toRow, toCol, &board)) {
                            // Create a copy of the board to test the move
                            Board testBoard = board;
                            if (testBoard.movePiece(fromRow, fromCol, toRow, toCol, isAIWhite)) {
                                // Make sure this move doesn't leave our king in check
                                if (!testBoard.isInCheck(isAIWhite)) {
                                    // Evaluate this position
                                    int score = evaluateBoard(testBoard, isAIWhite);

                                    // Check if this is better than our current best
                                    if ((isAIWhite && score > bestScore) || (!isAIWhite && score < bestScore)) {
                                        bestScore = score;
                                        // Convert to algebraic notation
                                        char fromFile = 'a' + fromCol;
                                        char fromRank = '1' + (7 - fromRow); // Convert to chess notation
                                        char toFile = 'a' + toCol;
                                        char toRank = '1' + (7 - toRow);

                                        bestMove = string(1, fromFile) + string(1, fromRank) +
                                            string(1, toFile) + string(1, toRank);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return bestMove;
}

int Game::evaluateBoard(Board& b, bool aiIsWhite) {
    if (b.isCheckmate(!aiIsWhite)) return 100000;
    if (b.isCheckmate(aiIsWhite))  return -100000;


    int score = 0, aiCenter = 0, oppCenter = 0;
    int aiDev = 0, oppDev = 0;
    bool aiCastled = false, oppCastled = false;

    const int CenterSquares[4][2] = { {3, 3}, {3, 4}, {4, 3}, {4, 4} };

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = b.getPiece(row, col);
            if (!piece) continue;

            bool isWhite = piece->isWhitePiece();
            bool isAI = (isWhite == aiIsWhite);
            char symbol = toupper(piece->getSymbol());

            int base = 0, pst = 0;
            switch (symbol) {
            case 'P': base = 100; pst = isWhite ? pawnTable[row][col] : pawnTable[7 - row][col]; break;
            case 'N': base = 300; pst = isWhite ? knightTable[row][col] : knightTable[7 - row][col]; break;
            case 'B': base = 310; pst = isWhite ? bishopTable[row][col] : bishopTable[7 - row][col]; break;
            case 'R': base = 500; pst = isWhite ? rookTable[row][col] : rookTable[7 - row][col]; break;
            case 'Q': base = 900; pst = isWhite ? queenTable[row][col] : queenTable[7 - row][col]; break;
            case 'K':
                base = 10000; pst = isWhite ? kingTable[row][col] : kingTable[7 - row][col];
                if ((row == 0 || row == 7) && (col == 2 || col == 6)) {
                    isAI ? aiCastled = true : oppCastled = true;
                }
                break;
            }

            int value = base + pst;
            score += isAI ? value : -value;

            for (const auto& sq : CenterSquares) {
                if (piece->isValidMove(row, col, sq[0], sq[1], &b)) {
                    isAI ? aiCenter++ : oppCenter++;
                }
            }

            if (symbol != 'P' && symbol != 'K') {
                if ((isWhite && row < 7) || (!isWhite && row > 0))
                    isAI ? aiDev++ : oppDev++;
            }
        }
    }

    score += (aiCenter - oppCenter) * 15;
    score += (aiDev - oppDev) * 5;
    score += (b.isInCheck(!aiIsWhite) ? 25 : 0);
    if (aiCastled) score += 40;
    if (oppCastled) score -= 40;

    return score;
}


void Game::AmbiguityCheck(Board& board, bool isWhiteTurn, int fromRow, int fromCol, int toRow, int toCol)
{
    Piece* piece = board.getPiece(fromRow, fromCol);
    if (!piece) {
        return;
    }
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((row == fromRow && col == fromCol) || (board.getPiece(row, col) == nullptr)) {
                continue;
            }
            Piece* other = board.getPiece(row, col);
            if (other->getSymbol() == piece->getSymbol() && other->isWhitePiece() == isWhiteTurn) {
                if (other->isValidMove(row, col, toRow, toCol, &board)) {
                    if (col != fromCol) {
                        FileCheck = true;
                        return;
                    }
                    else if (row != fromRow) {
                        RankCheck = true;
                        return;
                    }
                    else if (row != fromRow && col != fromCol) {
                        FileCheck = true;
                        RankCheck = true;
                        return;
                    }
                }
            }
        }
    }
}


int Game::minimax(Board& b, int depth, int alpha, int beta, bool maximizing, bool aiIsWhite) {
    minimaxNodeCount++;
    if (depth == 0 || b.isCheckmate(true) || b.isCheckmate(false)) {
        return evaluateBoard(b, aiIsWhite);
    }

    if (maximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (int fromRow = 0; fromRow < 8; ++fromRow) {
            for (int fromCol = 0; fromCol < 8; ++fromCol) {
                Piece* piece = b.getPiece(fromRow, fromCol);
                if (piece && piece->isWhitePiece() == aiIsWhite) {
                    for (int toRow = 0; toRow < 8; ++toRow) {
                        for (int toCol = 0; toCol < 8; ++toCol) {
                            if (piece->isValidMove(fromRow, fromCol, toRow, toCol, &b)) {
                                Board copy = b;
                                if (copy.movePiece(fromRow, fromCol, toRow, toCol, aiIsWhite)) {
                                    int eval = minimax(copy, depth - 1, alpha, beta, false, aiIsWhite);
                                    maxEval = std::max(maxEval, eval);
                                    alpha = std::max(alpha, eval);
                                    if (beta <= alpha) return maxEval;
                                }
                            }
                        }
                    }
                }
            }
        }
        return maxEval;
    }
    else {
        int minEval = std::numeric_limits<int>::max();
        for (int fromRow = 0; fromRow < 8; ++fromRow) {
            for (int fromCol = 0; fromCol < 8; ++fromCol) {
                Piece* piece = b.getPiece(fromRow, fromCol);
                if (piece && piece->isWhitePiece() != aiIsWhite) {
                    for (int toRow = 0; toRow < 8; ++toRow) {
                        for (int toCol = 0; toCol < 8; ++toCol) {
                            if (piece->isValidMove(fromRow, fromCol, toRow, toCol, &b)) {
                                Board copy = b;
                                if (copy.movePiece(fromRow, fromCol, toRow, toCol, !aiIsWhite)) {
                                    int eval = minimax(copy, depth - 1, alpha, beta, true, aiIsWhite);
                                    minEval = std::min(minEval, eval);
                                    beta = std::min(beta, eval);
                                    if (beta <= alpha) return minEval;
                                }
                            }
                        }
                    }
                }
            }
        }
        return minEval;
    }
}


void Game::start() {
    std::string mode;
    std::cout << "Choose mode (1 = Multiplayer, 2 = Vs AI): ";
    std::getline(std::cin, mode);
    bool vsAI = (mode == "2");

    bool playerIsWhite = true;
    if (vsAI) {
        std::string choice;
        std::cout << "Choose color (W = White, B = Black, R = Random): ";
        std::getline(std::cin, choice);
        if (choice == "W" || choice == "w") playerIsWhite = true;
        else if (choice == "B" || choice == "b") playerIsWhite = false;
        else playerIsWhite = (std::rand() % 2 == 0);
    }

    std::string input;
    int turns = 0;
    int moves = 0;
    while (true) {
        board.printBoard();
        if (vsAI && whiteTurn != playerIsWhite) {
            std::cout << "bird is thinking...\n";
            input = findBestMove(!playerIsWhite);
            std::cout << "bird plays: " << input << "\n";
            if (input.empty()) {
                std::cout << "bird failed to produce a move.\n";
                printPGN(pgnMoves);
                printFEN(fenMoves);
                break;
            }
        }
        else {
            std::cout << (whiteTurn ? "White" : "Black") << "'s move (e.g., E2 E4): ";
            std::getline(std::cin, input);
        }

        if (input == "exit") {
            printPGN(pgnMoves);
            printFEN(fenMoves);
            break;
        }

        int fromRow, fromCol, toRow, toCol;

        bool isKingside = (input == "O-O");
        bool isQueenside = (input == "O-O-O");

        if (isKingside) {
            fromRow = whiteTurn ? 7 : 0;
            fromCol = 4;
            toRow = fromRow;
            toCol = 6;
        }
        else if (isQueenside) {
            fromRow = whiteTurn ? 7 : 0;
            fromCol = 4;
            toRow = fromRow;
            toCol = 2;
        }
        else {
            if (input.length() != 5 || input[2] != ' ') {
                std::cout << "Invalid input format.\n";
                continue;
            }
            fromCol = std::toupper(input[0]) - 'A';
            fromRow = 8 - (input[1] - '0');
            toCol = std::toupper(input[3]) - 'A';
            toRow = 8 - (input[4] - '0');
        }

        if (fromCol < 0 || fromCol > 7 || fromRow < 0 || fromRow > 7 ||
            toCol < 0 || toCol > 7 || toRow < 0 || toRow > 7) {
            std::cout << "Invalid coordinates.\n";
            continue;
        }

        Piece* captured = board.getPiece(toRow, toCol);
        bool isPromotion = board.isPromotionMove(fromRow, fromCol, toRow, toCol, whiteTurn);
        if (!dynamic_cast<Pawn*>(board.getPiece(fromRow, fromCol))) {
            AmbiguityCheck(board, whiteTurn, fromRow, fromCol, toRow, toCol);
        }
        if (!board.movePiece(fromRow, fromCol, toRow, toCol, whiteTurn)) {
            std::cout << "Invalid move.\n";
            continue;
        }

        Piece* moved = board.getPiece(toRow, toCol); // <- FIXED dangling pointer
        std::string fromCoord = input.substr(0, 2);
        std::string toCoord = input.substr(3, 2);
        bool isEnPassant = (moved && dynamic_cast<Pawn*>(moved) && fromCol != toCol && captured == nullptr);

        if (dynamic_cast<King*>(moved)) {
            if (whiteTurn) {
                WhiteCastleKingside = false;
                WhiteCastleQueenside = false;
            }
            else {
                BlackCastleKingside = false;
                BlackCastleQueenside = false;
            }
        }
        else if (dynamic_cast<Rook*>(moved)) {
            if (whiteTurn) {
                if (fromRow == 7 && fromCol == 0) WhiteCastleKingside = false;
                if (fromRow == 7 && fromCol == 7) WhiteCastleQueenside = false;
            }
            else {
                if (fromRow == 0 && fromCol == 0) BlackCastleQueenside = false;
                if (fromRow == 0 && fromCol == 7) BlackCastleKingside = false;
            }
        }
        Move move = {
            whiteTurn ? "White" : "Black",
            fromCoord,
            toCoord,
            moved ? moved->getSymbol() : '?',
            captured != nullptr || isEnPassant,
            isEnPassant,
            isPromotion,
            'Q',
            board.isInCheck(!whiteTurn),
            board.isCheckmate(!whiteTurn),
            isKingside,
            isQueenside,
            RankCheck,
            FileCheck
        };
        moves++;
        if (!move.isCapture && !dynamic_cast<Pawn*>(moved)) ++turns;
        else turns = 0;

        moveHistory.push_back(move);
        pgnMoves.push_back(move.toPGN());
        board.setLastMove(move);
        std::string current = board.getSimplePosition(whiteTurn);
        int count = 0;
        for (const auto& pos : previousPositions) {
            if (pos == current) ++count;
        }
        if (count == 2) {
            std::cout << "\nDraw by threefold repetition.\n";
            printPGN(pgnMoves);
            printFEN(fenMoves);
            break;
        }

        previousPositions.push_back(current);
        if (turns == 100) {
            std::cout << "\nDraw by 50-move rule.\n";
            printPGN(pgnMoves);
            printFEN(fenMoves);
            break;
        }

        if (move.isCheckmate) {
            board.printBoard();
            std::cout << (whiteTurn ? "Black" : "White") << " is checkmated! "
                << (whiteTurn ? "White" : "Black") << " wins!\n";
            printPGN(pgnMoves);
            printFEN(fenMoves);
            break;
        }
        else if (board.isStalemate(!whiteTurn)) {
            board.printBoard();
            std::cout << "Stalemate! The game is a draw.\n";
            printPGN(pgnMoves);
            printFEN(fenMoves);
            break;
        }
        else if (board.insufficientMaterialCheck()) {
            cout << "Draw due to insufficient material";
            printPGN(pgnMoves);
            printFEN(fenMoves);
            break;
        }
        else if (move.isCheck) {
            std::cout << (whiteTurn ? "Black" : "White") << " is in check!\n";
        }

        whiteTurn = !whiteTurn;
        string fen = board.generateFEN(whiteTurn, WhiteCastleKingside, WhiteCastleQueenside, BlackCastleKingside, BlackCastleQueenside, turns, moves);
        cout << endl << fen << endl;
        fenMoves.push_back(fen);
    }
}