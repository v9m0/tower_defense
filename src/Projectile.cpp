#include "Projectile.hpp"

void Projectile::update(float dt) {
    // уменьшаем время жизни луча
    ttl -= dt;
}

void Projectile::draw(sf::RenderTarget& win) {
    // рисуем прямоугольный луч между началом и концом
    sf::Vector2f dir = end - start;
    float length = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (length <= 0.1f) return;
    sf::RectangleShape beam({length, width});
    beam.setOrigin(0.f, width * 0.5f);
    beam.setPosition(start);
    const float PI = 3.14159265f;
    float angle = std::atan2(dir.y, dir.x) * 180.f / PI;
    beam.setRotation(angle);
    beam.setFillColor(color);
    win.draw(beam);
}

bool Projectile::dead() const {
    return ttl <= 0.f;
}
