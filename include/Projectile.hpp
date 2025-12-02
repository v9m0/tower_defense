#pragma once
#include "Entity.hpp"
#include <cmath>
#include "Types.hpp"


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

    void update(float dt) override;

    void draw(sf::RenderTarget& win) override;

    bool dead() const;
};
