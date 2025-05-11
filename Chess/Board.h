#pragma once
#include "Piece.h"
#include "Moves.h"
#include "King.h"

class Board {
private:
    Piece* squares[8][8];
    bool shouldDeletePieces;
    Move lastMove;
    bool hasLastMove = false;

public:
    Board();
    ~Board();
    Board(const Board& other);

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
    bool isInCheck(bool isWhiteKing);
    bool isCheckmate(bool isWhiteKing);
    bool isStalemate(bool isWhiteKing);
    string getSimplePosition(bool whiteTurn) const;
    bool insufficientMaterialCheck();
};
