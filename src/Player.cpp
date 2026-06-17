#include "Player.h"
#include <algorithm>
#include <cmath>

namespace {
    constexpr sf::Vector2f PLAYER_HITBOX_SIZE = { 34.f, 44.f };

    constexpr float SPRITE_SCALE_KAI = 0.82f;
    constexpr float SPRITE_SCALE_RUNE = 0.82f;

    constexpr float SPRITE_FRAME_WIDTH = 128.f;
    constexpr float SPRITE_FRAME_HEIGHT = 128.f;

    constexpr float SPRITE_FOOT_OFFSET_Y = 2.f;
}

Player::Player(PlayerID id, sf::Vector2f startPos, sf::Color color)
    : id(id),
    position(startPos),
    velocity(0.f, 0.f)
{
    shape.setSize(PLAYER_HITBOX_SIZE);
    shape.setFillColor(color);
    shape.setPosition(position);

    if (id == PlayerID::One) {
        keyLeft = sf::Keyboard::Key::A;
        keyRight = sf::Keyboard::Key::D;
        keyJump = sf::Keyboard::Key::Space;
        keyDash = sf::Keyboard::Key::LShift;
        keyAttack = sf::Keyboard::Key::F;
        facingLeft = false;
    }
    else {
        keyLeft = sf::Keyboard::Key::Left;
        keyRight = sf::Keyboard::Key::Right;
        keyJump = sf::Keyboard::Key::Enter;
        keyDash = sf::Keyboard::Key::RShift;
        keyAttack = sf::Keyboard::Key::RControl;
        facingLeft = true;
    }

    loadSprites();
}

void Player::loadSprites() {
    bool ok = true;

    if (id == PlayerID::One) {
        ok &= anim.addAnimation("idle", "assets/images/kai/kai_moverse_strip.png", 4, 0.18f, true);
        ok &= anim.addAnimation("move", "assets/images/kai/kai_moverse_strip.png", 4, 0.10f, true);
        ok &= anim.addAnimation("jump", "assets/images/kai/kai_saltar_strip.png", 3, 0.10f, false);
        ok &= anim.addAnimation("dash", "assets/images/kai/kai_dashearse_strip.png", 3, 0.06f, false);
        ok &= anim.addAnimation("attack", "assets/images/kai/kai_golpear_strip.png", 3, 0.08f, false);
    }
    else {
        ok &= anim.addAnimation("idle", "assets/images/rune/rune_moverse_strip.png", 4, 0.18f, true);
        ok &= anim.addAnimation("move", "assets/images/rune/rune_moverse_strip.png", 4, 0.10f, true);
        ok &= anim.addAnimation("jump", "assets/images/rune/rune_saltar_strip.png", 3, 0.10f, false);
        ok &= anim.addAnimation("dash", "assets/images/rune/rune_dashearse_strip.png", 3, 0.06f, false);
        ok &= anim.addAnimation("attack", "assets/images/rune/rune_golpear_strip.png", 3, 0.08f, false);
    }

    hasSprite = ok;

    if (hasSprite) {
        anim.play("idle");

        if (id == PlayerID::One) {
            anim.setScale({ SPRITE_SCALE_KAI, SPRITE_SCALE_KAI });
        }
        else {
            anim.setScale({ SPRITE_SCALE_RUNE, SPRITE_SCALE_RUNE });
        }

        anim.setFlipped(facingLeft);
        syncSpritePosition();
    }
}

void Player::reset(sf::Vector2f startPos) {
    position = startPos;
    velocity = { 0.f, 0.f };

    onGround = false;
    canDash = true;
    isDashing = false;
    isAttacking = false;

    attackAlreadyHit = false;
    wasAttackPressed = false;
    wasDashPressed = false;

    jumpEvent = false;
    dashEvent = false;
    attackEvent = false;
    hitEvent = false;

    dashTimer = 0.f;
    attackTimer = 0.f;
    attackCooldownTimer = 0.f;
    hurtTimer = 0.f;
    invincibleTimer = 0.f;

    hp = maxHp;

    if (id == PlayerID::One) {
        facingLeft = false;
    }
    else {
        facingLeft = true;
    }

    shape.setPosition(position);

    if (hasSprite) {
        anim.play("idle");
        anim.setFlipped(facingLeft);
        syncSpritePosition();
    }
}

void Player::handleInput() {
    bool attackPressed = sf::Keyboard::isKeyPressed(keyAttack);
    bool dashPressed = sf::Keyboard::isKeyPressed(keyDash);

    if (isDead()) {
        wasAttackPressed = attackPressed;
        wasDashPressed = dashPressed;
        return;
    }

    if (hurtTimer > 0.f) {
        wasAttackPressed = attackPressed;
        wasDashPressed = dashPressed;
        return;
    }

    if (
        attackPressed &&
        !wasAttackPressed &&
        !isAttacking &&
        !isDashing &&
        attackCooldownTimer <= 0.f
        ) {
        isAttacking = true;
        attackAlreadyHit = false;
        attackTimer = ATTACK_DURATION;
        attackCooldownTimer = ATTACK_COOLDOWN;
        attackEvent = true;
    }

    if (
        dashPressed &&
        !wasDashPressed &&
        canDash &&
        !isDashing
        ) {
        isDashing = true;
        canDash = false;
        dashTimer = DASH_DURATION;
        dashEvent = true;

        float dirX = 0.f;

        if (sf::Keyboard::isKeyPressed(keyLeft)) {
            dirX = -1.f;
        }

        if (sf::Keyboard::isKeyPressed(keyRight)) {
            dirX = 1.f;
        }

        if (dirX == 0.f) {
            dirX = facingLeft ? -1.f : 1.f;
        }

        velocity.x = dirX * DASH_SPEED;
        velocity.y = 0.f;
    }

    if (!isDashing) {
        velocity.x = 0.f;

        if (sf::Keyboard::isKeyPressed(keyLeft)) {
            velocity.x = -SPEED;
            facingLeft = true;
        }

        if (sf::Keyboard::isKeyPressed(keyRight)) {
            velocity.x = SPEED;
            facingLeft = false;
        }

        if (sf::Keyboard::isKeyPressed(keyJump) && onGround) {
            velocity.y = JUMP_FORCE;
            onGround = false;
            jumpEvent = true;
        }
    }

    wasAttackPressed = attackPressed;
    wasDashPressed = dashPressed;
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
        if (!bounds.findIntersection(platform)) {
            continue;
        }

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

void Player::updateAnimationState() {
    if (!hasSprite) {
        return;
    }

    if (isDashing) {
        if (!anim.isPlaying("dash")) {
            anim.play("dash");
        }
    }
    else if (isAttacking) {
        if (!anim.isPlaying("attack")) {
            anim.play("attack");
        }
    }
    else if (!onGround) {
        if (!anim.isPlaying("jump")) {
            anim.play("jump");
        }
    }
    else if (std::abs(velocity.x) > 1.f) {
        if (!anim.isPlaying("move")) {
            anim.play("move");
        }
    }
    else {
        if (!anim.isPlaying("idle")) {
            anim.play("idle");
        }
    }

    anim.setFlipped(facingLeft);
}

void Player::syncSpritePosition() {
    if (!hasSprite) {
        return;
    }

    float currentScale = (id == PlayerID::One) ? SPRITE_SCALE_KAI : SPRITE_SCALE_RUNE;

    sf::Vector2f spriteSize = {
        SPRITE_FRAME_WIDTH * currentScale,
        SPRITE_FRAME_HEIGHT * currentScale
    };

    sf::Vector2f hitboxSize = shape.getSize();

    float offsetX = (spriteSize.x - hitboxSize.x) / 2.f;
    float offsetY = spriteSize.y - hitboxSize.y - SPRITE_FOOT_OFFSET_Y;

    anim.setPosition({
        position.x - offsetX,
        position.y - offsetY
        });
}

void Player::update(float dt, const std::vector<sf::FloatRect>& platforms) {
    if (isDead()) {
        velocity.x = 0.f;
    }

    if (hurtTimer > 0.f) {
        hurtTimer -= dt;

        if (hurtTimer < 0.f) {
            hurtTimer = 0.f;
        }
    }

    if (invincibleTimer > 0.f) {
        invincibleTimer -= dt;

        if (invincibleTimer < 0.f) {
            invincibleTimer = 0.f;
        }
    }

    if (attackCooldownTimer > 0.f) {
        attackCooldownTimer -= dt;

        if (attackCooldownTimer < 0.f) {
            attackCooldownTimer = 0.f;
        }
    }

    if (isAttacking) {
        attackTimer -= dt;

        if (attackTimer <= 0.f) {
            isAttacking = false;
            attackTimer = 0.f;
            attackAlreadyHit = false;
        }
    }

    if (isDashing) {
        dashTimer -= dt;

        if (dashTimer <= 0.f) {
            isDashing = false;
            dashTimer = 0.f;
            velocity.x = facingLeft ? -SPEED : SPEED;
        }
    }

    applyGravity(dt);

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    shape.setPosition(position);

    resolveCollisions(platforms);

    shape.setPosition(position);

    updateAnimationState();

    if (hasSprite) {
        anim.update(dt);
        syncSpritePosition();
    }
}

void Player::draw(sf::RenderWindow& window) {
    if (hasSprite) {
        anim.draw(window);
    }
    else {
        window.draw(shape);
    }
}

sf::FloatRect Player::getBounds() const {
    return { position, shape.getSize() };
}

sf::FloatRect Player::getAttackBounds() const {
    sf::Vector2f attackSize = { 48.f, 34.f };

    float attackX = 0.f;

    if (facingLeft) {
        attackX = position.x - attackSize.x;
    }
    else {
        attackX = position.x + shape.getSize().x;
    }

    float attackY = position.y + 5.f;

    return {
        { attackX, attackY },
        attackSize
    };
}

sf::Vector2f Player::getPosition() const {
    return position;
}

bool Player::getIsAttacking() const {
    return isAttacking;
}

bool Player::canHitDuringAttack() const {
    return isAttacking && !attackAlreadyHit;
}

void Player::markAttackHit() {
    attackAlreadyHit = true;
}

void Player::takeDamage(int amount, float knockbackDirection) {
    if (isDead()) {
        return;
    }

    if (invincibleTimer > 0.f) {
        return;
    }

    hp -= amount;

    if (hp < 0) {
        hp = 0;
    }

    isDashing = false;
    dashTimer = 0.f;

    hurtTimer = HURT_STUN_TIME;
    invincibleTimer = INVINCIBLE_TIME;

    velocity.x = knockbackDirection * KNOCKBACK_X;
    velocity.y = KNOCKBACK_Y;

    onGround = false;
    canDash = false;

    hitEvent = true;
}

int Player::getHp() const {
    return hp;
}

int Player::getMaxHp() const {
    return maxHp;
}

bool Player::isDead() const {
    return hp <= 0;
}

float Player::getFacingDirection() const {
    return facingLeft ? -1.f : 1.f;
}

bool Player::consumeJumpEvent() {
    bool value = jumpEvent;
    jumpEvent = false;
    return value;
}

bool Player::consumeDashEvent() {
    bool value = dashEvent;
    dashEvent = false;
    return value;
}

bool Player::consumeAttackEvent() {
    bool value = attackEvent;
    attackEvent = false;
    return value;
}

bool Player::consumeHitEvent() {
    bool value = hitEvent;
    hitEvent = false;
    return value;
}