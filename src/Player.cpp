#include "Player.h"
#include <algorithm>

Player::Player(PlayerID id, sf::Vector2f startPos, sf::Color color)
    : id(id), position(startPos), velocity(0.f, 0.f),
      onGround(false), canDash(true), isDashing(false), dashTimer(0.f)
{
    shape.setSize({40.f, 50.f});
    shape.setFillColor(color);
    shape.setPosition(position);

    if (id == PlayerID::One) {
        keyLeft  = sf::Keyboard::A;
        keyRight = sf::Keyboard::D;
        keyJump  = sf::Keyboard::Space;
        keyDash  = sf::Keyboard::LShift;
    } else {
        keyLeft  = sf::Keyboard::Left;
        keyRight = sf::Keyboard::Right;
        keyJump  = sf::Keyboard::Return;
        keyDash  = sf::Keyboard::RShift;
    }
}

void Player::handleInput() {
    if (sf::Keyboard::isKeyPressed(keyDash) && canDash && !isDashing) {
        isDashing = true;
        canDash   = false;
        dashTimer = DASH_DURATION;

        float dirX = 0.f;
        if (sf::Keyboard::isKeyPressed(keyLeft))  dirX = -1.f;
        if (sf::Keyboard::isKeyPressed(keyRight)) dirX =  1.f;
        if (dirX == 0.f) dirX = (velocity.x >= 0.f) ? 1.f : -1.f;

        velocity.x = dirX * DASH_SPEED;
        velocity.y = 0.f;
    }

    if (!isDashing) {
        velocity.x = 0.f;
        if (sf::Keyboard::isKeyPressed(keyLeft))  velocity.x = -SPEED;
        if (sf::Keyboard::isKeyPressed(keyRight)) velocity.x =  SPEED;

        if (sf::Keyboard::isKeyPressed(keyJump) && onGround) {
            velocity.y = JUMP_FORCE;
            onGround   = false;
        }
    }
}

void Player::applyGravity(float dt) {
    if (!isDashing) {
        velocity.y += GRAVITY * dt;
        velocity.y  = std::min(velocity.y, MAX_FALL);
    }
}

void Player::resolveCollisions(const std::vector<sf::FloatRect>& platforms) {
    onGround = false;
    sf::FloatRect bounds = getBounds();

    for (const auto& platform : platforms) {
        if (!bounds.intersects(platform)) continue;

        float overlapLeft   = (bounds.left + bounds.width) - platform.left;
        float overlapRight  = (platform.left + platform.width) - bounds.left;
        float overlapTop    = (bounds.top + bounds.height) - platform.top;
        float overlapBottom = (platform.top + platform.height) - bounds.top;

        float minOverlapX = std::min(overlapLeft, overlapRight);
        float minOverlapY = std::min(overlapTop,  overlapBottom);

        if (minOverlapY < minOverlapX) {
            if (overlapTop < overlapBottom) {
                position.y -= overlapTop;
                velocity.y  = 0.f;
                onGround    = true;
                canDash     = true;
            } else {
                position.y += overlapBottom;
                velocity.y  = 0.f;
            }
        } else {
            if (overlapLeft < overlapRight) {
                position.x -= overlapLeft;
            } else {
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
            isDashing  = false;
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
    return {position.x, position.y, shape.getSize().x, shape.getSize().y};
}

sf::Vector2f Player::getPosition() const {
    return position;
}
