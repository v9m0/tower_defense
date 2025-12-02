#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <optional>
#include "Types.hpp"

enum class MenuAction { Start, Records, Quit };

void snapTextPosition(sf::Text& txt);
void centerText(sf::Text& txt, float x, float y);

struct HUD {
    sf::Font font;
    sf::RectangleShape topBar;
    sf::RectangleShape bottomBar;
    sf::Text scoreText, coinsText, baseHpText;
    sf::RectangleShape pauseButton;
    sf::Sprite pauseIcon;
    sf::Sprite coinSprite;

    void init(const sf::Font& f);
    void setCoinTexture(sf::Texture& tex);
    void setPauseTexture(sf::Texture& tex);
    void setLayout(const sf::Vector2f& viewSize);
    void update(int score, int coins, float baseHp, float maxHp);
    void draw(sf::RenderTarget& win);
    sf::FloatRect bottomBarBounds() const;
    sf::FloatRect topBarBounds() const;
    sf::FloatRect pauseButtonBounds() const;

private:
    void positionLeftCentered(sf::Text& txt, const sf::Vector2f& anchor);
    void positionRightCentered(sf::Text& txt, const sf::Vector2f& anchor);
    void refreshCoinScale(float targetHeight);
    void refreshPauseIcon();
};

struct MenuUI {
    sf::Font font;
    sf::Text title, start, records, quit;
    float buttonWidth{260.f};
    float buttonHeight{50.f};

    void init(const sf::Font& f);
    void layout(const sf::Vector2f& viewSize);
    void draw(sf::RenderTarget& w);
    std::optional<MenuAction> handleClick(const sf::Vector2f& pos) const;

private:
    sf::FloatRect buttonBounds(const sf::Text& txt) const;
    void drawButton(sf::RenderTarget& w, const sf::Text& txt) const;
};

struct GameOverUI {
    sf::Font font;
    sf::Text title;
    sf::Text back;
    sf::RectangleShape backButton;

    void init(const sf::Font& f);
    void layout(const sf::Vector2f& viewSize);
    bool handleClick(const sf::Vector2f& pos) const;
    void draw(sf::RenderTarget& w);
};

struct RecordsUI {
    sf::Font font;
    sf::Text title;
    std::vector<sf::Text> lines;
    sf::RectangleShape backButton;
    sf::Text backLabel;

    void init(const sf::Font& f);
    void setRecords(const std::vector<int>& r);
    void layout(const sf::Vector2f& viewSize);
    void draw(sf::RenderTarget& w);
    bool handleClick(const sf::Vector2f& pos) const;
};

enum class PauseAction { Resume, Menu };

struct PauseUI {
    sf::Font font;
    sf::Text title;
    sf::RectangleShape resumeButton;
    sf::RectangleShape menuButton;
    sf::Text resumeLabel;
    sf::Text menuLabel;

    void init(const sf::Font& f);
    void layout(const sf::Vector2f& viewSize);
    std::optional<PauseAction> handleClick(const sf::Vector2f& pos) const;
    void draw(sf::RenderTarget& w);

private:
    void configureButton(sf::RectangleShape& button, sf::Text& label, const sf::Vector2f& pos, bool updateReference);
};

struct TowerSelectionUI {
    sf::Font font;
    sf::RectangleShape panel;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> labels;
    bool visible{false};
    sf::Vector2f position;

    void init(const sf::Font& f);
    void open(const sf::Vector2f& worldPos, float tileSize);
    void close();
    std::optional<TowerType> handleClick(const sf::Vector2f& worldPos);
    void draw(sf::RenderTarget& w);
};

struct TowerUpgradeUI {
    sf::Font font;
    sf::RectangleShape panel;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> labels;
    sf::Sprite coinIcon;
    float iconTargetHeight{16.f};
    std::vector<bool> showCoin;
    bool visible{false};

    void init(const sf::Font& f);
    void open(const sf::Vector2f& worldPos, float tileSize, const std::vector<std::string>& options);
    void close();
    std::optional<int> handleClick(const sf::Vector2f& worldPos);
    sf::FloatRect panelBounds() const;
    void draw(sf::RenderTarget& w);
    void setCoinTexture(sf::Texture& tex, float targetHeight);

private:
    void drawCoinIcon(sf::RenderTarget& w, const sf::Text& label);
    void refreshIconScale();
    void positionLabel(sf::Text& label, const sf::RectangleShape& button);
};
