#pragma once
#include "Piece.h"

class Queen : public Piece {
public:
    Queen(bool isWhite);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) override;
    char getSymbol() const override;
    Piece* clone() const override { return new Queen(isWhite); }  // For example
};
