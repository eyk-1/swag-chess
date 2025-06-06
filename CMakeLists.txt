cmake_minimum_required(VERSION 3.16)
project(Chess)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 3.0 COMPONENTS system window graphics REQUIRED)

# Create executable
add_executable(Chess
    Chess/Chess.cpp
    Chess/ChessGUI.cpp
    Chess/Board.cpp
    Chess/Piece.cpp
    Chess/Pawn.cpp
    Chess/Rook.cpp
    Chess/Knight.cpp
    Chess/Bishop.cpp
    Chess/King.cpp
    Chess/Queen.cpp
    Chess/Game.cpp
    Chess/Moves.cpp
)

# Add header include directory
target_include_directories(Chess PRIVATE Chess)

# Link SFML libraries
target_link_libraries(Chess sfml-graphics sfml-window sfml-system)

# Copy SFML DLLs to output directory on Windows
if(WIN32)
    add_custom_command(TARGET Chess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE_DIR:Chess>
    )
endif()

# Set startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Chess)

# Set working directory for debugging
set_target_properties(Chess PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)