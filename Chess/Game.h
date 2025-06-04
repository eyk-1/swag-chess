#include <vector>
#include "Board.h"
#include "Moves.h"
#include <string>
using namespace std;
class Game {
private:
    Board board;
    bool whiteTurn;
    void printPGN(const std::vector<std::string>& pgnMoves);
    void printFEN(const vector<string>& fenMoves);
    std::vector<Move> moveHistory;
    std::vector<std::string> pgnMoves;
    vector <string> fenMoves;
    bool WhiteCastleKingside = true;
    bool WhiteCastleQueenside = true;
    bool BlackCastleKingside = true;
    bool BlackCastleQueenside = true;
    bool RankCheck = false;
    bool FileCheck = false;
    vector<std::string> previousPositions;
    int minimaxNodeCount = 0;

public:
    Game();
    void start();
    string findBestMove(bool isAIWhite);
    int minimax(Board& b, int depth, int alpha, int beta, bool maximizing, bool aiIsWhite);
    int evaluateBoard(Board& board, bool isWhitePerspective);
    void AmbiguityCheck(Board& board, bool isWhite, int fromRow, int fromCol, int toRow, int toCol);
    Board getBoard() { return board; }
    bool isWhiteTurn() { return whiteTurn; }
};
