#pragma once
#include "Piece.h"
#include "Moves.h"
#include "King.h"
using namespace std;
class Board {
private:
    Piece* squares[8][8];
    bool shouldDeletePieces;
    Move lastMove;
    bool hasLastMove = false;
    string enPassantTarget = "-";
    bool whiteCanCastleKingside = true;
    bool whiteCanCastleQueenside = true;
    bool blackCanCastleKingside = true;
    bool blackCanCastleQueenside = true;

public:
    Board();
    ~Board();
    Board(const Board& other);
    Board& operator=(const Board& other);

    void initialize();
    void printBoard();

    Piece* getPiece(int row, int col) const;

    void setPiece(int col, int row, Piece* piece); //  for king safety simulation

    bool movePiece(int fromRow, int fromCol, int toRow, int toCol, bool isWhiteTurn);

    void setLastMove(const Move& move);  //  declared properly

    Move getLastMove() const {
        return lastMove;
    }

    bool hasEnPassant() const {
        return hasLastMove;
    }
    bool isPromotionMove(int fromRow, int fromCol, int toRow, int toCol, bool isWhiteTurn) const;
    bool isInCheck(bool isWhiteKing);
    bool isCheckmate(bool isWhiteKing);
    bool isStalemate(bool isWhiteKing);
    string getSimplePosition(bool whiteTurn) const;
    bool insufficientMaterialCheck();
    string generateFEN(bool whiteTurn, bool WhiteCastleKingside, bool WhiteCastleQueenside, bool BlackCastleKingside, bool BlackCastleQueenside, int turns, int moves) const;
    // Add getter methods for castling rights
    bool getWhiteCanCastleKingside() const { return whiteCanCastleKingside; }
    bool getWhiteCanCastleQueenside() const { return whiteCanCastleQueenside; }
    bool getBlackCanCastleKingside() const { return blackCanCastleKingside; }
    bool getBlackCanCastleQueenside() const { return blackCanCastleQueenside; }

    // Add setter methods (call these when king or rooks move)
    void setWhiteCanCastleKingside(bool canCastle) { whiteCanCastleKingside = canCastle; }
    void setWhiteCanCastleQueenside(bool canCastle) { whiteCanCastleQueenside = canCastle; }
    void setBlackCanCastleKingside(bool canCastle) { blackCanCastleKingside = canCastle; }
    void setBlackCanCastleQueenside(bool canCastle) { blackCanCastleQueenside = canCastle; }

};
