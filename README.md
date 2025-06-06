# Swag Chess

chess game made in the second semester of a CS major at IBA :D

## Quick Start

1. **Build the project** using your preferred C++ compiler
2. **Install SFML** for GUI support
3. **Run the executable** to start playing

## Current Status

- **GUI Mode**: Fully functional SFML-based interface
- **Console Mode**: Discontinued for now
- **AI Opponent**: "Bird Engine" with minimax algorithm

## Controls

- **Mouse**: Click to select and move pieces
- **New Game Button**: Reset the board
- **Mode Toggle**: Switch between vs Human and vs AI

## Project Structure

```
├── Chess/
│   ├── ChessGUI.cpp          # SFML interface implementation
│   ├── Game.cpp              # Core game logic and AI
│   ├── Board.cpp             # Board state management
│   ├── Piece classes/        # Individual piece movement rules
│   └── Move.h                # Move data structure
├── assets/                   # Piece graphics and fonts
└── Documentation/            # Detailed technical documentation
```

## Build Requirements

- **C++17** or higher
- **SFML 3.0.0+** for graphics
- Standard chess piece PNG files in `assets/` folder
- Font file (arial.ttf) for UI text

## Features

- Complete chess rule implementation
- Visual move highlighting
- AI opponent with configurable difficulty
- PGN and FEN notation support
- Draw condition detection (threefold repetition, fifty-move rule, etc.)

## Technical Highlights

Built using object-oriented design principles with inheritance, polymorphism, and clean separation of concerns. The AI uses minimax with alpha-beta pruning and has achieved 84% accuracy against intermediate players.

For detailed technical documentation, see `swag_chess_documentation.md`.
