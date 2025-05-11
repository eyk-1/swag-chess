#pragma once
#include <string>

class Board; 

class Piece {
protected:
    char symbol;
    bool isWhite;

public:
    Piece(bool isWhite) : isWhite(isWhite) {}
    virtual ~Piece() = default;

    bool isWhitePiece() const { return isWhite; }
    virtual char getSymbol() const = 0;
    virtual bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) = 0;
    virtual Piece* clone() const = 0;
};
