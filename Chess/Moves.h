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
    bool RankUnclear = false;
    bool FileUnclear = false;

    string toPGN() const {
        if (isKingsideCastle) return "O-O";
        if (isQueensideCastle) return "O-O-O";

        string pgn;
        char pieceChar = toupper(pieceSymbol);
        char fromFile = tolower(fromCoord[0]);
        char fromRank = fromCoord[1];
        string toSquare = string(1, tolower(toCoord[0])) + toCoord[1];

        // Promotion
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

        // Piece moves (non-pawn)
        if (pieceChar != 'P') {
            pgn += pieceChar;

            // Handle disambiguation
            if (FileUnclear && RankUnclear) {
                pgn += fromFile;
                pgn += fromRank;
            }
            else if (FileUnclear) {
                pgn += fromFile;
            }
            else if (RankUnclear) {
                pgn += fromRank;
            }
        }

        // Captures
        if (isCapture) {
            if (pieceChar == 'P') {
                pgn += fromFile;
            }
            pgn += 'x';
        }

        pgn += toSquare;

        // Check or checkmate
        if (isCheckmate) pgn += '#';
        else if (isCheck) pgn += '+';

        return pgn;
    }


};