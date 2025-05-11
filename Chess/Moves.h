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
        char fromFile = tolower(fromCoord[0]);
        string toSquare = string(1, tolower(toCoord[0])) + toCoord[1];

        // Promotion (with or without capture)
        if (isPromotion) {
            if (isCapture) {
                pgn += fromFile;
                pgn += 'x';
            }
            pgn += toSquare;
            pgn += '=';
            pgn += toupper(promotionTo);

            if (isCheckmate) pgn += '#';
            else if (isCheck) pgn += '+';

            return pgn;
        }

        // Regular move
        if (pieceChar != 'P') pgn += pieceChar;

        if (isCapture) {
            if (pieceChar == 'P') pgn += fromFile;
            pgn += 'x';
        }

        pgn += toSquare;

        if (isCheckmate) pgn += '#';
        else if (isCheck) pgn += '+';

        return pgn;
    }

};