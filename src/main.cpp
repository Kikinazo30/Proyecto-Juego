#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"

struct Platform {
    sf::RectangleShape shape;
    sf::FloatRect      bounds;

    Platform(float x, float y, float w, float h, sf::Color color = sf::Color(80, 80, 80)) {
        shape.setPosition(x, y);
        shape.setSize({w, h});
        shape.setFillColor(color);
        bounds = {x, y, w, h};
    }
};

int main() {
    const unsigned int WIDTH  = 1280;
    const unsigned int HEIGHT = 720;

    sf::RenderWindow window(
        sf::VideoMode(WIDTH, HEIGHT),
        "Proyecto Juego - PvP Metroidvania",
        sf::Style::Close
    );
    window.setFramerateLimit(60);

    Player player1(PlayerID::One,  {200.f, 500.f}, sf::Color(100, 149, 237));
    Player player2(PlayerID::Two,  {900.f, 500.f}, sf::Color(220, 80,  80));

    std::vector<Platform> platforms = {
        Platform(0.f,    650.f, 1280.f, 70.f,  sf::Color(60, 60, 60)),
        Platform(50.f,   520.f, 180.f,  20.f),
        Platform(100.f,  390.f, 150.f,  20.f),
        Platform(50.f,   260.f, 200.f,  20.f),
        Platform(500.f,  560.f, 280.f,  20.f),
        Platform(450.f,  420.f, 200.f,  20.f),
        Platform(540.f,  280.f, 200.f,  20.f),
        Platform(1050.f, 520.f, 180.f,  20.f),
        Platform(1000.f, 390.f, 150.f,  20.f),
        Platform(1030.f, 260.f, 200.f,  20.f),
        Platform(-20.f,    0.f,  20.f, 720.f, sf::Color(40, 40, 40)),
        Platform(1280.f,   0.f,  20.f, 720.f, sf::Color(40, 40, 40)),
    };

    std::vector<sf::FloatRect> colliders;
    for (const auto& p : platforms)
        colliders.push_back(p.bounds);

    sf::Font font;
    bool fontLoaded = font.loadFromFile("assets/fonts/arial.ttf");

    sf::Text label1, label2, hud;
    if (fontLoaded) {
        label1.setFont(font); label1.setString("J1");
        label1.setCharacterSize(16); label1.setFillColor(sf::Color::White);

        label2.setFont(font); label2.setString("J2");
        label2.setCharacterSize(16); label2.setFillColor(sf::Color::White);

        hud.setFont(font);
        hud.setString("J1: A/D mover | Espacio saltar | LShift dash      J2: Flechas mover | Enter saltar | RShift dash");
        hud.setCharacterSize(14);
        hud.setFillColor(sf::Color(180, 180, 180));
        hud.setPosition(10.f, 690.f);
    }

    sf::RectangleShape background({(float)WIDTH, (float)HEIGHT});
    background.setFillColor(sf::Color(15, 15, 25));

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);

        player1.handleInput();
        player2.handleInput();
        player1.update(dt, colliders);
        player2.update(dt, colliders);

        if (fontLoaded) {
            label1.setPosition(player1.getPosition().x + 10.f, player1.getPosition().y - 22.f);
            label2.setPosition(player2.getPosition().x + 10.f, player2.getPosition().y - 22.f);
        }

        window.clear();
        window.draw(background);

        for (const auto& p : platforms)
            window.draw(p.shape);

        player1.draw(window);
        player2.draw(window);

        if (fontLoaded) {
            window.draw(label1);
            window.draw(label2);
            window.draw(hud);
        }

        window.display();
    }

    return 0;
}
