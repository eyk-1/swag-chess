#include <SFML/Graphics.hpp>
#include "ChessGUI.h"
#include <iostream>

int main() {
    // Create mode selection window
    sf::RenderWindow window(sf::VideoMode({ 600u, 400u }), "Swag Chess - Select Mode");

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        // Try alternative font locations
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf") &&
            !font.openFromFile("/System/Library/Fonts/Arial.ttf") &&
            !font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            std::cerr << "Could not load any font!" << std::endl;
            return 1;
        }
    }

    sf::Text title(font);
    title.setString("Choose Game Mode");
    title.setCharacterSize(36);
    title.setPosition({ 120.f, 40.f });
    title.setFillColor(sf::Color::White);

    sf::RectangleShape multiButton({ 300.f, 60.f });
    multiButton.setPosition({ 150.f, 120.f });
    multiButton.setFillColor(sf::Color(100, 100, 255));

    sf::Text multiText(font);
    multiText.setString("Multiplayer");
    multiText.setCharacterSize(24);
    multiText.setPosition({ 210.f, 135.f });
    multiText.setFillColor(sf::Color::White);

    sf::RectangleShape aiButton({ 300.f, 60.f });
    aiButton.setPosition({ 150.f, 200.f });
    aiButton.setFillColor(sf::Color(100, 255, 100));

    sf::Text aiText(font);
    aiText.setString("Play against Bird");
    aiText.setCharacterSize(24);
    aiText.setPosition({ 200.f, 215.f });
    aiText.setFillColor(sf::Color::Black);

    sf::RectangleShape aiWhiteButton({ 300.f, 60.f });
    aiWhiteButton.setPosition({ 150.f, 280.f });
    aiWhiteButton.setFillColor(sf::Color(255, 200, 100));

    sf::Text aiWhiteText(font);
    aiWhiteText.setString("Bird plays White");
    aiWhiteText.setCharacterSize(24);
    aiWhiteText.setPosition({ 205.f, 295.f });
    aiWhiteText.setFillColor(sf::Color::Black);

    int selectedMode = 0; // 0 = no selection, 1 = multiplayer, 2 = AI as black, 3 = AI as white

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return 0;
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(mousePressed->position);

                    if (multiButton.getGlobalBounds().contains(mousePos)) {
                        selectedMode = 1;
                        window.close();
                    }
                    else if (aiButton.getGlobalBounds().contains(mousePos)) {
                        selectedMode = 2;
                        window.close();
                    }
                    else if (aiWhiteButton.getGlobalBounds().contains(mousePos)) {
                        selectedMode = 3;
                        window.close();
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));
        window.draw(title);
        window.draw(multiButton);
        window.draw(multiText);
        window.draw(aiButton);
        window.draw(aiText);
        window.draw(aiWhiteButton);
        window.draw(aiWhiteText);
        window.display();
    }

    // Launch the chess game based on selection
    if (selectedMode > 0) {
        ChessGUI* chessGUI = nullptr;

        switch (selectedMode) {
        case 1:
            // Multiplayer mode
            chessGUI = new ChessGUI(false, false);
            break;
        case 2:
            // AI plays black
            chessGUI = new ChessGUI(true, false);
            break;
        case 3:
            // AI plays white
            chessGUI = new ChessGUI(true, true);
            break;
        }

        if (chessGUI) {
            if (chessGUI->initialize()) {
                std::cout << "Starting chess game..." << std::endl;
                chessGUI->run();
            }
            else {
                std::cerr << "Failed to initialize chess GUI!" << std::endl;
            }

            delete chessGUI;
        }
    }

    return 0;
}