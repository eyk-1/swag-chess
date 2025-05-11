#include "Queen.h"
#include "Board.h"
#include "Rook.h"
#include "Bishop.h"
Queen::Queen(bool isWhite) : Piece(isWhite) {}

char Queen::getSymbol() const {
    return isWhite ? 'Q' : 'q';
}

bool Queen::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) {
    Rook rook(isWhite);
    Bishop bishop(isWhite);
    return rook.isValidMove(fromRow, fromCol, toRow, toCol, board) ||
        bishop.isValidMove(fromRow, fromCol, toRow, toCol, board);
}
