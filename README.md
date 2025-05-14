chess game made in the second semester of a CS major at IBA :D

Overview
This is a C++ implementation of a full-featured chess game with both multiplayer and AI (Minimax) support. The project demonstrates object-oriented principles such as encapsulation, inheritance, abstraction, and polymorphism. Key features include:

Player vs Player and Player vs AI modes

Minimax AI with evaluation heuristics

Full support for chess rules: castling, en passant, pawn promotion

PGN and FEN support for recording and loading game states

Terminal-based UI

Setup Instructions
Clone the repository or unzip the provided folder.

Open the project in Visual Studio (or any C++-17 compatible IDE).

Build the project.

Run main() from main.cpp.

Dependencies: None external – standard C++17 STL only.

Usage
Launch the game and select a mode:

1 for Multiplayer

2 for Player vs AI

If AI mode is selected, choose your color or allow random assignment.

Enter moves in standard format, e.g. e2 e4, O-O, O-O-O, or exit to quit.

Examples:
Castling: O-O or O-O-O

Standard move: e2 e4

Exit: exit

Object-Oriented Design
The project follows OOP design using:

Encapsulation: Each piece class (e.g., Knight, Pawn) handles its own logic and data.

Abstraction: The Piece base class defines the interface, and subclasses implement specific behavior.

Inheritance: All chess pieces inherit from the abstract Piece class.

Polymorphism: Each piece overrides the isValidMove() method based on its behavior.

File Structure
pgsql
Copy
Edit
/src
│   Board.cpp / Board.h
│   Game.cpp / Game.h
│   Piece.h (base class)
│   Pawn.cpp / Pawn.h
│   Knight.cpp / Knight.h
│   Bishop.cpp / Bishop.h
│   Rook.cpp / Rook.h
│   Queen.cpp / Queen.h
│   King.cpp / King.h
│   Move.cpp / Move.h
│   main.cpp
Features
AI: Uses Minimax with evaluation heuristics.

Castling: Handled through king and rook state.

En Passant: Tracked using move history.

Promotion: Supports user choice or auto-queen.

PGN Logging: All moves are saved in proper notation.

FEN Export: Board state is recorded after each move.

Known Issues / Limitations
No GUI – terminal only

AI depth is fixed to 3; may slow down in late game

No undo functionality

PGN disambiguation currently handles most, but not all, rare edge cases

Testing
Manual and functional testing has been done through:

Sample move sequences

Edge cases (e.g., threefold repetition, 50-move rule, stalemate, checkmate)

AI playing both colors

PGN and FEN verification using chess.com tools
