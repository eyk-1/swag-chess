    #include "Board.h"
    #include "Pawn.h"
    #include "Rook.h"
    #include "Knight.h"
    #include "Bishop.h"
    #include "Queen.h"
    #include "King.h"
    #include <iostream>
    #include <sstream>

using namespace std;
    Board::Board() : shouldDeletePieces(true) {
        initialize();
    }

    Board::~Board() {
        if (shouldDeletePieces) {
            for (int row = 0; row < 8; ++row)
                for (int col = 0; col < 8; ++col)
                    delete squares[row][col];
        }
    }

    Board::Board(const Board& other) :
        shouldDeletePieces(true),
        hasLastMove(other.hasLastMove),
        lastMove(other.lastMove),
        enPassantTarget(other.enPassantTarget),
        whiteCanCastleKingside(other.whiteCanCastleKingside),
        whiteCanCastleQueenside(other.whiteCanCastleQueenside),
        blackCanCastleKingside(other.blackCanCastleKingside),
        blackCanCastleQueenside(other.blackCanCastleQueenside) {

        // Initialize all squares to nullptr
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                squares[row][col] = nullptr;
            }
        }

        // Then copy the pieces
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (other.squares[row][col] != nullptr) {
                    squares[row][col] = other.squares[row][col]->clone();
                }
            }
        }
    }

    Board& Board::operator=(const Board& other) {
        if (this != &other) {
            // Clean up existing pieces
            if (shouldDeletePieces) {
                for (int row = 0; row < 8; ++row) {
                    for (int col = 0; col < 8; ++col) {
                        delete squares[row][col];
                        squares[row][col] = nullptr;
                    }
                }
            }

            // Copy new pieces
            for (int row = 0; row < 8; ++row) {
                for (int col = 0; col < 8; ++col) {
                    if (other.squares[row][col] != nullptr) {
                        squares[row][col] = other.squares[row][col]->clone();
                    }
                    else {
                        squares[row][col] = nullptr;
                    }
                }
            }

            // Copy other members
            hasLastMove = other.hasLastMove;
            lastMove = other.lastMove;
            enPassantTarget = other.enPassantTarget;
            whiteCanCastleKingside = other.whiteCanCastleKingside;
            whiteCanCastleQueenside = other.whiteCanCastleQueenside;
            blackCanCastleKingside = other.blackCanCastleKingside;
            blackCanCastleQueenside = other.blackCanCastleQueenside;
            shouldDeletePieces = true;
        }
        return *this;
    }


    void Board::initialize() {
        for (int row = 0; row < 8; ++row)
            for (int col = 0; col < 8; ++col)
                squares[row][col] = nullptr;

        // Black pieces
        squares[0][0] = new Rook(false);
        squares[0][1] = new Knight(false);
        squares[0][2] = new Bishop(false);
        squares[0][3] = new Queen(false);
        squares[0][4] = new King(false);
        squares[0][5] = new Bishop(false);
        squares[0][6] = new Knight(false);
        squares[0][7] = new Rook(false);
        for (int col = 0; col < 8; ++col)
           squares[1][col] = new Pawn(false);

        // White pieces
        squares[7][0] = new Rook(true);
        squares[7][1] = new Knight(true);
        squares[7][2] = new Bishop(true);
        squares[7][3] = new Queen(true);
        squares[7][4] = new King(true);
        squares[7][5] = new Bishop(true);
        squares[7][6] = new Knight(true);
        squares[7][7] = new Rook(true);
         for (int col = 0; col < 8; ++col)
            squares[6][col] = new Pawn(true);
    }

    void Board::printBoard() {
        std::cout << "\n    A   B   C   D   E   F   G   H\n";
        std::cout << "  +---+---+---+---+---+---+---+---+\n";
        for (int row = 0; row < 8; ++row) {
            std::cout << 8 - row << " |";
            for (int col = 0; col < 8; ++col) {
                Piece* piece = squares[row][col];
                if (piece)
                    std::cout << ' ' << piece->getSymbol() << " |";
                else
                    std::cout << "   |";
            }
            std::cout << ' ' << 8 - row << "\n";
            std::cout << "  +---+---+---+---+---+---+---+---+\n";
        }
        std::cout << "    A   B   C   D   E   F   G   H\n\n";
    }


    Piece* Board::getPiece(int row, int col) const {
        return squares[row][col];
    }


    void Board::setPiece(int row, int col, Piece* piece) {
        squares[row][col] = piece;
    }

    bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol, bool isWhiteTurn) {
        Piece* piece = squares[fromRow][fromCol];
        // Simulate for pin
        Piece* tempFrom = squares[fromRow][fromCol];
        Piece* tempTo = squares[toRow][toCol];
        squares[toRow][toCol] = tempFrom;
        squares[fromRow][fromCol] = nullptr;

        bool exposesKing = isInCheck(isWhiteTurn);
        squares[fromRow][fromCol] = tempFrom;
        squares[toRow][toCol] = tempTo;

        if (exposesKing || !piece || piece->isWhitePiece() != isWhiteTurn)
            return false;
        Piece* destPiece = squares[toRow][toCol];

        // Castling
        if (dynamic_cast<King*>(piece)) {
            bool kingInCheck = isInCheck(isWhiteTurn);

            if (!kingInCheck) {
                if (isWhiteTurn && fromRow == 7 && fromCol == 4) {
                    // White kingside castling
                    if (toRow == 7 && toCol == 6 && whiteCanCastleKingside &&
                        squares[7][5] == nullptr && squares[7][6] == nullptr &&
                        dynamic_cast<Rook*>(squares[7][7])) {

                        squares[7][6] = piece;
                        squares[7][4] = nullptr;
                        squares[7][5] = squares[7][7];
                        squares[7][7] = nullptr;

                        // Update castling rights
                        whiteCanCastleKingside = false;
                        whiteCanCastleQueenside = false;
                        return true;
                    }
                    // White queenside castling
                    if (toRow == 7 && toCol == 2 && whiteCanCastleQueenside &&
                        squares[7][1] == nullptr && squares[7][2] == nullptr &&
                        squares[7][3] == nullptr && dynamic_cast<Rook*>(squares[7][0])) {

                        squares[7][2] = piece;
                        squares[7][4] = nullptr;
                        squares[7][3] = squares[7][0];
                        squares[7][0] = nullptr;

                        // Update castling rights
                        whiteCanCastleKingside = false;
                        whiteCanCastleQueenside = false;
                        return true;
                    }
                }
                else if (!isWhiteTurn && fromRow == 0 && fromCol == 4) {
                    // Black kingside castling
                    if (toRow == 0 && toCol == 6 && blackCanCastleKingside &&
                        squares[0][5] == nullptr && squares[0][6] == nullptr &&
                        dynamic_cast<Rook*>(squares[0][7])) {

                        squares[0][6] = piece;
                        squares[0][4] = nullptr;
                        squares[0][5] = squares[0][7];
                        squares[0][7] = nullptr;

                        // Update castling rights
                        blackCanCastleKingside = false;
                        blackCanCastleQueenside = false;
                        return true;
                    }
                    // Black queenside castling
                    if (toRow == 0 && toCol == 2 && blackCanCastleQueenside &&
                        squares[0][1] == nullptr && squares[0][2] == nullptr &&
                        squares[0][3] == nullptr && dynamic_cast<Rook*>(squares[0][0])) {

                        squares[0][2] = piece;
                        squares[0][4] = nullptr;
                        squares[0][3] = squares[0][0];
                        squares[0][0] = nullptr;

                        // Update castling rights
                        blackCanCastleKingside = false;
                        blackCanCastleQueenside = false;
                        return true;
                    }
                }
            }
        }

        // Validate move legality
        if (!piece->isValidMove(fromRow, fromCol, toRow, toCol, this))
            return false;

        // No friendly fire
        if (destPiece && destPiece->isWhitePiece() == isWhiteTurn)
            return false;

        // En passant
        if (dynamic_cast<Pawn*>(piece) && fromCol != toCol && destPiece == nullptr) {
            delete squares[fromRow][toCol];
            squares[fromRow][toCol] = nullptr;
        }
        // FEN string - En passant
        if (dynamic_cast <Pawn*>(piece)) {
            int direction = isWhiteTurn ? -1 : 1;  // White moves up (row+1), black moves down (row-1)
            int startRow = isWhiteTurn ? 6 : 1;    // Starting row (2 for white, 7 for black)
            if (fromCol == toCol && fromRow == startRow && toRow == fromRow + 2 * direction && getPiece(fromRow + direction, fromCol) == nullptr && getPiece(toRow, toCol) == nullptr) {
                char file = 'a' + fromCol;
                int rank = isWhiteTurn ? 3 : 6;  // target square row in algebraic (i.e. e3 or e6)

                enPassantTarget = string(1, file) + to_string(rank);

            }
        }
        else {
            enPassantTarget = "-";
        }

        // Promotion
        if (dynamic_cast<Pawn*>(piece)) {
            if ((isWhiteTurn && toRow == 0) || (!isWhiteTurn && toRow == 7)) {
                delete squares[toRow][toCol];
                squares[toRow][toCol] = new Queen(isWhiteTurn);
                squares[fromRow][fromCol] = nullptr;
                delete piece;
                return true;
            }
        }

        // Normal move
        delete squares[toRow][toCol];
        squares[toRow][toCol] = piece;
        squares[fromRow][fromCol] = nullptr;
        updateCastlingRights(fromRow, fromCol, toRow, toCol);
        return true;
    }

    bool Board::isPromotionMove(int fromRow, int fromCol, int toRow, int toCol, bool isWhiteTurn) const
    {
        Piece* piece = squares[fromRow][fromCol];
        if (dynamic_cast<Pawn*>(piece)) {
            if ((isWhiteTurn && toRow == 0) || (!isWhiteTurn && toRow == 7)) {
                return true;
            }
        }
        return false;
    }

    bool Board::isInCheck(bool isWhiteKing) {
        int kingRow = -1, kingCol = -1;

        for (int row = 0; row < 8; ++row)
            for (int col = 0; col < 8; ++col)
                if (auto* piece = squares[row][col])
                    if (dynamic_cast<King*>(piece) && piece->isWhitePiece() == isWhiteKing) {
                        kingRow = row;
                        kingCol = col;
                    }

        if (kingRow == -1 || kingCol == -1) return false;

        for (int row = 0; row < 8; ++row)
            for (int col = 0; col < 8; ++col)
                if (auto* attacker = squares[row][col])
                    if (attacker->isWhitePiece() != isWhiteKing)
                        if (attacker->isValidMove(row, col, kingRow, kingCol, this))
                            return true;

        return false;
    }

    bool Board::isCheckmate(bool isWhiteKing) {
        if (!isInCheck(isWhiteKing)) return false;

        // Check all possible moves for this color
        for (int fromRow = 0; fromRow < 8; ++fromRow) {
            for (int fromCol = 0; fromCol < 8; ++fromCol) {
                Piece* piece = squares[fromRow][fromCol];
                if (piece && piece->isWhitePiece() == isWhiteKing) {
                    for (int toRow = 0; toRow < 8; ++toRow) {
                        for (int toCol = 0; toCol < 8; ++toCol) {
                            if (piece->isValidMove(fromRow, fromCol, toRow, toCol, this)) {
                                Board copy = *this;
                                if (copy.movePiece(fromRow, fromCol, toRow, toCol, isWhiteKing)) {
                                    if (!copy.isInCheck(isWhiteKing)) {
                                        return false; // Found a legal move
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return true; // No legal moves found
    }





    bool Board::isStalemate(bool isWhiteTurn) {
        if (isInCheck(isWhiteTurn)) return false; // Not stalemate if in check
        // Check all possible moves
        for (int fromRow = 0; fromRow < 8; ++fromRow) {
            for (int fromCol = 0; fromCol < 8; ++fromCol) {
                Piece* piece = squares[fromRow][fromCol];
                if (piece && piece->isWhitePiece() == isWhiteTurn) {
                    for (int toRow = 0; toRow < 8; ++toRow) {
                        for (int toCol = 0; toCol < 8; ++toCol) {
                            if (piece->isValidMove(fromRow, fromCol, toRow, toCol, this)) {
                                Board copy = *this;
                                if (copy.movePiece(fromRow, fromCol, toRow, toCol, isWhiteTurn)) {
                                    if (!copy.isInCheck(isWhiteTurn)) {
                                        return false; // Found a legal move
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return true; // No legal moves found
    }

    bool Board::insufficientMaterialCheck() {
        int piececount = 0;
        for (int row = 0; row < 8; row++) { // Check if sufficient material is on the board.
            for (int col = 0; col < 8; col++) {
                Piece* piece = squares[row][col];
                if (dynamic_cast<King*> (piece) || dynamic_cast<Knight*> (piece) || dynamic_cast <Bishop*> (piece)) {
                    piececount++;
                }
                else if (piece) {
                    piececount = piececount + 10;
                }
            }
        }
        if (piececount < 5) {
            return true;
        }
        else {
            return false;
        }
    }

    void Board::setLastMove(const Move& move) {
        lastMove = move;
        hasLastMove = true;
    }
    string Board::generateFEN(bool whiteTurn, bool WhiteCastleKingside, bool WhiteCastleQueenside,
        bool BlackCastleKingside, bool BlackCastleQueenside, int turns, int moves) const {
        string fen;

        // 1. Piece Placement
        for (int row = 0; row < 8; ++row) {
            int emptyCount = 0;
            for (int col = 0; col < 8; ++col) {
                Piece* piece = squares[row][col];
                if (piece) {
                    if (emptyCount > 0) {
                        fen += std::to_string(emptyCount);
                        emptyCount = 0;
                    }
                    fen += piece->getSymbol();
                }
                else {
                    ++emptyCount;
                }
            }
            if (emptyCount > 0) {
                fen += std::to_string(emptyCount);
            }
            if (row != 7) {
                fen += '/';
            }
        }

        // 2. Active Color
        fen += (!whiteTurn ? " w " : " b ");

        // 3. Castling Availability - Use the actual castling rights from the board
        std::string castling;
        if (whiteCanCastleKingside) castling += 'K';
        if (whiteCanCastleQueenside) castling += 'Q';
        if (blackCanCastleKingside) castling += 'k';
        if (blackCanCastleQueenside) castling += 'q';
        fen += (castling.empty() ? "- " : castling + " ");

        // 4. En Passant Target Square
        fen += enPassantTarget.empty() ? "- " : enPassantTarget + " ";

        // 5. Halfmove Clock
        fen += std::to_string(turns) + " ";

        // 6. Fullmove Number
        fen += std::to_string(moves / 2);

        return fen;
    }

    std::string Board::getSimplePosition(bool isWhiteTurn) const {
        std::ostringstream ss;

        // 1. Piece positions
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                Piece* piece = squares[row][col];
                if (piece)
                    ss << piece->getSymbol();
                else
                    ss << '.';
            }
        }

        // 2. Turn info
        ss << (isWhiteTurn ? "w" : "b");

        // 3. En passant target square
        ss << "|" << enPassantTarget;

        // 4. Castling rights using the actual member variables
        ss << "|";
        if (whiteCanCastleKingside) ss << "K";
        if (whiteCanCastleQueenside) ss << "Q";
        if (blackCanCastleKingside) ss << "k";
        if (blackCanCastleQueenside) ss << "q";
        if (!whiteCanCastleKingside && !whiteCanCastleQueenside &&
            !blackCanCastleKingside && !blackCanCastleQueenside) {
            ss << "-";
        }

        return ss.str();
    }

    void Board::updateCastlingRights(int fromRow, int fromCol, int toRow, int toCol) {
        // Check if king moved
        if (dynamic_cast<King*>(squares[toRow][toCol])) {
            if (squares[toRow][toCol]->isWhitePiece()) {
                whiteCanCastleKingside = false;
                whiteCanCastleQueenside = false;
            }
            else {
                blackCanCastleKingside = false;
                blackCanCastleQueenside = false;
            }
        }

        // Check if rook moved from starting position
        if (dynamic_cast<Rook*>(squares[toRow][toCol])) {
            // White rooks
            if (fromRow == 7 && fromCol == 0) { // Queenside white rook
                whiteCanCastleQueenside = false;
            }
            else if (fromRow == 7 && fromCol == 7) { // Kingside white rook
                whiteCanCastleKingside = false;
            }
            // Black rooks
            else if (fromRow == 0 && fromCol == 0) { // Queenside black rook
                blackCanCastleQueenside = false;
            }
            else if (fromRow == 0 && fromCol == 7) { // Kingside black rook
                blackCanCastleKingside = false;
            }
        }

        // Check if rook was captured
        if (toRow == 7 && toCol == 0) { // White queenside rook captured
            whiteCanCastleQueenside = false;
        }
        else if (toRow == 7 && toCol == 7) { // White kingside rook captured
            whiteCanCastleKingside = false;
        }
        else if (toRow == 0 && toCol == 0) { // Black queenside rook captured
            blackCanCastleQueenside = false;
        }
        else if (toRow == 0 && toCol == 7) { // Black kingside rook captured
            blackCanCastleKingside = false;
        }
    }
