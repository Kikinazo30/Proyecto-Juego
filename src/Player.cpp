#include "Player.h"
#include <algorithm>

Player::Player(PlayerID id, sf::Vector2f startPos, sf::Color color)
    : id(id), position(startPos), velocity(0.f, 0.f),
    onGround(false), canDash(true), isDashing(false), dashTimer(0.f)
{
    shape.setSize({ 40.f, 50.f });
    shape.setFillColor(color);
    shape.setPosition(position);

    if (id == PlayerID::One) {
        keyLeft = sf::Keyboard::Key::A;
        keyRight = sf::Keyboard::Key::D;
        keyJump = sf::Keyboard::Key::Space;
        keyDash = sf::Keyboard::Key::LShift;
    }
    else {
        keyLeft = sf::Keyboard::Key::Left;
        keyRight = sf::Keyboard::Key::Right;
        keyJump = sf::Keyboard::Key::Enter;
        keyDash = sf::Keyboard::Key::RShift;
    }
}

void Player::handleInput() {
    if (sf::Keyboard::isKeyPressed(keyDash) && canDash && !isDashing) {
        isDashing = true;
        canDash = false;
        dashTimer = DASH_DURATION;

        float dirX = 0.f;
        if (sf::Keyboard::isKeyPressed(keyLeft))  dirX = -1.f;
        if (sf::Keyboard::isKeyPressed(keyRight)) dirX = 1.f;
        if (dirX == 0.f) dirX = (velocity.x >= 0.f) ? 1.f : -1.f;

        velocity.x = dirX * DASH_SPEED;
        velocity.y = 0.f;
    }

    if (!isDashing) {
        velocity.x = 0.f;
        if (sf::Keyboard::isKeyPressed(keyLeft))  velocity.x = -SPEED;
        if (sf::Keyboard::isKeyPressed(keyRight)) velocity.x = SPEED;

        if (sf::Keyboard::isKeyPressed(keyJump) && onGround) {
            velocity.y = JUMP_FORCE;
            onGround = false;
        }
    }
}

void Player::applyGravity(float dt) {
    if (!isDashing) {
        velocity.y += GRAVITY * dt;
        velocity.y = std::min(velocity.y, MAX_FALL);
    }
}

void Player::resolveCollisions(const std::vector<sf::FloatRect>& platforms) {
    onGround = false;
    sf::FloatRect bounds = getBounds();

    for (const auto& platform : platforms) {
        if (!bounds.findIntersection(platform)) continue;

        float overlapLeft = (bounds.position.x + bounds.size.x) - platform.position.x;
        float overlapRight = (platform.position.x + platform.size.x) - bounds.position.x;
        float overlapTop = (bounds.position.y + bounds.size.y) - platform.position.y;
        float overlapBottom = (platform.position.y + platform.size.y) - bounds.position.y;

        float minOverlapX = std::min(overlapLeft, overlapRight);
        float minOverlapY = std::min(overlapTop, overlapBottom);

        if (minOverlapY < minOverlapX) {
            if (overlapTop < overlapBottom) {
                position.y -= overlapTop;
                velocity.y = 0.f;
                onGround = true;
                canDash = true;
            }
            else {
                position.y += overlapBottom;
                velocity.y = 0.f;
            }
        }
        else {
            if (overlapLeft < overlapRight) {
                position.x -= overlapLeft;
            }
            else {
                position.x += overlapRight;
            }
            velocity.x = 0.f;
        }

        bounds = getBounds();
    }
}

void Player::update(float dt, const std::vector<sf::FloatRect>& platforms) {
    if (isDashing) {
        dashTimer -= dt;
        if (dashTimer <= 0.f) {
            isDashing = false;
            velocity.x = (velocity.x > 0.f) ? SPEED : -SPEED;
        }
    }

    applyGravity(dt);

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    shape.setPosition(position);
    resolveCollisions(platforms);
    shape.setPosition(position);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Player::getBounds() const {
    return { position, shape.getSize() };
}

sf::Vector2f Player::getPosition() const {
    return position;
}