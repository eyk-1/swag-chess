# Swag Chess Codebase Documentation

## Overview

This document explains the Swag Chess codebase. The game is written in C++ using object-oriented programming principles and currently supports GUI mode using SFML, with console mode discontinued (maybe). 

**Key Features:**
- Player vs Player and Player vs AI gameplay modes
- **Bird Engine**: Minimax AI with alpha-beta pruning and static evaluation
- Full chess rule compliance: castling, en passant, promotion, draw conditions
- PGN (Portable Game Notation) and FEN (Forsyth-Edwards Notation) generation
- Game state tracking for threefold repetition and fifty-move rule
- Object-oriented design demonstrating inheritance, polymorphism, and encapsulation

---

## Game Class (`Game.h` / `Game.cpp`)

**Purpose:** Manages the full chess game lifecycle including game loop, input parsing, AI moves, and move history.

### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `board` | `Board` | Instance tracking current game state |
| `whiteTurn` | `bool` | True if it's White's turn to move |
| `moveHistory` | `vector<Move>` | History of all Move objects played |
| `pgnMoves` | `vector<string>` | PGN notation strings for each move |
| `fenMoves` | `vector<string>` | FEN notation strings for each move |
| `WhiteCastleKingside` | `bool` | White kingside castling rights |
| `WhiteCastleQueenside` | `bool` | White queenside castling rights |
| `BlackCastleKingside` | `bool` | Black kingside castling rights |
| `BlackCastleQueenside` | `bool` | Black queenside castling rights |
| `RankCheck` | `bool` | Flag for move disambiguation by rank |
| `FileCheck` | `bool` | Flag for move disambiguation by file |
| `previousPositions` | `vector<string>` | Board states for threefold repetition |
| `minimaxNodeCount` | `int` | Counter for minimax search nodes |

### Methods

| Method | Return Type | Description |
|--------|-------------|-------------|
| `Game()` | - | Constructor for Game objects |
| `start()` *(console only)* | `void` | Main game loop |
| `findBestMove(bool isAIWhite)` | `string` | AI move selection using minimax |
| `minimax(...)` | `int` | Recursive minimax with alpha-beta pruning |
| `evaluateBoard(...)` | `int` | Heuristic board evaluation for AI |
| `AmbiguityCheck(...)` | `void` | Detects PGN disambiguation needs |
| `printPGN(...)` *(console only)* | `void` | Outputs game history in PGN |
| `printFEN(...)` *(console only)* | `void` | Outputs game history in FEN |
| `isWhiteTurn()` | `bool` | Returns current turn state |
| `toggleTurn()` | `void` | Switches active player |
| `getBoard()` | `Board&` | Returns reference to game board |

---

## ChessGUI Class (`ChessGUI.h` / `ChessGUI.cpp`)

**Purpose:** Handles all graphical interface operations using SFML, including rendering, user input, and AI integration.

### Key Constants

```cpp
static const int BOARD_SIZE = 640;
static const int SQUARE_SIZE = 80;
static const int BOARD_OFFSET_X = 80;
static const int BOARD_OFFSET_Y = 80;
```

### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `window` | `sf::RenderWindow` | Main SFML window |
| `game` | `Game` | Core game logic instance |
| `font` | `sf::Font` | Font for UI text |
| `pieceTextures` | `unordered_map<string, sf::Texture>` | Piece graphics |
| `pieceSelected` | `bool` | Whether a piece is currently selected |
| `selectedRow/Col` | `int` | Coordinates of selected piece |
| `gameOver` | `bool` | Game termination state |
| `vsAI` | `bool` | Whether playing against AI |
| `aiIsWhite` | `bool` | AI color preference |
| `validMoves` | `vector<sf::Vector2i>` | Highlighted legal moves |
| `positionHistory` | `vector<string>` | For draw condition checking |
| `fiftyMoveCounter` | `int` | Fifty-move rule tracking |

### Methods

| Method | Return Type | Description |
|--------|-------------|-------------|
| `ChessGUI(...)` | - | Constructor with AI options |
| `initialize()` | `bool` | Setup graphics and resources |
| `run()` | `void` | Main GUI game loop |
| `checkThreefoldRepetition()` | `bool` | Detects threefold repetition |
| `checkFiftyMoveRule()` | `bool` | Detects fifty-move rule |
| `updateDrawConditions()` | `void` | Updates draw condition tracking |

---

## Move Struct (`Moves.h`)

**Purpose:** Stores comprehensive move information for replay, PGN generation, and legality checking.

### Fields

| Field | Type | Description |
|-------|------|-------------|
| `player` | `string` | Player who made the move |
| `fromCoord` | `string` | Starting square (e.g., "e2") |
| `toCoord` | `string` | Ending square (e.g., "e4") |
| `pieceSymbol` | `char` | Piece character ('P', 'N', etc.) |
| `isCapture` | `bool` | Whether move captures a piece |
| `isEnPassant` | `bool` | En passant capture flag |
| `isPromotion` | `bool` | Pawn promotion flag |
| `promotionTo` | `char` | Promoted piece type (default 'Q') |
| `isCheck` | `bool` | Move results in check |
| `isCheckmate` | `bool` | Move results in checkmate |
| `isKingsideCastle` | `bool` | Kingside castling move |
| `isQueensideCastle` | `bool` | Queenside castling move |
| `RankUnclear` | `bool` | Requires rank disambiguation |
| `FileUnclear` | `bool` | Requires file disambiguation |

### Key Method

**`toPGN()`**: Converts Move object to standard PGN notation
- Handles special moves (castling, en passant, promotion)
- Adds disambiguation when multiple pieces can reach same square
- Includes check (+) and checkmate (#) symbols

---

## Board Class (`Board.h` / `Board.cpp`)

**Purpose:** Represents the 8×8 chessboard state and manages piece interactions.

### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `squares[8][8]` | `Piece*` | 2D array representing board positions |
| `shouldDeletePieces` | `bool` | Memory management flag |
| `lastMove` | `Move` | Most recent move played |
| `hasLastMove` | `bool` | Whether any move has been made |
| `enPassantTarget` | `string` | En passant target square |
| `*CanCastle*` | `bool` | Castling rights for each side/direction |

### Core Methods

| Method | Return Type | Description |
|--------|-------------|-------------|
| `initialize()` | `void` | Sets up starting position |
| `printBoard()` *(console only)* | `void` | Console board display |
| `getPiece(int, int)` | `Piece*` | Retrieves piece at coordinates |
| `setPiece(int, int, Piece*)` | `void` | Places piece (for simulations) |
| `movePiece(...)` | `bool` | Validates and executes moves |
| `isInCheck(bool)` | `bool` | Checks if king is under attack |
| `isCheckmate(bool)` | `bool` | Determines checkmate condition |
| `isStalemate(bool)` | `bool` | Determines stalemate condition |
| `isPromotionMove(...)` | `bool` | Detects pawn promotion |
| `generateFEN(...)` | `string` | Creates FEN notation string |
| `getSimplePosition(bool)` | `string` | Simplified position string |
| `insufficientMaterialCheck()` | `bool` | Detects insufficient material draws |

### Castling Management

| Method | Return Type | Description |
|--------|-------------|-------------|
| `getWhiteCanCastleKingside()` | `bool` | White kingside castling rights |
| `getWhiteCanCastleQueenside()` | `bool` | White queenside castling rights |
| `getBlackCanCastleKingside()` | `bool` | Black kingside castling rights |
| `getBlackCanCastleQueenside()` | `bool` | Black queenside castling rights |
| `updateCastlingRights(...)` | `void` | Updates rights after moves |

---

## Piece Hierarchy

### Abstract Base Class: Piece (`Piece.h`)

**Purpose:** Defines common interface for all chess pieces.

#### Core Interface

| Member | Type | Description |
|--------|------|-------------|
| `symbol` | `char` | Piece character representation |
| `isWhite` | `bool` | Color flag |
| `isWhitePiece()` | `bool` | Color accessor |
| `getSymbol()` | `char` | **Pure virtual** - returns piece symbol |
| `isValidMove(...)` | `bool` | **Pure virtual** - validates moves |
| `clone()` | `Piece*` | **Pure virtual** - creates copy for simulations |

### Derived Piece Classes

Each piece implements unique movement logic:

#### **King** (`King.h`)
- **Movement:** One square in any direction
- **Special:** Castling logic with safety verification
- **Symbol:** 'K' (White) / 'k' (Black)

#### **Queen** (`Queen.h`)
- **Movement:** Combines rook and bishop movement
- **Range:** Unlimited distance on ranks, files, and diagonals
- **Symbol:** 'Q' (White) / 'q' (Black)

#### **Rook** (`Rook.h`)
- **Movement:** Horizontal and vertical lines
- **Special:** Affects castling rights when moved
- **Symbol:** 'R' (White) / 'r' (Black)

#### **Bishop** (`Bishop.h`)
- **Movement:** Diagonal lines only
- **Range:** Unlimited distance on diagonals
- **Symbol:** 'B' (White) / 'b' (Black)

#### **Knight** (`Knight.h`)
- **Movement:** L-shaped (2+1 squares)
- **Special:** Can jump over other pieces
- **Symbol:** 'N' (White) / 'n' (Black)

#### **Pawn** (`Pawn.h`)
- **Movement:** Forward one square, two on first move
- **Capture:** Diagonal only
- **Special:** En passant, promotion
- **Symbol:** 'P' (White) / 'p' (Black)

---

## Architecture Overview

```
Game (Main Logic)
├── Board (State Management)
│   ├── Piece Hierarchy (Movement Rules)
│   │   ├── King, Queen, Rook
│   │   ├── Bishop, Knight, Pawn
│   └── Move Validation
├── ChessGUI (SFML Interface)
│   ├── Rendering System
│   ├── User Input Handling
│   └── AI Integration
└── Move System
    ├── Move Struct (Data)
    ├── PGN Generation
    └── FEN Generation
```

---

## Key Features

### **Bird Engine (AI Implementation)**
- **Algorithm:** Minimax with alpha-beta pruning
- **Depth:** Currently fixed at 3 for performance balance
- **Evaluation Factors:**
  - Material balance (piece values)
  - Piece-square table bonuses
  - Castling status and king safety
  - Center control and piece development
- **Performance:** Node counting for optimization
- **Achievement:** One game achieved 84% accuracy rating by Stockfish against a 1400 ELO player

### **Special Moves**
- **En Passant:** Automatic detection and execution
- **Castling:** Safety verification and rights management
- **Promotion:** User choice with default to Queen

### **PGN & FEN Generation**
- **PGN Output:** Standard algebraic notation with disambiguation logic (e.g., Nfg4)
- **FEN Generation:** Complete position description including castling rights and en passant
- **Move History:** Full game replay capability with notation export
- **Validation:** Tested through chess.com analysis board verification

### **Draw Conditions**
- **Threefold Repetition:** Position tracking
- **Fifty-Move Rule:** Move counter management
- **Insufficient Material:** Automatic detection
- **Stalemate:** No legal moves available

---

## Current Implementation

### **GUI Mode (Active)**
- SFML-based graphical interface
- Mouse-driven piece selection
- Visual move highlighting
- Real-time game state display

### **Console Mode (In Development)**
- Text-based interface
- Direct algebraic notation input
- PGN/FEN output to console
- Full game replay

---

## Object-Oriented Design Principles

### **Inheritance & Polymorphism**
The game demonstrates clean OOP architecture through:
- **Piece Hierarchy:** Abstract base class `Piece` with derived classes for each piece type
- **Polymorphic Behavior:** Each piece overrides `isValidMove()` with specific movement rules
- **Code Reusability:** Common piece functionality shared through inheritance
- **Efficiency:** Polymorphic dispatch enables uniform piece handling

### **Encapsulation & Abstraction**
- **Board State Management:** `Board` class encapsulates 8×8 grid and game rules
- **Game Logic Separation:** `Game` class handles turns, AI, and notation generation
- **Data Hiding:** Private members with controlled public interfaces
- **Modular Design:** Clear separation of concerns between classes

---

## Development Challenges & Solutions

### **Major Challenges Overcome:**
1. **Rule Compliance:** Implementing complex rules like castling and en passant
2. **Checkmate Detection:** Fixed through proper board copying with custom copy constructor
3. **PGN Disambiguation:** Resolving ambiguous notation (e.g., Nfg4 vs Neg4)
4. **AI Tuning:** Balancing engine behavior for playability vs strength
5. **State Tracking:** Managing castling rights and en passant through move flags

### **Technical Learnings:**
- Deep understanding of chess notation and game logic
- Debugging polymorphic C++ code effectively
- Game state representation using industry standards (FEN/PGN)
- Performance considerations: 2D arrays vs bitboards for future optimization

---

## Testing & Validation

The engine has been thoroughly tested through:
- **Full Game Simulations:** Both PvP and AI modes
- **External Validation:** Verification using chess.com analysis board
- **Edge Case Testing:** Checkmate, stalemate, and draw conditions
- **AI Performance:** Games analyzed by Stockfish for accuracy ratings

---

## Future Development

### **Planned Improvements:**
- **Console/GUI Mode Selection:** Not fully confirmed but have thought about it
- **Enhanced AI:** Better heuristics and opening book integration
- **Chess960 Support:** Fischer Random Chess with 960 possible starting positions
- **Performance Optimization:** Transition to bitboard representation and hashing
- **GUI Enhancement:** Further development of the SFML-based interface
- **Engine Strength:** Improved evaluation functions for stronger play
- **Move History:** Show move history beside the board while playing, also have it update as the game goes on and when the game ends, save option
- **Efficiency:** Switch to bitboards instead of 2D arrays

### **Current Focus:**
The Bird Engine continues to evolve with better move selection algorithms and expanded chess variant support, maintaining the clean OOP architecture while improving performance and chess strength.
