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
    std::vector<Move> moveHistory;
    std::vector<std::string> pgnMoves;
    bool canCastleKingside;
    bool canCastleQueenside;
    vector<std::string> previousPositions;
public:
    Game();
    void start();
    string findBestMove(bool isAIWhite);
    int minimax(Board& b, int depth, int alpha, int beta, bool maximizing, bool aiIsWhite);
    int evaluateBoard(Board& board, bool isWhitePerspective); 
};
