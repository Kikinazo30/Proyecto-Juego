#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <optional>
#include <algorithm>
#include <iostream>
#include <string>
#include "Player.h"

enum class GameState {
    MainMenu,
    Playing,
    GameOver
};

struct Platform {
    sf::RectangleShape shape;
    sf::FloatRect bounds;

    Platform(float x, float y, float w, float h, sf::Color color = sf::Color(0, 255, 120, 75)) {
        shape.setPosition({ x, y });
        shape.setSize({ w, h });
        shape.setFillColor(color);
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color(0, 255, 120, 180));
        bounds = { {x, y}, {w, h} };
    }
};

struct GameSounds {
    sf::SoundBuffer jumpBuffer;
    sf::SoundBuffer dashBuffer;
    sf::SoundBuffer attackBuffer;
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer confirmBuffer;

    sf::Sound jump;
    sf::Sound dash;
    sf::Sound attack;
    sf::Sound hit;
    sf::Sound confirm;

    bool jumpLoaded = false;
    bool dashLoaded = false;
    bool attackLoaded = false;
    bool hitLoaded = false;
    bool confirmLoaded = false;

    GameSounds()
        : jump(jumpBuffer),
        dash(dashBuffer),
        attack(attackBuffer),
        hit(hitBuffer),
        confirm(confirmBuffer)
    {
    }

    bool load() {
        jumpLoaded = jumpBuffer.loadFromFile("assets/sounds/jump.wav");
        dashLoaded = dashBuffer.loadFromFile("assets/sounds/dash.wav");
        attackLoaded = attackBuffer.loadFromFile("assets/sounds/attack.wav");
        hitLoaded = hitBuffer.loadFromFile("assets/sounds/hit.wav");
        confirmLoaded = confirmBuffer.loadFromFile("assets/sounds/confirm.wav");

        jump.setVolume(65.f);
        dash.setVolume(60.f);
        attack.setVolume(55.f);
        hit.setVolume(75.f);
        confirm.setVolume(55.f);

        if (!jumpLoaded) std::cout << "No se pudo cargar: assets/sounds/jump.wav\n";
        if (!dashLoaded) std::cout << "No se pudo cargar: assets/sounds/dash.wav\n";
        if (!attackLoaded) std::cout << "No se pudo cargar: assets/sounds/attack.wav\n";
        if (!hitLoaded) std::cout << "No se pudo cargar: assets/sounds/hit.wav\n";
        if (!confirmLoaded) std::cout << "No se pudo cargar: assets/sounds/confirm.wav\n";

        return jumpLoaded || dashLoaded || attackLoaded || hitLoaded || confirmLoaded;
    }

    void playJump() {
        if (jumpLoaded) jump.play();
    }

    void playDash() {
        if (dashLoaded) dash.play();
    }

    void playAttack() {
        if (attackLoaded) attack.play();
    }

    void playHit() {
        if (hitLoaded) hit.play();
    }

    void playConfirm() {
        if (confirmLoaded) confirm.play();
    }
};

void checkAttackCollision(Player& attacker, Player& defender) {
    if (!attacker.canHitDuringAttack()) {
        return;
    }

    if (attacker.getAttackBounds().findIntersection(defender.getBounds())) {
        defender.takeDamage(10, attacker.getFacingDirection());
        attacker.markAttackHit();
    }
}

void drawHealthBar(
    sf::RenderWindow& window,
    float x,
    float y,
    float width,
    float height,
    int hp,
    int maxHp,
    sf::Color fillColor
) {
    float hpPercent = 0.f;

    if (maxHp > 0) {
        hpPercent = static_cast<float>(hp) / static_cast<float>(maxHp);
    }

    hpPercent = std::clamp(hpPercent, 0.f, 1.f);

    sf::RectangleShape background({ width, height });
    background.setPosition({ x, y });
    background.setFillColor(sf::Color(25, 25, 25, 220));
    background.setOutlineThickness(3.f);
    background.setOutlineColor(sf::Color(230, 230, 230, 220));

    sf::RectangleShape life({ width * hpPercent, height });
    life.setPosition({ x, y });
    life.setFillColor(fillColor);

    window.draw(background);
    window.draw(life);
}

bool loadUIFont(sf::Font& font) {
    std::vector<std::string> possibleFonts = {
        "assets/fonts/game_font.ttf",
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf"
    };

    for (const auto& path : possibleFonts) {
        if (font.openFromFile(path)) {
            std::cout << "Fuente cargada: " << path << std::endl;
            return true;
        }
    }

    std::cout << "No se pudo cargar ninguna fuente para el texto.\n";
    return false;
}

bool loadBackgroundMusic(sf::Music& music) {
    std::vector<std::string> possibleMusicFiles = {
        "assets/music/battle_theme.ogg",
        "assets/music/battle_theme.mp3",
        "assets/music/battle_theme.wav",
        "assets/music/battle_theme.flac"
    };

    for (const auto& path : possibleMusicFiles) {
        if (music.openFromFile(path)) {
            std::cout << "Musica cargada: " << path << std::endl;
            return true;
        }
    }

    std::cout << "No se pudo cargar la musica.\n";
    return false;
}

void centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
        });

    text.setPosition({ x, y });
}

void drawMainMenu(
    sf::RenderWindow& window,
    const sf::Font& font,
    bool fontLoaded
) {
    sf::RectangleShape overlay({ 1280.f, 720.f });
    overlay.setPosition({ 0.f, 0.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 145));
    window.draw(overlay);

    sf::RectangleShape panel({ 760.f, 430.f });
    panel.setPosition({ 260.f, 145.f });
    panel.setFillColor(sf::Color(20, 24, 45, 220));
    panel.setOutlineThickness(5.f);
    panel.setOutlineColor(sf::Color(230, 230, 255, 230));
    window.draw(panel);

    sf::RectangleShape blueSide({ 12.f, 430.f });
    blueSide.setPosition({ 260.f, 145.f });
    blueSide.setFillColor(sf::Color(80, 160, 255));
    window.draw(blueSide);

    sf::RectangleShape redSide({ 12.f, 430.f });
    redSide.setPosition({ 1008.f, 145.f });
    redSide.setFillColor(sf::Color(255, 90, 120));
    window.draw(redSide);

    if (!fontLoaded) {
        return;
    }

    sf::Text title(font, "MOUNTAIN DUEL", 68);
    title.setFillColor(sf::Color::White);
    title.setOutlineThickness(5.f);
    title.setOutlineColor(sf::Color(0, 0, 0, 220));
    centerText(title, 640.f, 220.f);
    window.draw(title);

    sf::Text subtitle(font, "PvP Platform Fighter", 30);
    subtitle.setFillColor(sf::Color(220, 220, 240));
    subtitle.setOutlineThickness(2.f);
    subtitle.setOutlineColor(sf::Color(0, 0, 0, 180));
    centerText(subtitle, 640.f, 285.f);
    window.draw(subtitle);

    sf::Text start(font, "Press ENTER to start", 34);
    start.setFillColor(sf::Color(255, 240, 160));
    start.setOutlineThickness(3.f);
    start.setOutlineColor(sf::Color(0, 0, 0, 200));
    centerText(start, 640.f, 365.f);
    window.draw(start);

    sf::Text controls1(font, "KAI: A/D Move  |  Space Jump  |  LShift Dash  |  F Attack", 22);
    controls1.setFillColor(sf::Color(150, 205, 255));
    controls1.setOutlineThickness(2.f);
    controls1.setOutlineColor(sf::Color(0, 0, 0, 190));
    centerText(controls1, 640.f, 450.f);
    window.draw(controls1);

    sf::Text controls2(font, "RUNE: Arrows Move  |  Enter Jump  |  RShift Dash  |  RCtrl Attack", 22);
    controls2.setFillColor(sf::Color(255, 150, 170));
    controls2.setOutlineThickness(2.f);
    controls2.setOutlineColor(sf::Color(0, 0, 0, 190));
    centerText(controls2, 640.f, 485.f);
    window.draw(controls2);

    sf::Text exitText(font, "ESC to exit", 20);
    exitText.setFillColor(sf::Color(210, 210, 210));
    exitText.setOutlineThickness(2.f);
    exitText.setOutlineColor(sf::Color(0, 0, 0, 180));
    centerText(exitText, 640.f, 535.f);
    window.draw(exitText);
}

void drawGameOverOverlay(
    sf::RenderWindow& window,
    const sf::Font& font,
    bool fontLoaded,
    PlayerID winner
) {
    sf::RectangleShape overlay({ 1280.f, 720.f });
    overlay.setPosition({ 0.f, 0.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 135));

    sf::RectangleShape winnerBox({ 620.f, 210.f });
    winnerBox.setPosition({ 330.f, 255.f });

    if (winner == PlayerID::One) {
        winnerBox.setFillColor(sf::Color(45, 110, 210, 230));
    }
    else {
        winnerBox.setFillColor(sf::Color(210, 55, 85, 230));
    }

    winnerBox.setOutlineThickness(5.f);
    winnerBox.setOutlineColor(sf::Color::White);

    window.draw(overlay);
    window.draw(winnerBox);

    if (!fontLoaded) {
        return;
    }

    std::string winnerText;

    if (winner == PlayerID::One) {
        winnerText = "KAI WINS";
    }
    else {
        winnerText = "RUNE WINS";
    }

    sf::Text title(font, winnerText, 64);
    title.setFillColor(sf::Color::White);
    title.setOutlineThickness(4.f);
    title.setOutlineColor(sf::Color(0, 0, 0, 200));
    centerText(title, 640.f, 325.f);

    sf::Text restart(font, "Press ENTER or R to restart", 30);
    restart.setFillColor(sf::Color(240, 240, 240));
    restart.setOutlineThickness(2.f);
    restart.setOutlineColor(sf::Color(0, 0, 0, 200));
    centerText(restart, 640.f, 405.f);

    window.draw(title);
    window.draw(restart);
}

int main() {
    const unsigned int WINDOW_WIDTH = 1280;
    const unsigned int WINDOW_HEIGHT = 720;

    const float WORLD_WIDTH = 1600.f;
    const float WORLD_HEIGHT = 900.f;

    const float SCALE_X = WORLD_WIDTH / static_cast<float>(WINDOW_WIDTH);
    const float SCALE_Y = WORLD_HEIGHT / static_cast<float>(WINDOW_HEIGHT);

    auto sx = [&](float value) {
        return value * SCALE_X;
        };

    auto sy = [&](float value) {
        return value * SCALE_Y;
        };

    sf::RenderWindow window(
        sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }),
        "Proyecto Juego - PvP Metroidvania"
    );

    window.setFramerateLimit(60);

    sf::View gameView;
    gameView.setSize({ WORLD_WIDTH, WORLD_HEIGHT });
    gameView.setCenter({ WORLD_WIDTH / 2.f, WORLD_HEIGHT / 2.f });
    window.setView(gameView);

    sf::Font uiFont;
    bool fontLoaded = loadUIFont(uiFont);

    sf::Music backgroundMusic;
    bool musicLoaded = loadBackgroundMusic(backgroundMusic);

    if (musicLoaded) {
        backgroundMusic.setLooping(true);
        backgroundMusic.setVolume(45.f);
        backgroundMusic.play();
    }

    GameSounds sounds;
    sounds.load();

    sf::Texture backgroundTexture;

    if (!backgroundTexture.loadFromFile("assets/images/backgrounds/pvp_background.png")) {
        std::cout << "Error: no se pudo cargar la imagen de fondo.\n";
        std::cout << "Ruta esperada: assets/images/backgrounds/pvp_background.png\n";
        return -1;
    }

    sf::Sprite backgroundSprite(backgroundTexture);

    sf::Vector2u textureSize = backgroundTexture.getSize();

    float backgroundScaleX = WORLD_WIDTH / static_cast<float>(textureSize.x);
    float backgroundScaleY = WORLD_HEIGHT / static_cast<float>(textureSize.y);

    backgroundSprite.setScale({ backgroundScaleX, backgroundScaleY });
    backgroundSprite.setPosition({ 0.f, 0.f });

    sf::Vector2f player1Start = { sx(95.f), sy(485.f) };
    sf::Vector2f player2Start = { sx(950.f), sy(485.f) };

    Player player1(PlayerID::One, player1Start, sf::Color(100, 149, 237));
    Player player2(PlayerID::Two, player2Start, sf::Color(220, 80, 80));

    std::vector<Platform> platforms = {
        Platform(sx(55.f),  sy(514.f), sx(300.f), sy(20.f)),
        Platform(sx(225.f), sy(550.f), sx(835.f), sy(26.f)),
        Platform(sx(930.f), sy(514.f), sx(300.f), sy(20.f)),

        Platform(sx(570.f), sy(207.f), sx(140.f), sy(16.f)),
        Platform(sx(326.f), sy(270.f), sx(162.f), sy(16.f)),
        Platform(sx(784.f), sy(270.f), sx(162.f), sy(16.f)),

        Platform(sx(139.f), sy(330.f), sx(145.f), sy(16.f)),
        Platform(sx(995.f), sy(330.f), sx(145.f), sy(16.f)),

        Platform(sx(566.f), sy(363.f), sx(150.f), sy(16.f)),

        Platform(sx(326.f), sy(420.f), sx(162.f), sy(16.f)),
        Platform(sx(786.f), sy(420.f), sx(162.f), sy(16.f)),

        Platform(sx(560.f), sy(502.f), sx(45.f), sy(12.f)),
        Platform(sx(684.f), sy(502.f), sx(45.f), sy(12.f)),

        Platform(sx(35.f), sy(0.f), sx(25.f), sy(650.f), sf::Color(0, 255, 120, 0)),
        Platform(sx(1220.f), sy(0.f), sx(25.f), sy(650.f), sf::Color(0, 255, 120, 0)),
        Platform(sx(35.f), sy(610.f), sx(1210.f), sy(45.f), sf::Color(0, 255, 120, 0))
    };

    std::vector<sf::FloatRect> colliders;

    for (const auto& p : platforms) {
        colliders.push_back(p.bounds);
    }

    bool showHitboxes = false;

    GameState gameState = GameState::MainMenu;
    PlayerID winner = PlayerID::One;

    auto resetMatch = [&]() {
        player1.reset(player1Start);
        player2.reset(player2Start);
        winner = PlayerID::One;
        };

    auto startMatch = [&]() {
        resetMatch();
        gameState = GameState::Playing;
        sounds.playConfirm();
        };

    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }

                if (gameState == GameState::MainMenu) {
                    if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        startMatch();
                        clock.restart();
                    }
                }
                else if (gameState == GameState::Playing) {
                    if (keyPressed->code == sf::Keyboard::Key::F3) {
                        showHitboxes = !showHitboxes;
                    }
                }
                else if (gameState == GameState::GameOver) {
                    if (
                        keyPressed->code == sf::Keyboard::Key::R ||
                        keyPressed->code == sf::Keyboard::Key::Enter
                        ) {
                        startMatch();
                        clock.restart();
                    }
                }
            }
        }

        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);

        if (gameState == GameState::Playing) {
            player1.handleInput();
            player2.handleInput();

            if (player1.consumeJumpEvent()) sounds.playJump();
            if (player2.consumeJumpEvent()) sounds.playJump();

            if (player1.consumeDashEvent()) sounds.playDash();
            if (player2.consumeDashEvent()) sounds.playDash();

            if (player1.consumeAttackEvent()) sounds.playAttack();
            if (player2.consumeAttackEvent()) sounds.playAttack();

            player1.update(dt, colliders);
            player2.update(dt, colliders);

            checkAttackCollision(player1, player2);
            checkAttackCollision(player2, player1);

            if (player1.consumeHitEvent()) sounds.playHit();
            if (player2.consumeHitEvent()) sounds.playHit();

            if (player1.isDead()) {
                winner = PlayerID::Two;
                gameState = GameState::GameOver;
            }

            if (player2.isDead()) {
                winner = PlayerID::One;
                gameState = GameState::GameOver;
            }
        }

        window.clear();

        window.setView(gameView);
        window.draw(backgroundSprite);

        if (gameState == GameState::Playing || gameState == GameState::GameOver) {
            if (showHitboxes) {
                for (const auto& p : platforms) {
                    window.draw(p.shape);
                }
            }

            player1.draw(window);
            player2.draw(window);
        }

        window.setView(window.getDefaultView());

        if (gameState == GameState::Playing || gameState == GameState::GameOver) {
            drawHealthBar(
                window,
                40.f,
                30.f,
                500.f,
                28.f,
                player1.getHp(),
                player1.getMaxHp(),
                sf::Color(80, 160, 255)
            );

            drawHealthBar(
                window,
                740.f,
                30.f,
                500.f,
                28.f,
                player2.getHp(),
                player2.getMaxHp(),
                sf::Color(255, 90, 120)
            );
        }

        if (gameState == GameState::MainMenu) {
            drawMainMenu(window, uiFont, fontLoaded);
        }

        if (gameState == GameState::GameOver) {
            drawGameOverOverlay(window, uiFont, fontLoaded, winner);
        }

        window.display();
    }

    return 0;
}