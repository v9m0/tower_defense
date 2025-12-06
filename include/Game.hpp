#pragma once
#include <SFML/Graphics.hpp>
#include "Types.hpp"
#include "ResourceHolder.hpp"
#include "Enemy.hpp"
#include "Tower.hpp"
#include "Projectile.hpp"
#include "Base.hpp"
#include "Level.hpp"
#include "UI.hpp"
#include "Records.hpp"
#include <vector>
#include <memory>
#include <random>
#include <string>
#include <cmath>
#include <algorithm>


// варианты улучшения башни
enum class UpgradeOption { Rate = 0, Special = 1, Sell = 2 };


// основной класс игры
struct Game {
    sf::RenderWindow window{sf::VideoMode(1280,720),"Tower Defense", sf::Style::Default};
    ResourceHolder res;
    GameState state{GameState::Menu};

    Level level;
    Base base;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Tower>> towers;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    Tower* selectedTower{nullptr};
    sf::Vector2f placementMenuPos{};
    int placementPadIndex{-1};
    sf::View worldView;
    sf::View uiView;
    sf::View offscreenWorldView;
    sf::View offscreenUIView;
    sf::RenderTexture frameBuffer;
    bool frameBufferReady{false};

    HUD hud;
    MenuUI menu;
    GameOverUI over;
    RecordsUI recUI;
    TowerSelectionUI towerMenu;
    TowerUpgradeUI upgradeMenu;
    PauseUI pauseUI;
    Tower* upgradeTarget{nullptr};
    Records records;

    sf::Font font;
    sf::Clock clock;
    int score{0};
    int coins{90};
    float spawnTimer{0.f};
    float spawnEvery{2.0f};
    int waveCount{0};
    const int towerUpgradeCost{20};
    std::mt19937 rng{std::random_device{}()};

    Game();

    // сброс состояния для новой сессии
    void resetGame();
    // главный цикл
    void run();
    // обработка очереди событий окна
    void handleEvents();

    // реакция на левый клик в игровом мире
    void handleLeftClick(const sf::Vector2f& worldPos);
    // открыть меню выбора типа башни
    void openPlacementMenu(const sf::Vector2f& worldPos);
    // открыть меню улучшений выбранной башни
    void openUpgradeMenu(Tower& tower);
    // открыть меню улучшения базы
    void openBaseMenu();
    // построить подписи для опций апгрейда
    std::vector<std::string> buildUpgradeLabels(const Tower& tower) const;
    // применить выбранную опцию улучшения
    void applyUpgradeChoice(UpgradeOption option);
    // обработка выбора пункта меню базы
    void handleBaseMenuSelection(int index);
    // поддержка letterbox при изменении окна
    void applyLetterboxView(unsigned width, unsigned height);
    // установка выбранного типа башни на площадку
    void placeTowerChoice(TowerType type);
    // начисление награды за убитого врага
    void checkEnemyDefeat(Enemy& enemy);
    // поиск врага рядом с точкой
    Enemy* findEnemyNear(const sf::Vector2f& pos, float radius = 18.f);
    // применение эффектов от попадания луча
    void handleBeamDamage(const Projectile& projectile, Enemy& target);
    // обработка попадания по цели в данный кадр
    void processBeamHit(const Projectile& projectile);
    // попытка улучшить здоровье базы
    void tryUpgradeBaseHp();
    // шаг обновления всей игры
    void update(float dt);
    // кадр отрисовки всего состояния
    void draw();
    // рисование сетки уровня и площадок
    void drawLevel(sf::RenderTarget& target);
    // вывод уровней башен поверх спрайтов
    void drawTowerLevels(sf::RenderTarget& target);
    // подсветка выбранной башни
    void drawSelection(sf::RenderTarget& target, const Tower& tower);
    // подсказки на экране
    void drawHelp(sf::RenderTarget& target);
    // вход в паузу
    void enterPause();
    // выход из паузы
    void resumeFromPause();
    // переход в меню из паузы
    void exitToMenuFromPause();
    // преобразование координат мира в ui пространство
    sf::Vector2f worldToUI(const sf::Vector2f& world) const;

    // поиск башни в радиусе клика
    Tower* getTowerAt(const sf::Vector2f& pos) {
        float radius = level.hasGrid ? level.tileSize * 0.5f : 18.f;
        for (auto& tower : towers) {
            float dist = std::hypot(tower->pos.x - pos.x, tower->pos.y - pos.y);
            if (dist <= radius) return tower.get();
        }
        return nullptr;
    }
};
