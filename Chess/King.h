#pragma once
#include "Piece.h"

class King : public Piece {
public:
    King(bool isWhite);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) override;
    char getSymbol() const override;
    Piece* clone() const override { return new King(isWhite); }  // For example
};
