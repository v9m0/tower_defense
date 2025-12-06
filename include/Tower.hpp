
#pragma once
#include "Entity.hpp"
#include "Types.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include <vector>
#include <memory>
#include <optional>
#include <cmath>


// башня защищающая базу
struct Tower : public Entity {
    TowerType type{TowerType::Normal};
    float range{140.f};
    float fireCooldown{0.4f}; // секунды
    float fireTimer{0.f};
    int level{1};
    float damage{3.f};
    float projectileSpeed{400.f};
    float splashRadius{0.f};
    float slowMultiplier{0.6f};
    float slowDuration{1.5f};
    float freezeDuration{2.f};
    float poisonDuration{3.f};
    float poisonTotalDamage{6.f};
    int totalUpgradeLevels{0};
    int rateUpgradeLevel{0};
    int specialUpgradeLevel{0};
    int totalInvested{30};
    sf::RectangleShape shape;
    sf::Sprite baseSprite;
    sf::Sprite gunSprite;
    sf::Sprite flashSprite;
    float flashTimer{0.f};

    Tower();

    // настройка параметров по типу и размеру клетки
    void configure(TowerType t, float tileSize);

    void update(float dt) override;
    void draw(sf::RenderTarget& win) override;

    // назначение текстуры основания
    void setBaseTexture(sf::Texture& tex, float tileSize);

    // назначение текстуры пушки
    void setGunTexture(sf::Texture& tex, float tileSize);

    // назначение вспышки выстрела для обычной башни
    void setFlashTexture(sf::Texture& tex, TowerType towerType);

    // попытка выстрелить по ближайшему врагу
    std::optional<Projectile> tryShoot(const std::vector<std::unique_ptr<Enemy>>& enemies);
};
