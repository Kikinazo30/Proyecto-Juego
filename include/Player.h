#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "AnimatedSprite.h"

enum class PlayerID {
    One,
    Two
};

class Player {
public:
    Player(PlayerID id, sf::Vector2f startPos, sf::Color color);

    void handleInput();
    void update(float dt, const std::vector<sf::FloatRect>& platforms);
    void draw(sf::RenderWindow& window);

    void reset(sf::Vector2f startPos);

    sf::FloatRect getBounds() const;
    sf::FloatRect getAttackBounds() const;
    sf::Vector2f getPosition() const;

    bool getIsAttacking() const;
    bool canHitDuringAttack() const;
    void markAttackHit();

    void takeDamage(int amount, float knockbackDirection);

    int getHp() const;
    int getMaxHp() const;
    bool isDead() const;

    float getFacingDirection() const;

    bool consumeJumpEvent();
    bool consumeDashEvent();
    bool consumeAttackEvent();
    bool consumeHitEvent();

private:
    PlayerID id;

    sf::RectangleShape shape;
    AnimatedSprite anim;

    sf::Vector2f position;
    sf::Vector2f velocity;

    bool hasSprite = false;
    bool onGround = false;
    bool canDash = true;
    bool isDashing = false;
    bool isAttacking = false;
    bool facingLeft = false;

    bool attackAlreadyHit = false;
    bool wasAttackPressed = false;
    bool wasDashPressed = false;

    bool jumpEvent = false;
    bool dashEvent = false;
    bool attackEvent = false;
    bool hitEvent = false;

    float dashTimer = 0.f;
    float attackTimer = 0.f;
    float attackCooldownTimer = 0.f;
    float hurtTimer = 0.f;
    float invincibleTimer = 0.f;

    int hp = 100;
    int maxHp = 100;

    sf::Keyboard::Key keyLeft;
    sf::Keyboard::Key keyRight;
    sf::Keyboard::Key keyJump;
    sf::Keyboard::Key keyDash;
    sf::Keyboard::Key keyAttack;

private:
    static constexpr float SPEED = 200.f;
    static constexpr float JUMP_FORCE = -550.f;
    static constexpr float GRAVITY = 1200.f;
    static constexpr float MAX_FALL = 600.f;

    static constexpr float DASH_SPEED = 500.f;
    static constexpr float DASH_DURATION = 0.15f;

    static constexpr float ATTACK_DURATION = 0.20f;
    static constexpr float ATTACK_COOLDOWN = 0.28f;

    static constexpr float KNOCKBACK_X = 380.f;
    static constexpr float KNOCKBACK_Y = -260.f;
    static constexpr float HURT_STUN_TIME = 0.18f;
    static constexpr float INVINCIBLE_TIME = 0.35f;

private:
    void loadSprites();
    void applyGravity(float dt);
    void resolveCollisions(const std::vector<sf::FloatRect>& platforms);
    void updateAnimationState();
    void syncSpritePosition();
};