#include "AnimatedSprite.h"

#include <iostream>
#include <memory>
#include <cmath>

namespace {
    constexpr unsigned int CLEAN_BORDER_SIZE = 4;

    bool isAlmostWhite(const sf::Color& color) {
        return color.a > 0 &&
            color.r >= 245 &&
            color.g >= 245 &&
            color.b >= 245;
    }

    void makePixelTransparent(sf::Image& image, unsigned int x, unsigned int y) {
        sf::Color color = image.getPixel({ x, y });

        if (isAlmostWhite(color)) {
            color.a = 0;
            image.setPixel({ x, y }, color);
        }
    }

    void cleanFrameWhiteBorders(sf::Image& image, int frameCount) {
        sf::Vector2u imageSize = image.getSize();

        if (frameCount <= 0 || imageSize.x == 0 || imageSize.y == 0) {
            return;
        }

        unsigned int frameWidth = imageSize.x / static_cast<unsigned int>(frameCount);
        unsigned int frameHeight = imageSize.y;

        if (frameWidth == 0 || frameHeight == 0) {
            return;
        }

        for (int frame = 0; frame < frameCount; frame++) {
            unsigned int frameStartX = static_cast<unsigned int>(frame) * frameWidth;
            unsigned int frameEndX = frameStartX + frameWidth - 1;

            for (unsigned int y = 0; y < frameHeight; y++) {
                for (unsigned int i = 0; i < CLEAN_BORDER_SIZE; i++) {
                    if (frameStartX + i < imageSize.x) {
                        makePixelTransparent(image, frameStartX + i, y);
                    }

                    if (frameEndX >= i && frameEndX - i < imageSize.x) {
                        makePixelTransparent(image, frameEndX - i, y);
                    }
                }
            }

            for (unsigned int x = frameStartX; x <= frameEndX && x < imageSize.x; x++) {
                for (unsigned int i = 0; i < CLEAN_BORDER_SIZE; i++) {
                    if (i < frameHeight) {
                        makePixelTransparent(image, x, i);
                    }

                    if (frameHeight - 1 >= i) {
                        makePixelTransparent(image, x, frameHeight - 1 - i);
                    }
                }
            }
        }
    }
}

AnimatedSprite::AnimatedSprite() {
}

bool AnimatedSprite::addAnimation(
    const std::string& name,
    const std::string& filepath,
    int frameCount,
    float frameTime,
    bool loop
) {
    if (frameCount <= 0) {
        std::cerr << "FrameCount invalido en animacion: " << name << std::endl;
        return false;
    }

    sf::Image image;

    if (!image.loadFromFile(filepath)) {
        std::cerr << "No se pudo cargar la imagen: " << filepath << std::endl;
        return false;
    }

    cleanFrameWhiteBorders(image, frameCount);

    AnimationClip clip;

    if (!clip.texture.loadFromImage(image)) {
        std::cerr << "No se pudo crear textura desde imagen: " << filepath << std::endl;
        return false;
    }

    clip.texture.setSmooth(false);
    clip.texture.setRepeated(false);

    sf::Vector2u texSize = clip.texture.getSize();

    clip.frameCount = frameCount;
    clip.frameWidth = static_cast<int>(texSize.x) / frameCount;
    clip.frameHeight = static_cast<int>(texSize.y);
    clip.frameTime = frameTime;
    clip.loop = loop;

    clips[name] = std::move(clip);

    if (!spriteInitialized) {
        sprite = std::make_unique<sf::Sprite>(clips[name].texture);
        currentClip = name;
        currentFrame = 0;
        frameTimer = 0.f;
        finished = false;
        spriteInitialized = true;

        applyCurrentFrameRect();
        applyTransform();
    }

    return true;
}

void AnimatedSprite::play(const std::string& name) {
    if (!spriteInitialized) {
        return;
    }

    if (clips.find(name) == clips.end()) {
        return;
    }

    if (currentClip == name) {
        return;
    }

    currentClip = name;
    currentFrame = 0;
    frameTimer = 0.f;
    finished = false;

    auto& clip = clips[currentClip];

    sprite->setTexture(clip.texture, true);

    applyCurrentFrameRect();
    applyTransform();
}

void AnimatedSprite::update(float dt) {
    if (!spriteInitialized) {
        return;
    }

    if (clips.find(currentClip) == clips.end()) {
        return;
    }

    auto& clip = clips[currentClip];

    if (finished && !clip.loop) {
        return;
    }

    frameTimer += dt;

    if (frameTimer >= clip.frameTime) {
        frameTimer -= clip.frameTime;
        currentFrame++;

        if (currentFrame >= clip.frameCount) {
            if (clip.loop) {
                currentFrame = 0;
            }
            else {
                currentFrame = clip.frameCount - 1;
                finished = true;
            }
        }

        applyCurrentFrameRect();
    }
}

void AnimatedSprite::setPosition(sf::Vector2f pos) {
    if (!spriteInitialized) {
        return;
    }

    sprite->setPosition(pos);
}

void AnimatedSprite::setFlipped(bool f) {
    if (!spriteInitialized) {
        return;
    }

    flipped = f;
    applyTransform();
}

void AnimatedSprite::setScale(sf::Vector2f scale) {
    if (!spriteInitialized) {
        return;
    }

    currentScale = {
        std::abs(scale.x),
        std::abs(scale.y)
    };

    applyTransform();
}

void AnimatedSprite::draw(sf::RenderWindow& window) {
    if (!spriteInitialized) {
        return;
    }

    window.draw(*sprite);
}

sf::Vector2f AnimatedSprite::getSize() const {
    if (clips.find(currentClip) == clips.end()) {
        return { 0.f, 0.f };
    }

    const auto& clip = clips.at(currentClip);

    return {
        static_cast<float>(clip.frameWidth) * currentScale.x,
        static_cast<float>(clip.frameHeight) * currentScale.y
    };
}

bool AnimatedSprite::isPlaying(const std::string& name) const {
    return currentClip == name;
}

bool AnimatedSprite::hasFinished() const {
    return finished;
}

void AnimatedSprite::applyCurrentFrameRect() {
    if (!spriteInitialized) {
        return;
    }

    if (clips.find(currentClip) == clips.end()) {
        return;
    }

    auto& clip = clips[currentClip];

    sprite->setTextureRect(sf::IntRect(
        { currentFrame * clip.frameWidth, 0 },
        { clip.frameWidth, clip.frameHeight }
    ));
}

void AnimatedSprite::applyTransform() {
    if (!spriteInitialized) {
        return;
    }

    if (clips.find(currentClip) == clips.end()) {
        return;
    }

    auto& clip = clips[currentClip];

    if (flipped) {
        sprite->setOrigin({ static_cast<float>(clip.frameWidth), 0.f });
        sprite->setScale({ -currentScale.x, currentScale.y });
    }
    else {
        sprite->setOrigin({ 0.f, 0.f });
        sprite->setScale({ currentScale.x, currentScale.y });
    }
}