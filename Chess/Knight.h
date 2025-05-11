#pragma once
#include "Piece.h"

class Knight : public Piece {
public:
    Knight(bool isWhite);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) override;
    char getSymbol() const override;
    Piece* clone() const override { return new Knight(isWhite); }  // For example
};
