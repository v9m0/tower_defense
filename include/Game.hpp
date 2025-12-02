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


enum class UpgradeOption { Rate = 0, Special = 1, Sell = 2 };


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

    void resetGame();
    void run();
    void handleEvents();

    void handleLeftClick(const sf::Vector2f& worldPos);
    void openPlacementMenu(const sf::Vector2f& worldPos);
    void openUpgradeMenu(Tower& tower);
    void openBaseMenu();
    std::vector<std::string> buildUpgradeLabels(const Tower& tower) const;
    void applyUpgradeChoice(UpgradeOption option);
    void handleBaseMenuSelection(int index);
    void applyLetterboxView(unsigned width, unsigned height);
    void placeTowerChoice(TowerType type);
    void checkEnemyDefeat(Enemy& enemy);
    Enemy* findEnemyNear(const sf::Vector2f& pos, float radius = 18.f);
    void handleBeamDamage(const Projectile& projectile, Enemy& target);
    void processBeamHit(const Projectile& projectile);
    void tryUpgradeBaseHp();
    void update(float dt);
    void draw();
    void drawLevel(sf::RenderTarget& target);
    void drawTowerLevels(sf::RenderTarget& target);
    void drawSelection(sf::RenderTarget& target, const Tower& tower);
    void drawHelp(sf::RenderTarget& target);
    void enterPause();
    void resumeFromPause();
    void exitToMenuFromPause();
    sf::Vector2f worldToUI(const sf::Vector2f& world) const;

    Tower* getTowerAt(const sf::Vector2f& pos) {
        float radius = level.hasGrid ? level.tileSize * 0.5f : 18.f;
        for (auto& tower : towers) {
            float dist = std::hypot(tower->pos.x - pos.x, tower->pos.y - pos.y);
            if (dist <= radius) return tower.get();
        }
        return nullptr;
    }
};
