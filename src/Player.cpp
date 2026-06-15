#include "Player.h"
#include <algorithm>

Player::Player(PlayerID id, sf::Vector2f startPos, sf::Color color)
    : id(id), position(startPos), velocity(0.f, 0.f),
      onGround(false), canDash(true), isDashing(false), dashTimer(0.f),
      hp(100), maxHp(100), isAttacking(false), attackTimer(0.f), facingDir(1.f)
{
    shape.setSize({ 40.f, 50.f });
    shape.setPosition(position);

    // --- NUEVO: Cargar los sprites reales desde tus carpetas ---
    if (id == PlayerID::One) {
        if (texture.loadFromFile("assets/images/erick sprit.png")) {
            shape.setTexture(&texture);
        }
        shape.setFillColor(sf::Color::White); // Blanco para que no altere los colores del sprite

        keyLeft = sf::Keyboard::Key::A;
        keyRight = sf::Keyboard::Key::D;
        keyJump = sf::Keyboard::Key::Space;
        keyDash = sf::Keyboard::Key::LShift;
        keyAttack = sf::Keyboard::Key::F; 
    }
    else {
        if (texture.loadFromFile("assets/images/kike sprit.jpg.jpeg")) {
            shape.setTexture(&texture);
        }
        shape.setFillColor(sf::Color::White); 

        keyLeft = sf::Keyboard::Key::Left;
        keyRight = sf::Keyboard::Key::Right;
        keyJump = sf::Keyboard::Key::Enter;
        keyDash = sf::Keyboard::Key::RShift;
        keyAttack = sf::Keyboard::Key::P; 
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
        
        if (sf::Keyboard::isKeyPressed(keyLeft)) {
            velocity.x = -SPEED;
            facingDir = -1.f; 
        }
        if (sf::Keyboard::isKeyPressed(keyRight)) {
            velocity.x = SPEED;
            facingDir = 1.f;
        }

        if (sf::Keyboard::isKeyPressed(keyJump) && onGround) {
            velocity.y = JUMP_FORCE;
            onGround = false;
        }
    }

    if (sf::Keyboard::isKeyPressed(keyAttack) && !isAttacking) {
        isAttacking = true;
        attackTimer = ATTACK_DURATION;
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

    if (isAttacking) {
        attackTimer -= dt;
        if (attackTimer <= 0.f) {
            isAttacking = false;
        }
    }

    applyGravity(dt);

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    shape.setPosition(position);
    resolveCollisions(platforms);
    shape.setPosition(position);

    // --- NUEVO: Voltear el Sprite horizontalmente usando el TextureRect de SFML 3 ---
    sf::Vector2i texSize = static_cast<sf::Vector2i>(texture.getSize());
    if (facingDir == 1.f) {
        // Mirando a la derecha: coordenadas normales
        shape.setTextureRect({ {0, 0}, {texSize.x, texSize.y} });
    } else {
        // Mirando a la izquierda: invertimos el eje X empezando desde el final del ancho
        shape.setTextureRect({ {texSize.x, 0}, {-texSize.x, texSize.y} });
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);

    if (isAttacking) {
        sf::RectangleShape attackVisual(getAttackBounds().size);
        attackVisual.setPosition(getAttackBounds().position);
        attackVisual.setFillColor(sf::Color(255, 255, 255, 120)); 
        window.draw(attackVisual);
    }
}

sf::FloatRect Player::getBounds() const {
    return { position, shape.getSize() };
}

sf::Vector2f Player::getPosition() const {
    return position;
}

void Player::takeDamage(int amount) {
    hp -= amount;
    if (hp < 0) hp = 0;
}

bool Player::getIsAttacking() const {
    return isAttacking;
}

int Player::getHp() const {
    return hp;
}

sf::FloatRect Player::getAttackBounds() const {
    sf::Vector2f attackSize(45.f, 40.f);
    float attackX = 0.f;

    if (facingDir == 1.f) {
        attackX = position.x + shape.getSize().x;
    } else {
        attackX = position.x - attackSize.x;
    }

    float attackY = position.y + (shape.getSize().y / 2.f) - (attackSize.y / 2.f);

    return { {attackX, attackY}, attackSize };
}
