#include "Knight.h"
#include "Board.h"
Knight::Knight(bool isWhite) : Piece(isWhite) {}

char Knight::getSymbol() const {
    return isWhite ? 'N' : 'n';
}

bool Knight::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) {
    int dRow = abs(fromRow - toRow);
    int dCol = abs(fromCol - toCol);

    if ((dRow == 2 && dCol == 1) || (dRow == 1 && dCol == 2)) {
        Piece* target = board->getPiece(toRow, toCol);
        return !target || target->isWhitePiece() != isWhite;
    }
    return false;
}
