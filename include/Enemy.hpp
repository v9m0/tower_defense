#pragma once
#include "Entity.hpp"
#include <algorithm>
#include <cmath>
#include <deque>

// враг двигающийся по пути к базе
struct Enemy : public Entity {
    float baseSpeed{60.f};
    float hp{10.f};
    float damage{5.f};
    bool alive{true};
    bool rewardGranted{false};
    std::deque<sf::Vector2f> path;
    sf::CircleShape shape;         // круг для фолбека при отсутствии спрайта
    sf::Sprite sprite;             // спрайт
    bool isBoss{false};

    // эффекты
    float slowTimer{0.f};
    float currentSlowMultiplier{1.f};
    int cryoStacks{0};
    float cryoStackTimer{0.f};
    bool frozen{false};
    float freezeTimer{0.f};
    float damageTakenMultiplier{1.f};

    float poisonTimer{0.f};
    float poisonDps{0.f};

    Enemy();

    // установка спрайта и масштабирование под клетку
    void setTexture(sf::Texture& tex, float targetSize);

    // перевод врага в статус босса
    void makeBoss();

    void update(float dt) override;

    void draw(sf::RenderTarget& win) override;

    // получение урона с учётом модификаторов
    void takeDamage(float amount);

    // наложение замедления
    void applySlow(float multiplier, float duration);

    // станлок
    void applyCryoHit(float multiplier, float slowDuration, float freezeDuration);

    // мгновенное замораживание
    void triggerFreeze(float duration);

    // отложенный урон ядом
    void applyPoison(float totalDamage, float duration);

    // обновление таймеров эффектов
    void updateStatus(float dt);
};
