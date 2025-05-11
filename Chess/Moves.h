#pragma once
#include <string>
#include "Piece.h"
using namespace std;

struct Move {
    string player;
    string fromCoord;
    string toCoord;
    char pieceSymbol;
    bool isCapture = false;
    bool isEnPassant = false;
    bool isPromotion = false;
    char promotionTo = 'Q';
    bool isCheck = false;
    bool isCheckmate = false;
    bool isKingsideCastle = false;
    bool isQueensideCastle = false;

    string toPGN() const {
        if (isKingsideCastle) return "O-O";
        if (isQueensideCastle) return "O-O-O";

        string pgn;
        char pieceChar = toupper(pieceSymbol);

        // For promotions, always show pawn->queen (even if symbol is 'Q')
        if (isPromotion) {
            pgn += toCoord + "=Q"; // e.g., "e8=Q"
            if (isCheckmate) pgn += '#';
            else if (isCheck) pgn += '+';
            return pgn;
        }

        // Non-promotion moves
        if (pieceChar != 'P') pgn += pieceChar;
        if (isCapture) {
            if (pieceChar == 'P') pgn += fromCoord[0];
            pgn += 'x';
        }
        pgn += toCoord;
        if (isCheckmate) pgn += '#';
        else if (isCheck) pgn += '+';

        return pgn;
    }
};