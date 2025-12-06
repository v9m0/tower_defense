#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <optional>
#include "Types.hpp"

// действия главного меню по клику
enum class MenuAction { Start, Records, Quit };

// подгонка координат текста под целые пиксели
void snapTextPosition(sf::Text& txt);
// установка центра текста в заданную точку
void centerText(sf::Text& txt, float x, float y);

// панель очков и ресурсов во время игры
struct HUD {
    sf::Font font;
    sf::RectangleShape topBar;
    sf::RectangleShape bottomBar;
    sf::Text scoreText, coinsText, baseHpText;
    sf::RectangleShape pauseButton;
    sf::Sprite pauseIcon;
    sf::Sprite coinSprite;

    // базовая инициализация шрифтов и цветов
    void init(const sf::Font& f);
    // установка текстуры монетки для отображения валюты
    void setCoinTexture(sf::Texture& tex);
    // установка иконки паузы
    void setPauseTexture(sf::Texture& tex);
    // расчёт размеров панелей исходя из вью
    void setLayout(const sf::Vector2f& viewSize);
    // обновление отображаемых чисел
    void update(int score, int coins, float baseHp, float maxHp);
    // отрисовка панелей и текстов
    void draw(sf::RenderTarget& win);
    // внешние рамки нижней панели
    sf::FloatRect bottomBarBounds() const;
    // внешние рамки верхней панели
    sf::FloatRect topBarBounds() const;
    // рамки кнопки паузы
    sf::FloatRect pauseButtonBounds() const;

private:
    // позиционирование текста слева от якоря
    void positionLeftCentered(sf::Text& txt, const sf::Vector2f& anchor);
    // позиционирование текста справа от якоря
    void positionRightCentered(sf::Text& txt, const sf::Vector2f& anchor);
    // подгонка масштаба монетки под высоту текста
    void refreshCoinScale(float targetHeight);
    // обновление позиции и масштаба иконки паузы
    void refreshPauseIcon();
};

// элементы главного меню
struct MenuUI {
    sf::Font font;
    sf::Text title, start, records, quit;
    float buttonWidth{260.f};
    float buttonHeight{50.f};

    // установка текстов и шрифта
    void init(const sf::Font& f);
    // расчёт размеров и позиций кнопок
    void layout(const sf::Vector2f& viewSize);
    // отрисовка меню
    void draw(sf::RenderTarget& w);
    // реакция на клик по кнопке
    std::optional<MenuAction> handleClick(const sf::Vector2f& pos) const;

private:
    // вычисление прямоугольника кнопки по тексту
    sf::FloatRect buttonBounds(const sf::Text& txt) const;
    // отрисовка фона кнопки и текста
    void drawButton(sf::RenderTarget& w, const sf::Text& txt) const;
};

// экран завершения игры
struct GameOverUI {
    sf::Font font;
    sf::Text title;
    sf::Text back;
    sf::RectangleShape backButton;

    // подготовка текста и внешнего вида
    void init(const sf::Font& f);
    // позиционирование элементов по размеру окна
    void layout(const sf::Vector2f& viewSize);
    // проверка клика по кнопке возврата
    bool handleClick(const sf::Vector2f& pos) const;
    // отрисовка заголовка и кнопки
    void draw(sf::RenderTarget& w);
};

// экран таблицы рекордов
struct RecordsUI {
    sf::Font font;
    sf::Text title;
    std::vector<sf::Text> lines;
    sf::RectangleShape backButton;
    sf::Text backLabel;

    // загрузка шрифта и заготовок
    void init(const sf::Font& f);
    // заполнение строк очков
    void setRecords(const std::vector<int>& r);
    // расчёт расположения списка
    void layout(const sf::Vector2f& viewSize);
    // отрисовка заголовка, строк и кнопки
    void draw(sf::RenderTarget& w);
    // обработка клика по кнопке назад
    bool handleClick(const sf::Vector2f& pos) const;
};

// варианты действий при паузе
enum class PauseAction { Resume, Menu };

// оверлей паузы
struct PauseUI {
    sf::Font font;
    sf::Text title;
    sf::RectangleShape resumeButton;
    sf::RectangleShape menuButton;
    sf::Text resumeLabel;
    sf::Text menuLabel;

    // настройка текста и стилей
    void init(const sf::Font& f);
    // выставление позиций кнопок
    void layout(const sf::Vector2f& viewSize);
    // определение выбранного действия
    std::optional<PauseAction> handleClick(const sf::Vector2f& pos) const;
    // отрисовка оверлея
    void draw(sf::RenderTarget& w);

private:
    // унифицированная настройка размеров и положения кнопок
    void configureButton(sf::RectangleShape& button, sf::Text& label, const sf::Vector2f& pos, bool updateReference);
};

// меню выбора типа новой башни
struct TowerSelectionUI {
    sf::Font font;
    sf::RectangleShape panel;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> labels;
    bool visible{false};
    sf::Vector2f position;

    // установка шрифта и базовых стилей
    void init(const sf::Font& f);
    // открытие панели около клетки на карте
    void open(const sf::Vector2f& worldPos, float tileSize);
    // скрытие панели
    void close();
    // обработка клика по кнопке выбора башни
    std::optional<TowerType> handleClick(const sf::Vector2f& worldPos);
    // отрисовка панели и кнопок
    void draw(sf::RenderTarget& w);
};

// меню улучшений башни или базы
struct TowerUpgradeUI {
    sf::Font font;
    sf::RectangleShape panel;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> labels;
    sf::Sprite coinIcon;
    float iconTargetHeight{16.f};
    std::vector<bool> showCoin;
    bool visible{false};

    // подготовка панели
    void init(const sf::Font& f);
    // построение кнопок с переданными опциями
    void open(const sf::Vector2f& worldPos, float tileSize, const std::vector<std::string>& options);
    // скрытие меню
    void close();
    // обработка клика по кнопке улучшения
    std::optional<int> handleClick(const sf::Vector2f& worldPos);
    // получить рамки панели
    sf::FloatRect panelBounds() const;
    // отрисовка панели, подписей и иконок монет
    void draw(sf::RenderTarget& w);
    // установка текстуры монетки и целевой высоты
    void setCoinTexture(sf::Texture& tex, float targetHeight);

private:
    // отрисовка иконки монетки после текста
    void drawCoinIcon(sf::RenderTarget& w, const sf::Text& label);
    // пересчёт масштаба иконки
    void refreshIconScale();
    // позиционирование текста по центру кнопки
    void positionLabel(sf::Text& label, const sf::RectangleShape& button);
};
