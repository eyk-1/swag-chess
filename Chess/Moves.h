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

        // Piece identifier (except pawns)
        if (pieceChar != 'P') pgn += pieceChar;

        // Capture indicator
        if (isCapture) {
            if (pieceChar == 'P') pgn += fromCoord[0]; // File for pawn captures
            pgn += 'x';
        }

        // Destination square (just the toCoord)
        pgn += toCoord;

        // Promotion
        if (isPromotion) {
            pgn += '=';
            pgn += toupper(promotionTo);
        }

        // Check/Checkmate
        if (isCheckmate) pgn += '#';
        else if (isCheck) pgn += '+';

        return pgn;
    }
};