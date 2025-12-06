#pragma once
#include <SFML/Graphics.hpp>

// базовый класс для всех объектов на сцене
struct Entity {
    sf::Vector2f pos{};
    // виртуальный обновляющий шаг с дельтой времени
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
    // отрисовка сущности в переданную цель
    virtual void draw(sf::RenderTarget& win) = 0;
};
