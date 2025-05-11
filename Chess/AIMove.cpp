#pragma once
#include "Board.h"
struct AIMove {
    int fromRow, fromCol;
    int toRow, toCol;
    int score;
    int evaluateBoard(const Board& board) {
        int score = 0;
        for (int row = 0; row < 8; ++row)
            for (int col = 0; col < 8; ++col) {
                Piece* p = board.getPiece(row, col);
                if (!p) continue;
                int val = 0;
                switch (tolower(p->getSymbol())) {
                case 'p': val = 10; break;
                case 'n': case 'b': val = 30; break;
                case 'r': val = 50; break;
                case 'q': val = 90; break;
                case 'k': val = 900; break;
                }
                score += (p->isWhitePiece() ? val : -val);
            }
        return score;
    }

};
