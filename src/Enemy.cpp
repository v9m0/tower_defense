#include "Enemy.hpp"


Enemy::Enemy() {
    shape.setRadius(12.f);
    shape.setOrigin(12,12);
    shape.setFillColor(sf::Color::Red);
}


void Enemy::setTexture(sf::Texture& tex, float targetSize) {
    sprite.setTexture(tex);
    sprite.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
    float maxDim = static_cast<float>(std::max(tex.getSize().x, tex.getSize().y));
    float scale = targetSize / std::max(1.f, maxDim);
    sprite.setScale(scale, scale);
    sprite.setPosition(pos);
}


void Enemy::makeBoss() {
    isBoss = true;
    sf::CircleShape tri;
    tri.setPointCount(3);
    tri.setRadius(18.f);
    tri.setOrigin(18.f,18.f);
    tri.setFillColor(sf::Color(250, 220, 90));
    shape = tri;
}


void Enemy::update(float dt) {
    if (!alive) return;
    updateStatus(dt);
    const float epsilon = 4.f;
    while (!path.empty()) {
        sf::Vector2f target = path.front();
        sf::Vector2f dir = target - pos;
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len < epsilon) {
            path.pop_front();
            continue;
        }
        if (!frozen) {
            dir /= len;
            float moveSpeed = baseSpeed * currentSlowMultiplier;
            pos += dir * moveSpeed * dt;
        }
        break;
    }
    shape.setPosition(pos);
    if (sprite.getTexture()) {
        sprite.setPosition(pos);
    }
}


void Enemy::draw(sf::RenderTarget& win) {
    if (!alive) return;
    if (sprite.getTexture()) {
        win.draw(sprite);
    } else {
        win.draw(shape);
    }
}


void Enemy::takeDamage(float amount) {
    hp -= amount * damageTakenMultiplier;
    if (hp <= 0.f) {
        hp = 0.f;
        alive = false;
    }
}


void Enemy::applySlow(float multiplier, float duration) {
    currentSlowMultiplier = std::min(currentSlowMultiplier, multiplier);
    slowTimer = std::max(slowTimer, duration);
}


void Enemy::applyCryoHit(float multiplier, float slowDuration, float freezeDuration) {
    constexpr int stacksForFreeze = 4;
    if (frozen) {
        freezeTimer = std::max(freezeTimer, freezeDuration);
        return;
    }
    if (cryoStackTimer > 0.f) {
        cryoStacks++;
    } else {
        cryoStacks = 1;
    }
    cryoStackTimer = slowDuration;
    if (cryoStacks >= stacksForFreeze) {
        triggerFreeze(freezeDuration);
        cryoStacks = 0;
        cryoStackTimer = 0.f;
    } else {
        applySlow(multiplier, slowDuration);
    }
}


void Enemy::triggerFreeze(float duration) {
    frozen = true;
    freezeTimer = duration;
    damageTakenMultiplier = 2.f;
    currentSlowMultiplier = 0.f;
}


void Enemy::applyPoison(float totalDamage, float duration) {
    if (duration <= 0.f) return;
    poisonTimer = duration;
    poisonDps = totalDamage / duration;
}


void Enemy::updateStatus(float dt) {
    if (poisonTimer > 0.f) {
        float dmg = poisonDps * dt;
        takeDamage(dmg);
        poisonTimer -= dt;
        if (poisonTimer <= 0.f) {
            poisonTimer = 0.f;
            poisonDps = 0.f;
        }
    }

    if (cryoStackTimer > 0.f) {
        cryoStackTimer -= dt;
        if (cryoStackTimer <= 0.f) {
            cryoStackTimer = 0.f;
            cryoStacks = 0;
        }
    }

    if (slowTimer > 0.f) {
        slowTimer -= dt;
        if (slowTimer <= 0.f) {
            slowTimer = 0.f;
            currentSlowMultiplier = 1.f;
        }
    }

    if (frozen) {
        freezeTimer -= dt;
        if (freezeTimer <= 0.f) {
            frozen = false;
            damageTakenMultiplier = 1.f;
            currentSlowMultiplier = 1.f;
        }
    }

    sf::Color color;
    if (frozen) color = sf::Color(170, 220, 255);
    else if (poisonTimer > 0.f) color = sf::Color(180, 120, 80);
    else if (currentSlowMultiplier < 1.f) color = sf::Color(220, 220, 255);
    else color = sf::Color::White;

    if (sprite.getTexture()) {
        sprite.setColor(color);
    } else {
        shape.setFillColor(color == sf::Color::White ? sf::Color::Red : color);
    }
}
