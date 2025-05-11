#pragma once
#include "Piece.h"

class Bishop : public Piece {
public:
    Bishop(bool isWhite);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) override;
    char getSymbol() const override;
    Piece* clone() const override { return new Bishop(isWhite); }  // For example

};
