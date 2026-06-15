#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"

struct Platform {
    sf::RectangleShape shape;
    sf::FloatRect      bounds;

    Platform(float x, float y, float w, float h, sf::Color color = sf::Color(80, 80, 80)) {
        shape.setPosition({ x, y });
        shape.setSize({ w, h });
        shape.setFillColor(color);
        bounds = { {x, y}, {w, h} };
    }
};

int main() {
    const unsigned int WIDTH = 1280;
    const unsigned int HEIGHT = 720;

    sf::RenderWindow window(
        sf::VideoMode({ WIDTH, HEIGHT }),
        "Proyecto Juego - PvP Metroidvania"
    );
    window.setFramerateLimit(60);

    Player player1(PlayerID::One, { 200.f, 500.f }, sf::Color(100, 149, 237));
    Player player2(PlayerID::Two, { 900.f, 500.f }, sf::Color(220, 80, 80));

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

    sf::RectangleShape background({ (float)WIDTH, (float)HEIGHT });
    background.setFillColor(sf::Color(15, 15, 25));

    // --- INICIO DEL CÓDIGO NUEVO (CARGAR IMAGEN) ---
    sf::Texture backgroundTexture;
    // Recuerda cambiar "tu_fondo.jpg" por el nombre de tu archivo en GitHub
    if (backgroundTexture.loadFromFile("assets/images/tu_fondo.jpg")) {
        // Carga exitosa
    }
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    
    sf::Vector2u textureSize = backgroundTexture.getSize();
    if (textureSize.x > 0 && textureSize.y > 0) {
        float scaleX = (float)WIDTH / textureSize.x;
        float scaleY = (float)HEIGHT / textureSize.y;
        backgroundSprite.setScale({ scaleX, scaleY });
    }
    // --- FIN DEL CÓDIGO NUEVO ---

    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    window.close();
        }

        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);

        player1.handleInput();
        player2.handleInput();
        player1.update(dt, colliders);
        player2.update(dt, colliders);

        window.clear();
        window.draw(background);

        // --- INICIO DEL CÓDIGO NUEVO (DIBUJAR IMAGEN) ---
        // Se dibuja justo después de tu fondo original para que lo cubra
        window.draw(backgroundSprite);
        // --- FIN DEL CÓDIGO NUEVO ---

        for (const auto& p : platforms)
            window.draw(p.shape);

        player1.draw(window);
        player2.draw(window);

        window.display();
    }

    return 0;
}
