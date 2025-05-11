#include "Pawn.h"
#include "Board.h"
#include "Moves.h"
#include <cmath>

Pawn::Pawn(bool isWhite) : Piece(isWhite) {
    symbol = isWhite ? 'P' : 'p';
}
char Pawn::getSymbol() const {
    return isWhite ? 'P' : 'p';
}

bool Pawn::isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) {
    int direction = isWhite ? -1 : 1;  // White moves up (row+1), black moves down (row-1)
    int startRow = isWhite ? 6 : 1;    // Starting row (2 for white, 7 for black)

    // Standard one-square forward
    if (fromCol == toCol && toRow == fromRow + direction &&
        board->getPiece(toRow, toCol) == nullptr) {
        return true;
    }

    // Initial two-square forward move
    if (fromCol == toCol && fromRow == startRow && toRow == fromRow + 2 * direction &&
        board->getPiece(fromRow + direction, fromCol) == nullptr &&
        board->getPiece(toRow, toCol) == nullptr) {
        return true;
    }

    // Diagonal capture
    if (abs(toCol - fromCol) == 1 && toRow == fromRow + direction) {
        Piece* destPiece = board->getPiece(toRow, toCol);
        if (destPiece && destPiece->isWhitePiece() != isWhite) {
            return true;
        }

        // En Passant capture
        if (board->hasEnPassant()) {
            Move last = board->getLastMove();
            char lastPiece = last.pieceSymbol;
            if ((lastPiece == 'P' && !isWhite) || (lastPiece == 'p' && isWhite)) {
                int lastFromRow = 8 - (last.fromCoord[1] - '0');
                int lastFromCol = last.fromCoord[0] - 'A';
                int lastToRow = 8 - (last.toCoord[1] - '0');
                int lastToCol = last.toCoord[0] - 'A';  

                if (abs(lastFromRow - lastToRow) == 2 &&    // Two-step move
                    lastToRow == fromRow &&                // Same rank
                    lastToCol == toCol &&                  // Target file
                    abs(lastToCol - fromCol) == 1 &&       // Adjacent file
                    board->getPiece(lastToRow, lastToCol)) { // Pawn still there
                    return true;
                }
            }
        }
    }

    return false;
}

