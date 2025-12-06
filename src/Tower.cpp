#include "Tower.hpp"


Tower::Tower() {
    // базовая форма башни без текстур
    shape.setSize({24,24});
    shape.setOrigin(12,12);
    shape.setFillColor(sf::Color::Cyan);
}


void Tower::configure(TowerType t, float tileSize) {
    // установка параметров под выбранный тип
    type = t;
    level = 1;
    totalUpgradeLevels = 0;
    rateUpgradeLevel = 0;
    specialUpgradeLevel = 0;
    totalInvested = 30;
    fireTimer = 0.f;
    shape.setSize({tileSize - 8.f, tileSize - 8.f});
    shape.setOrigin((tileSize - 8.f)/2.f, (tileSize - 8.f)/2.f);
    if (type == TowerType::Normal) {
        range = 160.f;
        fireCooldown = 0.4f;
        damage = 5.f;
        projectileSpeed = 420.f;
        splashRadius = 0.f;
        slowMultiplier = 0.6f;
        slowDuration = 0.f;
        freezeDuration = 0.f;
        poisonDuration = 0.f;
        poisonTotalDamage = 0.f;
        shape.setFillColor(sf::Color(220, 70, 70));
    } else if (type == TowerType::Cryo) {
        range = 150.f;
        fireCooldown = 0.7f;
        damage = 3.f;
        projectileSpeed = 350.f;
        splashRadius = 0.f;
        slowMultiplier = 0.5f;
        slowDuration = 2.0f;
        freezeDuration = 2.0f;
        poisonDuration = 0.f;
        poisonTotalDamage = 0.f;
        shape.setFillColor(sf::Color(80, 160, 255));
    } else if (type == TowerType::Poison) {
        range = 170.f;
        fireCooldown = 0.6f;
        damage = 2.5f;
        projectileSpeed = 360.f;
        splashRadius = 0.f;
        slowMultiplier = 0.6f;
        slowDuration = 0.f;
        freezeDuration = 0.f;
        poisonDuration = 3.f;
        poisonTotalDamage = 6.f;
        shape.setFillColor(sf::Color(90, 200, 90));
    }
}


void Tower::update(float dt) {
    // таймер стрельбы и вспышки
    fireTimer -= dt;
    flashTimer = std::max(0.f, flashTimer - dt);
    shape.setPosition(pos);
    baseSprite.setPosition(pos);
    gunSprite.setPosition(pos);
    if (flashTimer <= 0.f) {
        flashSprite.setPosition(pos);
    }
}


void Tower::draw(sf::RenderTarget& win) {
    // приоритет спрайтов над геометрией
    if (baseSprite.getTexture()) win.draw(baseSprite);
    else win.draw(shape);
    if (gunSprite.getTexture()) win.draw(gunSprite);
    if (flashSprite.getTexture() && flashTimer > 0.f) win.draw(flashSprite);
}


void Tower::setBaseTexture(sf::Texture& tex, float tileSize) {
    // масштаб основания под размер клетки
    baseSprite.setTexture(tex);
    baseSprite.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
    float target = tileSize - 4.f;
    baseSprite.setScale(target / static_cast<float>(tex.getSize().x),
                        target / static_cast<float>(tex.getSize().y));
    baseSprite.setPosition(pos);
}


void Tower::setGunTexture(sf::Texture& tex, float tileSize) {
    // масштаб пушки под клетку
    gunSprite.setTexture(tex);
    gunSprite.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
    float target = tileSize * 0.9f;
    gunSprite.setScale(target / static_cast<float>(tex.getSize().x),
                       target / static_cast<float>(tex.getSize().y));
    gunSprite.setPosition(pos);
}


void Tower::setFlashTexture(sf::Texture& tex, TowerType towerType) {
    if (towerType != TowerType::Normal) return;
    flashSprite.setTexture(tex);
    flashSprite.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
}


std::optional<Projectile> Tower::tryShoot(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    // выбираем ближайшую цель в радиусе
    if (fireTimer > 0.f) return std::nullopt;
    const Enemy* target = nullptr;
    float bestDist2 = range * range;
    for (auto& ePtr : enemies) {
        if (!ePtr->alive) continue;
        sf::Vector2f d = ePtr->pos - pos;
        float d2 = d.x * d.x + d.y * d.y;
        if (d2 <= bestDist2) {
            bestDist2 = d2;
            target = ePtr.get();
        }
    }
    if (!target) return std::nullopt;

    // сброс таймера и подготовка луча
    fireTimer = fireCooldown;
    Projectile p;
    sf::Vector2f dir = target->pos - pos;
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    sf::Vector2f normDir = len > 0 ? dir / len : sf::Vector2f{1.f, 0.f};
    const float muzzleOffset = 15.f;
    sf::Vector2f muzzlePos = pos + normDir * muzzleOffset;
    p.start = muzzlePos;
    p.end = target->pos;
    p.damage = damage;
    p.type = type;
    p.slowMultiplier = slowMultiplier;
    p.slowDuration = slowDuration;
    p.freezeDuration = freezeDuration;
    p.poisonDuration = poisonDuration;
    p.poisonTotalDamage = poisonTotalDamage;
    if (type == TowerType::Normal) p.color = sf::Color::Yellow;
    else if (type == TowerType::Cryo) p.color = sf::Color(180, 230, 255);
    else if (type == TowerType::Poison) p.color = sf::Color(160, 250, 120);
    p.width = 2.f;
    float angleDeg = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;
    if (gunSprite.getTexture()) {
        gunSprite.setRotation(angleDeg + 90.f);
    }
    if (flashSprite.getTexture()) {
        // вспышка отображается короткое время на конце ствола
        flashTimer = 0.08f;
        flashSprite.setPosition(muzzlePos);
        flashSprite.setRotation(angleDeg + 90.f);
    }
    return p;
}
