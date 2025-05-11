#include "Bishop.h"
#include "Board.h"
Bishop::Bishop(bool isWhite) : Piece(isWhite) {}

char Bishop::getSymbol() const {
    return isWhite ? 'B' : 'b';
}

bool Bishop::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) {
    if (abs(fromRow - toRow) != abs(fromCol - toCol))
        return false;

    int rowStep = (toRow > fromRow) ? 1 : -1;
    int colStep = (toCol > fromCol) ? 1 : -1;
    int row = fromRow + rowStep;
    int col = fromCol + colStep;

    while (row != toRow && col != toCol) {
        if (board->getPiece(row, col))
            return false;
        row += rowStep;
        col += colStep;
    }

    Piece* target = board->getPiece(toRow, toCol);
    return !target || target->isWhitePiece() != isWhite;
}
