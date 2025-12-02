#pragma once
#include "Entity.hpp"

struct Base : public Entity {
    float hp{1000.f};
    float maxHp{1000.f};
    sf::RectangleShape shape; // квадрат

    Base();

    void update(float) override;
    void draw(sf::RenderTarget& win) override;

    // определяет клик по базе
    sf::FloatRect bounds() const;

    // подгоняет квадврат под клетку после загрузки
    void setSize(float size);
};
