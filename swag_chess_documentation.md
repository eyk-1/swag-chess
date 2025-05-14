# Swag Chess Codebase Documentation

## Overview

This document explains the Swag Chess codebase line-by-line, including all classes, structs, and their roles in the program. The game is written in C++ and includes features such as move legality checking, AI using minimax, en passant, castling, promotion, PGN output and FEN output.

## Game.h / Game.cpp

### Class Game

*Manages the full chess game lifecycle including game loop, input parsing, AI moves, and PGN history.*

#### Attributes:

- `board`: *Instance of the Board class that tracks current game state.*
- `whiteTurn`: *Boolean that is true if it's White's turn to move.*
- `moveHistory`: *A vector that stores the history of all Move objects played.*
- `pgnMoves`: *Vector that stores strings representing PGN of each move.*
- `fenMoves`: *Vector that stores strings representing FEN of each move.*
- `WhiteCastleKingside`: *Flag indicating if White can castle kingside.*
- `WhiteCastleQueenside`: *Flag indicating if White can castle queenside.*
- `BlackCastleKingside`: *Flag indicating if Black can castle kingside.*
- `BlackCastleQueenside`: *Flag indicating if Black can castle queenside.*
- `RankCheck`: *Flag for move disambiguation by rank.*
- `FileCheck`: *Flag for move disambiguation by file.*
- `previousPositions`: *Vector tracking board states for threefold repetition detection.*
- `minimaxNodeCount`: *Counter for nodes evaluated during minimax search.*

#### Methods:

- `Game()`: *Constructor for Game objects.*
- `void start()`: *Main game loop that manages turns and processes user input.*
- `string findBestMove(bool isAIWhite)`: *Selects the best move for AI using minimax algorithm.*
- `int minimax(Board& b, int depth, int alpha, int beta, bool maximizing, bool aiIsWhite)`: *Recursive decision tree for AI with alpha-beta pruning.*
- `int evaluateBoard(Board& board, bool isWhitePerspective)`: *Heuristic evaluator for AI move assessment.*
- `void AmbiguityCheck(Board& board, bool isWhite, int fromRow, int fromCol, int toRow, int toCol)`: *Detects whether PGN disambiguation is needed.*
- `void printPGN(const std::vector<std::string>& pgnMoves)`: *Outputs the game history in PGN format.*
- `void printFEN(const vector<string>& fenMoves)`: *Outputs the game history in FEN format.*

#### Main Logic:

- Handles player vs player or player vs AI gameplay.
- Moves are input as standard algebraic notation.
- Promotion is detected in `Game::start` by checking if a pawn reached the last rank.
- Promotion notation ('=Q') is appended to PGN via the Move struct.

## Moves.h

### Struct Move

*Stores information about a move for replay, PGN generation, and legality checking.*

#### Fields:

- `string player`: *Which player played the move.*
- `string fromCoord`: *Starting square in algebraic notation (e.g., "e2").*
- `string toCoord`: *Ending square in algebraic notation (e.g., "e4").*
- `char pieceSymbol`: *Character representing the piece (e.g., 'P', 'N').*
- `bool isCapture`: *True if move is a capture.*
- `bool isEnPassant`: *True if en passant capture occurred.*
- `bool isPromotion`: *True if move promotes a pawn.*
- `char promotionTo`: *Character representing promoted piece (default 'Q').*
- `bool isCheck`: *True if move puts opponent in check.*
- `bool isCheckmate`: *True if move puts opponent in checkmate.*
- `bool isKingsideCastle`: *True if move is kingside castling.*
- `bool isQueensideCastle`: *True if move is queenside castling.*
- `bool RankUnclear`: *True if move requires rank disambiguation.*
- `bool FileUnclear`: *True if move requires file disambiguation.*

#### Method:

- `string toPGN() const`: *Converts the Move object into PGN string notation.*
  - Handles pawn moves, captures, disambiguation (e.g., Nfg4, R1d3)
  - Adds symbols for check (+), checkmate (#), and promotions (=Q)

## Board.h / Board.cpp

### Class Board

*Represents the 8x8 chessboard and contains all the Piece objects.*

#### Attributes:

- `Piece* squares[8][8]`: *2D array of Piece pointers representing the board.*
- `bool shouldDeletePieces`: *Flag determining if pieces should be deleted on destruction.*
- `Move lastMove`: *Stores the latest Move played.*
- `bool hasLastMove`: *Flag indicating if a move has been made.*
- `string enPassantTarget`: *String representing square where en passant is possible.*

#### Methods:

- `Board()`: *Constructor for board objects.*
- `~Board()`: *Destructor for board objects.*
- `Board(const Board& other)`: *Copy constructor for simulations and minimax.*
- `void initialize()`: *Sets up the starting position of all pieces.*
- `void printBoard()`: *Displays the current board state.*
- `Piece* getPiece(int row, int col) const`: *Fetches a piece from the board at specified coordinates.*
- `void setPiece(int col, int row, Piece* piece)`: *Sets piece on the board, mainly for simulations.*
- `bool movePiece(int fromRow, int fromCol, int toRow, int toCol, bool isWhiteTurn)`: *Validates and applies a move (including castling, en passant, promotion).*
- `void setLastMove(const Move& move)`: *Updates the most recent move information.*
- `Move getLastMove() const`: *Returns the last move made.*
- `bool hasEnPassant() const`: *Checks if en passant is possible.*
- `bool isPromotionMove(int fromRow, int fromCol, int toRow, int toCol, bool isWhiteTurn) const`: *Determines if a move is a promotion.*
- `bool isInCheck(bool isWhiteKing)`: *Checks if the specified king is under threat.*
- `bool isCheckmate(bool isWhiteKing)`: *Checks if the specified king is in checkmate.*
- `bool isStalemate(bool isWhiteKing)`: *Checks if the specified player has no legal moves but is not in check.*
- `string getSimplePosition(bool whiteTurn) const`: *Returns a simple string representation of the board position.*
- `bool insufficientMaterialCheck()`: *Detects draw conditions due to insufficient material.*
- `string generateFEN(bool whiteTurn, bool WhiteCastleKingside, bool WhiteCastleQueenside, bool BlackCastleKingside, bool BlackCastleQueenside, int turns, int moves) const`: *Builds FEN string from current board state.*

## Piece Hierarchy

### Abstract Class Piece

*Base class for King, Queen, Rook, Bishop, Knight, Pawn.*

#### Attributes:

- `char symbol`: *Character representing the piece (e.g., 'K', 'N').*
- `bool isWhite`: *Boolean flag that is true if the piece is white.*

#### Methods:

- `Piece(bool isWhite)`: *Constructor for Piece objects.*
- `virtual ~Piece() = default`: *Virtual destructor for proper inheritance cleanup.*
- `bool isWhitePiece() const`: *Returns true if the piece is white.*
- `virtual char getSymbol() const = 0`: *Pure virtual method to fetch the piece symbol.*
- `virtual bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, Board* board) = 0`: *Pure virtual method to check if a move is legal for this piece.*
- `virtual Piece* clone() const = 0`: *Pure virtual function that returns a copy of the Piece object for simulation.*

### Derived Classes (King, Queen, etc.)

*Each piece type inherits from the Piece base class and implements its own movement logic.*

- Override `isValidMove()` to implement individual piece logic:
  - Knight uses L-shape rules
  - Rook and Bishop use ray tracing
  - Pawn handles diagonal capture, en passant, and forward pushes
  - King adds castling detection with safety checks
  - Queen combines Bishop + Rook movement

*All moves are validated before updating the board state.*

![PlantUML diagram](media/image1.png)
