#pragma once
#include <SFML/Graphics.hpp>

struct Entity {
    sf::Vector2f pos{};
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderTarget& win) = 0;
};
