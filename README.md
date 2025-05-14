chess game made in the second semester of a CS major at IBA :D

# Terminal Chess Game in C++

## 1. Introduction  
This project is a terminal-based chess game written in C++ that supports two gameplay modes: multiplayer and vs AI. It has been built using object-oriented programming principles, and it demonstrates the use of inheritance, polymorphism, encapsulation, and abstraction in a real-world game application.

## 2. Features  
Player vs Player and Player vs AI modes – Minimax AI with a static evaluation function. Full chess rule support: castling, en passant, promotion. PGN and FEN notation generation and tracking. Game state tracking for threefold repetition and 50-move rule.

## 3. Object-Oriented Design  
The game is structured around a `Piece` base class with derived classes for each chess piece. Each piece overrides the `isValidMove` method to define its legal move set. This is polymorphism used, showing that each piece has its own specific rules for movement, allowing reusability of code and creates efficiency. The `Board` class manages the 8x8 grid and enforces movement and game rules. The `Game` class handles the main loop, PGN/FEN tracking, and player/AI turns. `Move` struct objects track metadata including checks, castling, and disambiguation.

## 4. AI Logic  
The AI uses the Minimax algorithm with alpha-beta pruning to select the best move. An evaluation function considers: Material balance, Piece-square table bonuses, Castling status, Center control and development. Depth is currently fixed to 3 for performance balance.

## 5. PGN & FEN Implementation  
PGN (Portable Game Notation) is constructed move-by-move and includes disambiguation logic for cases like Nfg4. FEN (Forsyth-Edwards Notation) is generated after each turn and includes castling rights, en passant squares, half-move and full-move counters. Move History printed at the end of the game both in FEN or PGN, so it can be shared to other engines or sites.

## 6. Testing & Validation  
The game has been tested through: Simulated full-length games (PvP and vs AI), Verification on chess.com analysis board, Edge case handling: checkmate, stalemate, draw rules.

## 7. Challenges & Learnings  
**Challenges:** Implementing full rule compliance (especially castling, en passant and checkmate). PGN ambiguity resolution (e.g., Nfg4 disambiguation). AI behaviour tuning for playability. Fixed bugs in the checkmate function by fully copying the board through a customized copy constructor. Castling and en passant was fixed by tracking the last move and seeing if the king or rook was moved. Flags used to track and store every move. AI is still a work in progress. One of the games played by the bird engine was given an 84% accuracy by stockfish (against a 1400 ELO rating player, me). Better heuristics can be added, and scoring can be adjusted to play the top engine moves.

**Learnings:** Deeper understanding of move notation and chess logic. Debugging polymorphic C++ code. Working with game state representation (FEN/PGN). Using 2D arrays is not the most efficient way to go, bit boards and hashing is a much better option, so will consider this moving forward.

## 8. Conclusion and Future  
This project demonstrated the application of object-oriented programming principles in building a non-trivial, rules-complete chess engine. It integrates classic AI techniques and highlights the importance of clean design and modularity in game development.

Currently working to improve the AI’s choice of moves, to ensure it plays the best move there is. More heuristics and factors such as considering famous opening lines or moves, can be added to ensure the AI understands the board. Chess960 or more commonly known as "Fischer Chess" or "Freestyle Chess," is also being worked on. The arrangement of pieces will be read from a file from which the user can either select a specific position out of 960 possible positions or it will be randomized. This way, one can tell if the engine really plays proper moves as the board isn't arranged in the traditional manner. GUI is also in the works, this will make it more easier to debug as entering the piece's coordinates in proper move notation might not be easy for beginners.
