#include "UI.hpp"
#include <algorithm>
#include <cmath>


void snapTextPosition(sf::Text& txt) {
    // защёлкиваем координаты к целым пикселям чтобы не мылить шрифт
    sf::Vector2f pos = txt.getPosition();
    txt.setPosition(std::round(pos.x), std::round(pos.y));
}


void centerText(sf::Text& txt, float x, float y) {
    // меняем origin чтобы центр совпал с переданной точкой
    sf::FloatRect bounds = txt.getLocalBounds();
    txt.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    txt.setPosition(x, y);
    snapTextPosition(txt);
}


void HUD::init(const sf::Font& f) {
    // копируем шрифт и задаём размеры текста
    font = f;
    scoreText.setFont(font);
    coinsText.setFont(font);
    baseHpText.setFont(font);
    scoreText.setCharacterSize(24);
    coinsText.setCharacterSize(24);
    baseHpText.setCharacterSize(22);

    topBar.setFillColor(sf::Color(0, 0, 0, 0));
    bottomBar.setFillColor(sf::Color(0, 0, 0, 0));
    topBar.setOutlineThickness(0.f);
    bottomBar.setOutlineThickness(0.f);
    pauseButton.setFillColor(sf::Color(40, 60, 110, 200));
    pauseButton.setOutlineThickness(2.f);
    pauseButton.setOutlineColor(sf::Color(200, 220, 255));
}


void HUD::setCoinTexture(sf::Texture& tex) {
    // сохраняем текстуру монетки для отображения валюты
    coinSprite.setTexture(tex);
    coinSprite.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
}


void HUD::setPauseTexture(sf::Texture& tex) {
    // спрайт иконки паузы и её масштаб
    pauseIcon.setTexture(tex);
    pauseIcon.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
    refreshPauseIcon();
}


void HUD::setLayout(const sf::Vector2f& viewSize) {
    // рассчитываем полосы интерфейса и кнопку паузы
    const float topHeight = 70.f;
    const float bottomHeight = 90.f;
    topBar.setSize({viewSize.x, topHeight});
    topBar.setPosition(0.f, 0.f);
    bottomBar.setSize({viewSize.x, bottomHeight});
    bottomBar.setPosition(0.f, viewSize.y - bottomHeight);
    sf::Vector2f buttonSize(52.f, 52.f);
    pauseButton.setSize(buttonSize);
    pauseButton.setOrigin(buttonSize.x, buttonSize.y);
    pauseButton.setPosition(viewSize.x - 12.f, viewSize.y - 12.f);
    refreshPauseIcon();
}


void HUD::update(int score, int coins, float baseHp, float maxHp) {
    // обновляем строки текста и их позицию
    scoreText.setString("Score: " + std::to_string(score));
    coinsText.setString(std::to_string(coins));
    baseHpText.setString("Base HP: " + std::to_string(static_cast<int>(baseHp)) + "/" + std::to_string(static_cast<int>(maxHp)));

    positionLeftCentered(scoreText, topBar.getPosition() + sf::Vector2f(30.f, topBar.getSize().y * 0.5f));
    positionRightCentered(coinsText, topBar.getPosition() + sf::Vector2f(topBar.getSize().x - 30.f, topBar.getSize().y * 0.5f));
    positionLeftCentered(baseHpText, bottomBar.getPosition() + sf::Vector2f(30.f, bottomBar.getSize().y * 0.5f));
    refreshPauseIcon();
    if (coinSprite.getTexture()) {
        sf::FloatRect textBounds = coinsText.getGlobalBounds();
        float desiredHeight = std::max(12.f, coinsText.getLocalBounds().height + 8.f);
        refreshCoinScale(desiredHeight);
        float iconWidth = coinSprite.getGlobalBounds().width;
        float centerX = textBounds.left + textBounds.width + 4.f + iconWidth * 0.5f;
        float centerY = textBounds.top + textBounds.height * 0.5f;
        coinSprite.setPosition(centerX, centerY);
    }
}


void HUD::draw(sf::RenderTarget& win) {
    // выводим панели, текст и кнопки
    win.draw(topBar);
    win.draw(bottomBar);
    win.draw(scoreText);
    if (coinSprite.getTexture()) win.draw(coinSprite);
    win.draw(coinsText);
    win.draw(baseHpText);
    win.draw(pauseButton);
    if (pauseIcon.getTexture()) win.draw(pauseIcon);
}

sf::FloatRect HUD::bottomBarBounds() const { return bottomBar.getGlobalBounds(); }
sf::FloatRect HUD::topBarBounds() const { return topBar.getGlobalBounds(); }
sf::FloatRect HUD::pauseButtonBounds() const { return pauseButton.getGlobalBounds(); }

void HUD::positionLeftCentered(sf::Text& txt, const sf::Vector2f& anchor) {
    // якорим текст слева и центрируем по вертикали
    auto bounds = txt.getLocalBounds();
    txt.setOrigin(bounds.left, bounds.top + bounds.height * 0.5f);
    txt.setPosition(anchor);
    snapTextPosition(txt);
}

void HUD::positionRightCentered(sf::Text& txt, const sf::Vector2f& anchor) {
    // якорим текст справа и центрируем по вертикали
    auto bounds = txt.getLocalBounds();
    txt.setOrigin(bounds.left + bounds.width, bounds.top + bounds.height * 0.5f);
    txt.setPosition(anchor);
    snapTextPosition(txt);
}

void HUD::refreshCoinScale(float targetHeight) {
    // масштабируем спрайт монетки под высоту текста
    if (!coinSprite.getTexture()) return;
    auto texSize = coinSprite.getTexture()->getSize();
    if (texSize.y == 0) return;
    float scale = targetHeight / static_cast<float>(texSize.y);
    coinSprite.setScale(scale, scale);
}

void HUD::refreshPauseIcon() {
    // обновляем иконку паузы при изменении размеров
    if (!pauseIcon.getTexture()) return;
    sf::Vector2f size = pauseButton.getSize();
    auto texSize = pauseIcon.getTexture()->getSize();
    if (texSize.x == 0 || texSize.y == 0) return;
    float target = std::max(10.f, std::min(size.x, size.y) - 12.f);
    float scale = target / static_cast<float>(std::max(texSize.x, texSize.y));
    pauseIcon.setScale(scale, scale);
    sf::Vector2f pos = pauseButton.getPosition();
    pauseIcon.setPosition(pos.x - size.x * 0.5f,
                          pos.y - size.y * 0.5f);
}


void MenuUI::init(const sf::Font& f) {
    // базовые подписи и размеры главного меню
    font = f;
    title.setFont(font); start.setFont(font); records.setFont(font); quit.setFont(font);
    title.setString("Tower Defense"); title.setCharacterSize(56);
    start.setString("Start"); start.setCharacterSize(32);
    records.setString("Records"); records.setCharacterSize(32);
    quit.setString("Quit"); quit.setCharacterSize(32);
}


void MenuUI::layout(const sf::Vector2f& viewSize) {
    // рассчитываем позицию кнопок в центре экрана
    float centerX = viewSize.x * 0.5f;
    float centerY = viewSize.y * 0.5f;
    centerText(title, centerX, centerY - 160.f);
    float spacing = buttonHeight + 20.f;
    centerText(start, centerX, centerY - spacing);
    centerText(records, centerX, centerY);
    centerText(quit, centerX, centerY + spacing);

    auto paddedWidth = [](const sf::Text& txt){ return txt.getLocalBounds().width + 80.f; };
    auto paddedHeight = [](const sf::Text& txt){ return txt.getLocalBounds().height + 30.f; };
    buttonWidth = std::max({paddedWidth(start), paddedWidth(records), paddedWidth(quit)});
    buttonHeight = std::max({paddedHeight(start), paddedHeight(records), paddedHeight(quit)});

    auto adjustOrigin = [](sf::Text& txt){
        sf::FloatRect bounds = txt.getLocalBounds();
        txt.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    };
    adjustOrigin(start);
    adjustOrigin(records);
    adjustOrigin(quit);
}


void MenuUI::draw(sf::RenderTarget& w) {
    // рисуем фон кнопок и их текст
    w.draw(title);
    drawButton(w, start);
    drawButton(w, records);
    drawButton(w, quit);
    w.draw(start);
    w.draw(records);
    w.draw(quit);
}


std::optional<MenuAction> MenuUI::handleClick(const sf::Vector2f& pos) const {
    // определяем какая кнопка нажата
    if (buttonBounds(start).contains(pos)) return MenuAction::Start;
    if (buttonBounds(records).contains(pos)) return MenuAction::Records;
    if (buttonBounds(quit).contains(pos)) return MenuAction::Quit;
    return std::nullopt;
}

sf::FloatRect MenuUI::buttonBounds(const sf::Text& txt) const {
    // размеры кнопки с отступами вокруг текста
    sf::FloatRect textBounds = txt.getGlobalBounds();
    float left = textBounds.left - (buttonWidth - textBounds.width) * 0.5f;
    float top = textBounds.top - (buttonHeight - textBounds.height) * 0.5f;
    return {left, top, buttonWidth, buttonHeight};
}


void MenuUI::drawButton(sf::RenderTarget& w, const sf::Text& txt) const {
    // рисуем прямоугольник кнопки с рамкой
    sf::FloatRect r = buttonBounds(txt);
    sf::RectangleShape box;
    box.setPosition(r.left, r.top);
    box.setSize({r.width, r.height});
    box.setFillColor(sf::Color(40, 60, 110, 200));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(200, 220, 255));
    w.draw(box);
}


void GameOverUI::init(const sf::Font& f) {
    // заголовок и кнопка возврата в меню
    font = f; title.setFont(font); back.setFont(font);
    title.setString("Game Over"); title.setCharacterSize(56);
    back.setString("Main Menu"); back.setCharacterSize(32);
    backButton.setFillColor(sf::Color(40, 60, 110, 200));
    backButton.setOutlineThickness(2.f);
    backButton.setOutlineColor(sf::Color(200, 220, 255));
}


void GameOverUI::layout(const sf::Vector2f& viewSize) {
    // центрирование надписи и кнопки
    float centerX = viewSize.x * 0.5f;
    float centerY = viewSize.y * 0.5f;
    centerText(title, centerX, centerY - 60.f);
    sf::FloatRect labelBounds = back.getLocalBounds();
    back.setOrigin(labelBounds.left + labelBounds.width * 0.5f,
                   labelBounds.top + labelBounds.height * 0.5f);
    sf::Vector2f buttonPos(centerX, centerY + 40.f);
    back.setPosition(buttonPos);
    sf::Vector2f buttonSize(labelBounds.width + 80.f, labelBounds.height + 30.f);
    backButton.setSize(buttonSize);
    backButton.setOrigin(buttonSize.x * 0.5f, buttonSize.y * 0.5f);
    backButton.setPosition(buttonPos);
    snapTextPosition(back);
}


bool GameOverUI::handleClick(const sf::Vector2f& pos) const {
    // проверяем, попал ли клик по кнопке
    return backButton.getGlobalBounds().contains(pos);
}


void GameOverUI::draw(sf::RenderTarget& w) {
    // отрисовка заголовка и кнопки
    w.draw(title);
    w.draw(backButton);
    w.draw(back);
}


void RecordsUI::init(const sf::Font& f) {
    // заготовка заголовка и кнопки назад
    font=f; title.setFont(font); title.setString("Records"); title.setCharacterSize(44);
    backLabel.setFont(font);
    backLabel.setString("Back");
    backLabel.setCharacterSize(32);
    backButton.setFillColor(sf::Color(40, 60, 110, 200));
    backButton.setOutlineThickness(2.f);
    backButton.setOutlineColor(sf::Color(200, 220, 255));
}


void RecordsUI::setRecords(const std::vector<int>& r) {
    // генерируем пять строк таблицы
    lines.clear();
    std::vector<int> display = r;
    if (display.size() < 5) display.resize(5, 0);
    for(size_t i=0;i<display.size();++i){
        sf::Text t; t.setFont(font);
        t.setString(std::to_string(i+1)+". "+std::to_string(display[i]));
        t.setCharacterSize(32);
        lines.push_back(t);
    }
}


void RecordsUI::layout(const sf::Vector2f& viewSize) {
    // центрируем список и кнопку в окне
    float centerX = viewSize.x * 0.5f;
    centerText(title, centerX, viewSize.y * 0.3f);
    float startY = viewSize.y * 0.4f;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        centerText(lines[i], centerX, startY + static_cast<float>(i) * 40.f);
    }
    sf::FloatRect labelBounds = backLabel.getLocalBounds();
    backLabel.setOrigin(labelBounds.left + labelBounds.width * 0.5f,
                        labelBounds.top + labelBounds.height * 0.5f);
    sf::Vector2f buttonPos(centerX, viewSize.y * 0.8f);
    backLabel.setPosition(buttonPos);
    sf::Vector2f buttonSize(labelBounds.width + 80.f, labelBounds.height + 30.f);
    backButton.setSize(buttonSize);
    backButton.setOrigin(buttonSize.x * 0.5f, buttonSize.y * 0.5f);
    backButton.setPosition(buttonPos);
    snapTextPosition(backLabel);
}


void RecordsUI::draw(sf::RenderTarget& w) {
    // вывод заголовка, строк и кнопки
    w.draw(title);
    for(auto& l:lines) w.draw(l);
    w.draw(backButton);
    w.draw(backLabel);
}


bool RecordsUI::handleClick(const sf::Vector2f& pos) const {
    // клик по кнопке назад
    return backButton.getGlobalBounds().contains(pos);
}


void PauseUI::init(const sf::Font& f) {
    // заголовок паузы и две кнопки
    font = f;
    title.setFont(font);
    title.setString("Paused");
    title.setCharacterSize(56);
    resumeLabel.setFont(font);
    resumeLabel.setString("Resume");
    resumeLabel.setCharacterSize(32);
    menuLabel.setFont(font);
    menuLabel.setString("Main Menu");
    menuLabel.setCharacterSize(32);
    auto styleButton = [](sf::RectangleShape& btn) {
        btn.setFillColor(sf::Color(40, 60, 110, 220));
        btn.setOutlineThickness(2.f);
        btn.setOutlineColor(sf::Color(200, 220, 255));
    };
    styleButton(resumeButton);
    styleButton(menuButton);
}


void PauseUI::layout(const sf::Vector2f& viewSize) {
    // размещаем элементы в центре экрана
    float centerX = viewSize.x * 0.5f;
    float centerY = viewSize.y * 0.5f;
    centerText(title, centerX, centerY - 100.f);
    configureButton(resumeButton, resumeLabel, {centerX, centerY}, true);
    configureButton(menuButton, menuLabel, {centerX, centerY + 80.f}, false);
}


std::optional<PauseAction> PauseUI::handleClick(const sf::Vector2f& pos) const {
    // возвращаем действие если клик попал по кнопке
    if (resumeButton.getGlobalBounds().contains(pos)) return PauseAction::Resume;
    if (menuButton.getGlobalBounds().contains(pos)) return PauseAction::Menu;
    return std::nullopt;
}


void PauseUI::draw(sf::RenderTarget& w) {
    // рисуем заголовок и обе кнопки
    w.draw(title);
    w.draw(resumeButton);
    w.draw(resumeLabel);
    w.draw(menuButton);
    w.draw(menuLabel);
}

void PauseUI::configureButton(sf::RectangleShape& button, sf::Text& label, const sf::Vector2f& pos, bool updateReference) {
    // единый расчёт размеров и положения кнопок паузы
    static sf::Vector2f referenceSize(0.f, 0.f);
    sf::FloatRect bounds = label.getLocalBounds();
    sf::Vector2f size(bounds.width + 120.f, bounds.height + 40.f);
    if (updateReference || (referenceSize.x == 0.f && referenceSize.y == 0.f)) {
        referenceSize = size;
    }
    if (!updateReference) size = referenceSize;
    button.setSize(size);
    button.setOrigin(size.x * 0.5f, size.y * 0.5f);
    button.setPosition(pos);
    label.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    label.setPosition(pos);
    snapTextPosition(label);
}


void TowerSelectionUI::init(const sf::Font& f) {
    // три кнопки выбора типа башни
    font = f;
    buttons.assign(3, sf::RectangleShape(sf::Vector2f(0.f, 0.f)));
    labels.assign(3, sf::Text());
    panel.setSize({220.f, 120.f});
    panel.setFillColor(sf::Color(20, 20, 30, 220));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(200, 200, 220));

    const char* names[3] = {"Normal", "Cryo", "Poison"};
    sf::Color colors[3] = {
        sf::Color(220, 70, 70),
        sf::Color(80, 160, 255),
        sf::Color(90, 200, 90)
    };

    for (std::size_t i = 0; i < buttons.size(); ++i) {
        buttons[i].setSize({60.f, 60.f});
        buttons[i].setFillColor(colors[i]);
        buttons[i].setOutlineThickness(2.f);
        buttons[i].setOutlineColor(sf::Color::White);

        labels[i].setFont(font);
        labels[i].setCharacterSize(18);
        labels[i].setFillColor(sf::Color::White);
        labels[i].setString(names[i]);
    }
}


void TowerSelectionUI::open(const sf::Vector2f& worldPos, float tileSize) {
    // масштабируем панель и кнопки под размер клетки
    position = worldPos;
    panel.setSize({tileSize * buttons.size() + 20.f * (buttons.size() + 1), tileSize + 40.f});
    panel.setPosition(position);
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        sf::Vector2f offset(10.f + i * (tileSize + 20.f), 40.f);
        buttons[i].setSize({tileSize, tileSize});
        buttons[i].setPosition(position + offset);
        labels[i].setPosition(position.x + offset.x, position.y + 10.f);
        snapTextPosition(labels[i]);
    }
    visible = true;
}


void TowerSelectionUI::close() { visible = false; }


std::optional<TowerType> TowerSelectionUI::handleClick(const sf::Vector2f& worldPos) {
    // проверяем попадание клика по кнопке, иначе скрываем
    if (!visible) return std::nullopt;
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i].getGlobalBounds().contains(worldPos)) {
            visible = false;
            return static_cast<TowerType>(i);
        }
    }
    if (!panel.getGlobalBounds().contains(worldPos)) {
        visible = false;
    }
    return std::nullopt;
}


void TowerSelectionUI::draw(sf::RenderTarget& w) {
    // вывод панели и подписей типов башен
    if (!visible) return;
    w.draw(panel);
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        w.draw(buttons[i]);
        w.draw(labels[i]);
    }
}


void TowerUpgradeUI::init(const sf::Font& f) {
    // начальные размеры и стили панели улучшений
    font = f;
    panel.setSize({240.f, 120.f});
    panel.setFillColor(sf::Color(25, 25, 35, 230));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(200, 200, 220));

    buttons.resize(0);
    labels.resize(0);
}


void TowerUpgradeUI::open(const sf::Vector2f& worldPos, float tileSize, const std::vector<std::string>& options) {
    // создаём кнопки по переданным опциям и подгоняем размер панели
    buttons.resize(options.size());
    labels.resize(options.size());
    showCoin.assign(options.size(), false);

    float maxLabelWidth = 0.f;
    for (const auto& text : options) {
        sf::Text temp(text, font, 16);
        auto bounds = temp.getLocalBounds();
        maxLabelWidth = std::max(maxLabelWidth, bounds.width);
    }

    float buttonWidth = std::max(tileSize * 2.5f, maxLabelWidth + 60.f);
    float panelHeight = tileSize * 0.6f * static_cast<float>(options.size()) + 40.f + 20.f * (options.size() - 1);
    panel.setSize({buttonWidth + 40.f, panelHeight});
    sf::Vector2f pos = worldPos;
    panel.setPosition(pos);
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        buttons[i].setSize({buttonWidth, tileSize * 0.6f});
        buttons[i].setFillColor(sf::Color(60, 60, 90, 220));
        buttons[i].setOutlineThickness(2.f);
        buttons[i].setOutlineColor(sf::Color::White);
        sf::Vector2f offset(20.f, 20.f + static_cast<float>(i) * (tileSize * 0.6f + 20.f));
        buttons[i].setPosition(pos + offset);
        labels[i].setFont(font);
        labels[i].setCharacterSize(20);
        labels[i].setFillColor(sf::Color::White);
        std::string optionText = options[i];
        const std::string token = "<coin>";
        bool hasIcon = false;
        size_t tokenPos;
        while ((tokenPos = optionText.find(token)) != std::string::npos) {
            optionText.erase(tokenPos, token.size());
            hasIcon = true;
        }
        labels[i].setString(optionText);
        showCoin[i] = hasIcon;
        positionLabel(labels[i], buttons[i]);
    }
    visible = true;
}


void TowerUpgradeUI::close() { visible = false; }


std::optional<int> TowerUpgradeUI::handleClick(const sf::Vector2f& worldPos) {
    // ищем индекс кнопки по клику
    if (!visible) return std::nullopt;
    for (int i = 0; i < static_cast<int>(buttons.size()); ++i) {
        if (buttons[i].getGlobalBounds().contains(worldPos)) {
            return i;
        }
    }
    return std::nullopt;
}

sf::FloatRect TowerUpgradeUI::panelBounds() const { return panel.getGlobalBounds(); }


void TowerUpgradeUI::draw(sf::RenderTarget& w) {
    if (!visible) return;
    w.draw(panel);
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        w.draw(buttons[i]);
        w.draw(labels[i]);
        if (coinIcon.getTexture() && showCoin[i]) {
            drawCoinIcon(w, labels[i]);
        }
    }
}


void TowerUpgradeUI::setCoinTexture(sf::Texture& tex, float targetHeight) {
    // сохраняем спрайт монетки и нужную высоту для масштабирования
    coinIcon.setTexture(tex);
    coinIcon.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
    iconTargetHeight = targetHeight;
    refreshIconScale();
}


void TowerUpgradeUI::drawCoinIcon(sf::RenderTarget& w, const sf::Text& label) {
    // рисуем монетку после текста опции
    sf::FloatRect bounds = label.getGlobalBounds();
    sf::Sprite icon = coinIcon;
    icon.setPosition(bounds.left + bounds.width + 6.f, bounds.top + bounds.height * 0.5f);
    w.draw(icon);
}

void TowerUpgradeUI::refreshIconScale() {
    // масштабируем монету под нужную высоту
    if (!coinIcon.getTexture()) return;
    auto sz = coinIcon.getTexture()->getSize();
    if (sz.y == 0) return;
    float scale = iconTargetHeight / static_cast<float>(sz.y);
    coinIcon.setScale(scale, scale);
}

void TowerUpgradeUI::positionLabel(sf::Text& label, const sf::RectangleShape& button) {
    // выравниваем текст по центру кнопки
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    label.setPosition(button.getPosition().x + button.getSize().x * 0.5f,
                      button.getPosition().y + button.getSize().y * 0.5f);
    snapTextPosition(label);
}
