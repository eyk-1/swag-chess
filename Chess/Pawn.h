#pragma once
#include "Piece.h"

class Pawn : public Piece {
public:
    Pawn(bool isWhite);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) override;
    char getSymbol() const override;
    Piece* clone() const override { return new Pawn(isWhite); }  // For example
};
