#include "King.h"
#include "Board.h"
#include <cmath>
King::King(bool isWhite) : Piece(isWhite) {
    symbol = isWhite ? 'K' : 'k';
}
char King::getSymbol() const {
    return isWhite ? 'K' : 'k';
}
bool King::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) {
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);

    // Normal king move
    if (rowDiff <= 1 && colDiff <= 1) {
        Piece* dest = board->getPiece(toRow, toCol);
        if (dest && dest->isWhitePiece() == isWhite) return false;

        Board temp = *board;
        temp.setPiece(toRow, toCol, temp.getPiece(fromRow, fromCol));
        temp.setPiece(fromRow, fromCol, nullptr);
        return !temp.isInCheck(isWhite);
    }

    // Castling
    if (rowDiff == 0 && colDiff == 2) {
        if (isWhite && fromRow == 7 && fromCol == 4) {
            // Kingside castling
            if (toCol == 6) {
                return board->getPiece(7, 5) == nullptr &&
                    board->getPiece(7, 6) == nullptr &&
                    board->getPiece(7, 7) &&
                    board->getPiece(7, 7)->getSymbol() == 'R' &&
                    !board->isInCheck(isWhite);
            }
            // Queenside castling
            if (toCol == 2) {
                return board->getPiece(7, 1) == nullptr &&
                    board->getPiece(7, 2) == nullptr &&
                    board->getPiece(7, 3) == nullptr &&
                    board->getPiece(7, 0) &&
                    board->getPiece(7, 0)->getSymbol() == 'R' &&
                    !board->isInCheck(isWhite);
            }
        }
        else if (!isWhite && fromRow == 0 && fromCol == 4) {
            if (toCol == 6) {
                return board->getPiece(0, 5) == nullptr &&
                    board->getPiece(0, 6) == nullptr &&
                    board->getPiece(0, 7) &&
                    board->getPiece(0, 7)->getSymbol() == 'r' &&
                    !board->isInCheck(isWhite);
            }
            if (toCol == 2) {
                return board->getPiece(0, 1) == nullptr &&
                    board->getPiece(0, 2) == nullptr &&
                    board->getPiece(0, 3) == nullptr &&
                    board->getPiece(0, 0) &&
                    board->getPiece(0, 0)->getSymbol() == 'r' &&
                    !board->isInCheck(isWhite);
            }
        }
    }

    return false;
}

