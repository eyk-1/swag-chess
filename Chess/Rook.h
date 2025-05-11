#pragma once
#include "Piece.h"

class Rook : public Piece {
public:
    Rook(bool isWhite);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) override;
    char getSymbol() const override;
    Piece* clone() const override { return new Rook(isWhite); }  // For example
};
