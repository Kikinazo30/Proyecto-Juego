#pragma once
#include <SFML/Graphics.hpp>

enum class PlayerID { One, Two };

class Player {
public:
    Player(PlayerID id, sf::Vector2f startPos, sf::Color color);

    void handleInput();
    void update(float dt, const std::vector<sf::FloatRect>& platforms);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    void takeDamage(int amount);           
    sf::FloatRect getAttackBounds() const; 
    bool getIsAttacking() const;           
    int getHp() const;                     

private:
    PlayerID id;

    sf::RectangleShape shape;
    // --- NUEVO: Textura para el sprite del jugador ---
    sf::Texture texture;
    // -------------------------------------------------
    sf::Vector2f velocity;
    sf::Vector2f position;

    bool onGround;
    bool canDash;
    bool isDashing;
    float dashTimer;

    int hp;
    int maxHp;
    bool isAttacking;
    float attackTimer;
    float facingDir; 

    static constexpr float SPEED = 200.f;
    static constexpr float JUMP_FORCE = -550.f;
    static constexpr float GRAVITY = 1200.f;
    static constexpr float MAX_FALL = 600.f;
    static constexpr float DASH_SPEED = 500.f;
    static constexpr float DASH_DURATION = 0.15f;
    
    static constexpr float ATTACK_DURATION = 0.2f; 

    sf::Keyboard::Key keyLeft, keyRight, keyJump, keyDash, keyAttack;

    void applyGravity(float dt);
    void resolveCollisions(const std::vector<sf::FloatRect>& platforms);
};
