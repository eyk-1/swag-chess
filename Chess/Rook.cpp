#include "Rook.h"
#include "Board.h"
Rook::Rook(bool isWhite) : Piece(isWhite) {}

char Rook::getSymbol() const {
    return isWhite ? 'R' : 'r';
}

bool Rook::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) {
    if (fromRow != toRow && fromCol != toCol)
        return false;

    int rowStep = (toRow > fromRow) ? 1 : (toRow < fromRow ? -1 : 0);
    int colStep = (toCol > fromCol) ? 1 : (toCol < fromCol ? -1 : 0);
    int row = fromRow + rowStep;
    int col = fromCol + colStep;

    // The while loop checks that all intermediate squares are empty
    while (row != toRow || col != toCol) {
        if (board->getPiece(row, col))
            return false;
        row += rowStep;
        col += colStep;
    }

    Piece* target = board->getPiece(toRow, toCol);
    return !target || target->isWhitePiece() != isWhite;
}
