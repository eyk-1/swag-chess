#include <SFML/Graphics.hpp>
#include "ChessGUI.h"
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 600u, 400u }), "Swag Chess - Select Mode");

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Font load failed.\n";
        return 1;
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
    aiButton.setPosition({ 150.f, 220.f });
    aiButton.setFillColor(sf::Color(100, 255, 100));

    sf::Text aiText(font);
    aiText.setString("Play against Bird");
    aiText.setCharacterSize(24);
    aiText.setPosition({ 225.f, 235.f });
    aiText.setFillColor(sf::Color::Black);

    int selectedMode = 0;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

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
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));
        window.draw(title);
        window.draw(multiButton);
        window.draw(multiText);
        window.draw(aiButton);
        window.draw(aiText);
        window.display();
    }

    return 0;
}