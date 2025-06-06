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

        // Check if the destination square is adjacent to the opponent's king
        // Find the opponent's king
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                Piece* piece = board->getPiece(row, col);
                if (piece && piece->getSymbol() == (isWhite ? 'k' : 'K')) {
                    // This is the opponent's king
                    int opponentKingRowDiff = abs(row - toRow);
                    int opponentKingColDiff = abs(col - toCol);

                    // If our king would be adjacent to opponent's king, it's illegal
                    if (opponentKingRowDiff <= 1 && opponentKingColDiff <= 1 &&
                        !(opponentKingRowDiff == 0 && opponentKingColDiff == 0)) {
                        return false;
                    }
                    break;
                }
            }
        }

        // Create temporary board to check if our king would be in check
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
                    !board->isInCheck(isWhite) &&
                    board->getWhiteCanCastleKingside();
            }
            // Queenside castling
            if (toCol == 2) {
                return board->getPiece(7, 1) == nullptr &&
                    board->getPiece(7, 2) == nullptr &&
                    board->getPiece(7, 3) == nullptr &&
                    board->getPiece(7, 0) &&
                    board->getPiece(7, 0)->getSymbol() == 'R' &&
                    !board->isInCheck(isWhite) &&
                    board->getWhiteCanCastleQueenside();
            }
        }
        else if (!isWhite && fromRow == 0 && fromCol == 4) {
            if (toCol == 6) {
                return board->getPiece(0, 5) == nullptr &&
                    board->getPiece(0, 6) == nullptr &&
                    board->getPiece(0, 7) &&
                    board->getPiece(0, 7)->getSymbol() == 'r' &&
                    !board->isInCheck(isWhite) &&
                    board->getBlackCanCastleKingside();
            }
            if (toCol == 2) {
                return board->getPiece(0, 1) == nullptr &&
                    board->getPiece(0, 2) == nullptr &&
                    board->getPiece(0, 3) == nullptr &&
                    board->getPiece(0, 0) &&
                    board->getPiece(0, 0)->getSymbol() == 'r' &&
                    !board->isInCheck(isWhite) &&
                    board->getBlackCanCastleQueenside();
            }
        }
    }
    return false;
}