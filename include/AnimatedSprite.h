#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

struct AnimationClip {
    sf::Texture texture;
    int frameCount = 1;
    int frameWidth = 128;
    int frameHeight = 128;
    float frameTime = 0.1f;
    bool loop = true;
};

class AnimatedSprite {
public:
    AnimatedSprite();

    bool addAnimation(
        const std::string& name,
        const std::string& filepath,
        int frameCount,
        float frameTime = 0.1f,
        bool loop = true
    );

    void play(const std::string& name);
    void update(float dt);

    void setPosition(sf::Vector2f pos);
    void setFlipped(bool flipped);
    void setScale(sf::Vector2f scale);

    void draw(sf::RenderWindow& window);

    sf::Vector2f getSize() const;
    bool isPlaying(const std::string& name) const;
    bool hasFinished() const;

private:
    std::unordered_map<std::string, AnimationClip> clips;

    std::string currentClip;
    int currentFrame = 0;
    float frameTimer = 0.f;

    bool flipped = false;
    bool finished = false;

    sf::Vector2f currentScale = { 1.f, 1.f };

    std::unique_ptr<sf::Sprite> sprite;
    bool spriteInitialized = false;

private:
    void applyCurrentFrameRect();
    void applyTransform();
};