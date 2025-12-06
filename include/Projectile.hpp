#pragma once
#include "Entity.hpp"
#include <cmath>
#include "Types.hpp"


// снаряд-луч от башни к врагу
struct Projectile : public Entity {
    sf::Vector2f start{};
    sf::Vector2f end{};
    sf::Color color{sf::Color::Yellow};
    float width{5.f};
    float damage{2.f};
    float ttl{0.12f};
    TowerType type{TowerType::Normal};
    float slowMultiplier{0.6f};
    float slowDuration{0.f};
    float freezeDuration{0.f};
    float poisonDuration{0.f};
    float poisonTotalDamage{0.f};

    // уменьшение времени жизни
    void update(float dt) override;

    // рисование луча между началом и концом
    void draw(sf::RenderTarget& win) override;

    // проверка завершения существования
    bool dead() const;
};
