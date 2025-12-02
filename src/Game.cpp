#include "Game.hpp"


Game::Game() {
    window.setFramerateLimit(60);
    font.loadFromFile("assets/ByteBounce.ttf");
    hud.init(font); menu.init(font); over.init(font); recUI.init(font); towerMenu.init(font); upgradeMenu.init(font); pauseUI.init(font);
    worldView.setSize(1280.f, 720.f);
    worldView.setCenter(worldView.getSize().x * 0.5f, worldView.getSize().y * 0.5f);
    uiView = worldView;
    applyLetterboxView(window.getSize().x, window.getSize().y);
    frameBufferReady = frameBuffer.create(static_cast<unsigned>(worldView.getSize().x),
                                          static_cast<unsigned>(worldView.getSize().y));
    if (frameBufferReady) {
        frameBuffer.setSmooth(false);
    }

    records.load();
    recUI.setRecords(records.last);

    sf::Vector2f viewSize = worldView.getSize();
    menu.layout(viewSize);
    over.layout(viewSize);
    recUI.layout(viewSize);
    pauseUI.layout(viewSize);
    hud.setLayout(viewSize);
    hud.update(score, coins, base.hp, base.maxHp);
    auto& coinTex = res.getTexture("assets/coin.png");
    hud.setCoinTexture(coinTex);
    upgradeMenu.setCoinTexture(coinTex, 18.f);
    auto& pauseTex = res.getTexture("assets/pause.png");
    hud.setPauseTexture(pauseTex);

    float gridWidth = level.tileSize * Level::GridSize;
    float gridHeight = level.tileSize * Level::GridSize;
    level.origin = { (viewSize.x - gridWidth) * 0.5f, (viewSize.y - gridHeight) * 0.5f };

    bool mapLoaded = level.loadFromFile("data/map.txt");
    if (!mapLoaded) {
        mapLoaded = level.loadFromFile("../data/map.txt");
    }

    if (!mapLoaded) {
        float centerY = viewSize.y * 0.5f;
        float roadLength = 600.f;
        float startX = (viewSize.x - roadLength) * 0.5f;
        for (int i=0;i<5;++i) level.pads.push_back({sf::Vector2f(startX + 180.f + i*90.f, centerY - 120.f)});
        for (int i=0;i<5;++i) level.pads.push_back({sf::Vector2f(startX + 180.f + i*90.f, centerY + 120.f)});
        for (int i=0;i<=10;++i) level.path.waypoints.push_back({startX + i*(roadLength/10.f), centerY});
        level.spawnPos = level.path.waypoints.front();
        level.basePos = {startX + roadLength, centerY};
    } else {
        base.setSize(level.tileSize - 6.f);
    }

    base.pos = level.basePos;
}


void Game::resetGame(){
    enemies.clear(); towers.clear(); projectiles.clear();
    selectedTower = nullptr;
    placementPadIndex = -1;
    towerMenu.close();
    upgradeMenu.close();
    upgradeTarget = nullptr;
    score=0; coins=90; spawnTimer=0; spawnEvery=2.0f; waveCount=0;
    base.hp = base.maxHp = 1000.f;
    state = GameState::Playing;
    hud.update(score, coins, base.hp, base.maxHp);
}


void Game::run(){
    while (window.isOpen()) {
        handleEvents();
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.1f);
        update(dt);
        draw();
    }
}


void Game::handleEvents(){
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.close();
        if (e.type == sf::Event::Resized) {
            applyLetterboxView(e.size.width, e.size.height);
        }
        if (state==GameState::Menu) {
            if (e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left) {
                sf::Vector2f mpos = window.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y}, window.getDefaultView());
                auto action = menu.handleClick(mpos);
                if (action) {
                    if (*action == MenuAction::Start) {
                        resetGame();
                    } else if (*action == MenuAction::Records) {
                        state = GameState::Records;
                        recUI.setRecords(records.last);
                        recUI.layout(worldView.getSize());
                    } else if (*action == MenuAction::Quit) {
                        window.close();
                    }
                }
            }
            if (e.type==sf::Event::KeyPressed && e.key.code==sf::Keyboard::Enter) resetGame();
            if (e.type==sf::Event::KeyPressed && e.key.code==sf::Keyboard::R) { state=GameState::Records; recUI.setRecords(records.last); recUI.layout(worldView.getSize()); }
            if (e.type==sf::Event::KeyPressed && e.key.code==sf::Keyboard::Escape) window.close();
        } else if (state==GameState::Records) {
            if (e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left) {
                sf::Vector2f mpos = window.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y}, worldView);
                if (recUI.handleClick(mpos)) {
                    state = GameState::Menu;
                }
            }
            if (e.type==sf::Event::KeyPressed) state=GameState::Menu;
        } else if (state==GameState::Paused) {
            if (e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left) {
                sf::Vector2f mpos = window.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y}, worldView);
                auto choice = pauseUI.handleClick(mpos);
                if (choice) {
                    if (*choice == PauseAction::Resume) resumeFromPause();
                    else if (*choice == PauseAction::Menu) exitToMenuFromPause();
                }
            }
            if (e.type==sf::Event::KeyPressed && e.key.code==sf::Keyboard::Escape) resumeFromPause();
        } else if (state==GameState::GameOver) {
            if (e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left) {
                sf::Vector2f mpos = window.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y}, worldView);
                if (over.handleClick(mpos)) {
                    state = GameState::Menu;
                }
            }
        } else if (state==GameState::Playing) {
            if (e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left) {
                sf::Vector2f mpos = window.mapPixelToCoords({e.mouseButton.x, e.mouseButton.y}, worldView);
                handleLeftClick(mpos);
            }
            if (e.type==sf::Event::KeyPressed) {
                if (e.key.code==sf::Keyboard::Num3) tryUpgradeBaseHp();
                if (e.key.code==sf::Keyboard::Escape) {
                    enterPause();
                }
            }
        }
    }
}


void Game::handleLeftClick(const sf::Vector2f& worldPos) {
    if (hud.pauseButtonBounds().contains(worldPos)) {
        enterPause();
        return;
    }
    if (upgradeMenu.visible) {
        sf::Vector2f uiClick = worldToUI(worldPos);
        auto choice = upgradeMenu.handleClick(uiClick);
        if (choice) {
            if (upgradeTarget) applyUpgradeChoice(static_cast<UpgradeOption>(*choice));
            else handleBaseMenuSelection(*choice);
            return;
        }
        if (upgradeMenu.panelBounds().contains(uiClick)) {
            return;
        }
        upgradeMenu.close();
        upgradeTarget = nullptr;
        return;
    }

    if (towerMenu.visible) {
        sf::Vector2f uiClick = worldToUI(worldPos);
        auto choice = towerMenu.handleClick(uiClick);
        if (choice) {
            placeTowerChoice(*choice);
            return;
        }
        if (towerMenu.visible && towerMenu.panel.getGlobalBounds().contains(uiClick)) {
            return;
        }
        towerMenu.close();
        placementPadIndex = -1;
        return;
    }

    if (base.bounds().contains(worldPos)) {
        towerMenu.close();
        openBaseMenu();
        return;
    }

    if (hud.topBarBounds().contains(worldPos) || hud.bottomBarBounds().contains(worldPos)) {
        towerMenu.close();
        upgradeMenu.close();
        upgradeTarget = nullptr;
        selectedTower = nullptr;
        return;
    }

    Tower* towerUnderCursor = getTowerAt(worldPos);
    if (towerUnderCursor) {
        selectedTower = towerUnderCursor;
        openUpgradeMenu(*towerUnderCursor);
        return;
    }

    upgradeMenu.close();
    upgradeTarget = nullptr;
    selectedTower = nullptr;
    openPlacementMenu(worldPos);
}


void Game::openPlacementMenu(const sf::Vector2f& worldPos) {
    if (coins < 30) return;
    int idx = level.nearestPadIndex(worldPos);
    if (idx < 0) return;
    const sf::Vector2f& padPos = level.pads[idx].pos;
    float placementRadius = level.hasGrid ? level.tileSize * 0.6f : 20.f;
    if (std::hypot(padPos.x - worldPos.x, padPos.y - worldPos.y) >= placementRadius) return;
    for (auto& tower : towers) {
        if (std::hypot(tower->pos.x - padPos.x, tower->pos.y - padPos.y) < 1.f) return;
    }
    upgradeMenu.close();
    upgradeTarget = nullptr;
    placementMenuPos = padPos;
    placementPadIndex = idx;
    sf::Vector2f panelWorldPos = padPos - sf::Vector2f(level.tileSize, level.tileSize);
    sf::Vector2f uiPos = worldToUI(panelWorldPos);
    towerMenu.open(uiPos, level.tileSize);
}


void Game::openUpgradeMenu(Tower& tower) {
    towerMenu.close();
    upgradeTarget = &tower;
    auto labels = buildUpgradeLabels(tower);
    float panelWidth = std::max(240.f, level.tileSize * 2.5f);
    float panelHeight = level.tileSize * 0.6f * static_cast<float>(labels.size()) + 40.f + 20.f * (labels.size() - 1);
    sf::Vector2f viewTopLeft = worldView.getCenter() - worldView.getSize() * 0.5f;
    sf::Vector2f viewBottomRight = viewTopLeft + worldView.getSize();
    sf::Vector2f desiredPos = tower.pos + sf::Vector2f(level.tileSize * 0.4f, -panelHeight - 10.f);
    desiredPos.x = std::clamp(desiredPos.x, viewTopLeft.x + 10.f, viewBottomRight.x - panelWidth - 10.f);
    desiredPos.y = std::clamp(desiredPos.y, viewTopLeft.y + 10.f, viewBottomRight.y - panelHeight - 10.f);
    desiredPos.x = std::round(desiredPos.x);
    desiredPos.y = std::round(desiredPos.y);
    sf::Vector2f uiPos = worldToUI(desiredPos);
    upgradeMenu.open(uiPos, level.tileSize, labels);
}


void Game::openBaseMenu() {
    towerMenu.close();
    upgradeTarget = nullptr;
    std::vector<std::string> opts;
    if (coins >= 30) {
        opts.push_back("HP -30 <coin>");
    } else {
        opts.push_back("HP - 30 <coin>");
    }
    sf::Vector2f viewTopLeft = worldView.getCenter() - worldView.getSize() * 0.5f;
    sf::Vector2f viewBottomRight = viewTopLeft + worldView.getSize();
    float panelHeight = level.tileSize * 0.6f + 40.f;
    float panelWidth = std::max(260.f, level.tileSize * 2.5f);
    sf::Vector2f desiredPos = base.pos + sf::Vector2f(level.tileSize * 0.4f, -panelHeight - 10.f);
    desiredPos.x = std::clamp(desiredPos.x, viewTopLeft.x + 10.f, viewBottomRight.x - panelWidth - 10.f);
    desiredPos.y = std::clamp(desiredPos.y, viewTopLeft.y + 10.f, viewBottomRight.y - panelHeight - 10.f);
    desiredPos.x = std::round(desiredPos.x);
    desiredPos.y = std::round(desiredPos.y);
    sf::Vector2f uiPos = worldToUI(desiredPos);
    upgradeMenu.open(uiPos, level.tileSize, opts);
    upgradeTarget = nullptr;
}


std::vector<std::string> Game::buildUpgradeLabels(const Tower& tower) const {
    std::vector<std::string> labels;
    std::string rateLabel = "Fire Rate -20 <coin>";
    std::string special;
    switch (tower.type) {
        case TowerType::Normal:
            special = "Damage -20 <coin>";
            break;
        case TowerType::Cryo:
            special = "Freeze -20 <coin>";
            break;
        case TowerType::Poison:
            special = "Poison -20 <coin>";
            break;
    }
    // Limit is 12 total levels including base (level 1 + 11 upgrades)
    if (tower.totalUpgradeLevels >= 11) {
        rateLabel += " [Limit]";
        special += " [Limit]";
    }
    labels.push_back(rateLabel);
    labels.push_back(special);

    int refund = std::max(0, tower.totalInvested / 2);
    labels.push_back("Sell +" + std::to_string(refund) + " <coin>");

    return labels;
}


void Game::applyUpgradeChoice(UpgradeOption option) {
    if (!upgradeTarget) return;
    Tower& tower = *upgradeTarget;
    bool applied = false;

    if (option == UpgradeOption::Sell) {
        int refund = std::max(0, tower.totalInvested / 2);
        coins += refund;
        towers.erase(std::remove_if(towers.begin(), towers.end(), [&](const std::unique_ptr<Tower>& ptr){ return ptr.get() == &tower; }), towers.end());
        upgradeMenu.close();
        upgradeTarget = nullptr;
        selectedTower = nullptr;
        hud.update(score, coins, base.hp, base.maxHp);
        return;
    }

    // Total levels: base level 1 + upgrades; stop when displaying Lv12
    if (tower.totalUpgradeLevels >= 11 || coins < towerUpgradeCost) {
        openUpgradeMenu(tower);
        return;
    }

    if (option == UpgradeOption::Rate) {
        tower.fireCooldown = std::max(0.02f, tower.fireCooldown - 0.05f);
        tower.rateUpgradeLevel++;
        applied = true;
    } else {
        switch (tower.type) {
            case TowerType::Normal:
                tower.damage += 2.f;
                applied = true;
                break;
            case TowerType::Cryo:
                tower.freezeDuration += 0.4f;
                applied = true;
                break;
            case TowerType::Poison:
                tower.poisonTotalDamage += 3.f;
                applied = true;
                break;
        }
        if (applied) tower.specialUpgradeLevel++;
    }

    if (applied) {
        tower.level++;
        tower.totalUpgradeLevels++;
        coins -= towerUpgradeCost;
        tower.totalInvested += towerUpgradeCost;
        hud.update(score, coins, base.hp, base.maxHp);
        openUpgradeMenu(tower);
        return;
    }
}


void Game::handleBaseMenuSelection(int index) {
    if (index == 0) {
        tryUpgradeBaseHp();
    }
    openBaseMenu();
}


void Game::applyLetterboxView(unsigned width, unsigned height) {
    if (height == 0 || width == 0) return;
    const float baseWidth = 1280.f;
    const float baseHeight = 720.f;
    float windowRatio = static_cast<float>(width) / static_cast<float>(height);
    float baseRatio = baseWidth / baseHeight;
    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);

    if (windowRatio > baseRatio) {
        float scale = baseRatio / windowRatio;
        viewport.width = scale;
        viewport.left = (1.f - scale) * 0.5f;
    } else {
        float scale = windowRatio / baseRatio;
        viewport.height = scale;
        viewport.top = (1.f - scale) * 0.5f;
    }

    auto snapViewport = [&](float& offset, float& size, unsigned total) {
        float pixelStart = std::round(offset * total);
        float pixelSize = std::round(size * total);
        offset = pixelStart / static_cast<float>(total);
        size = pixelSize / static_cast<float>(total);
    };
    snapViewport(viewport.left, viewport.width, width);
    snapViewport(viewport.top, viewport.height, height);

    worldView.setSize(baseWidth, baseHeight);
    worldView.setCenter(baseWidth * 0.5f, baseHeight * 0.5f);
    worldView.setViewport(viewport);
    window.setView(worldView);

    uiView.setSize(baseWidth, baseHeight);
    uiView.setCenter(baseWidth * 0.5f, baseHeight * 0.5f);
    uiView.setViewport(viewport);

    offscreenWorldView = worldView;
    offscreenWorldView.setViewport({0.f, 0.f, 1.f, 1.f});
    offscreenUIView = uiView;
    offscreenUIView.setViewport({0.f, 0.f, 1.f, 1.f});
    sf::Vector2f viewSize = worldView.getSize();
    hud.setLayout(viewSize);
    menu.layout(viewSize);
    over.layout(viewSize);
    recUI.layout(viewSize);
    pauseUI.layout(viewSize);
}


void Game::placeTowerChoice(TowerType type) {
    if (placementPadIndex < 0) return;
    if (coins < 30) {
        towerMenu.close();
        return;
    }
    const sf::Vector2f& padPos = level.pads[placementPadIndex].pos;
    for (auto& tower : towers) {
        if (std::hypot(tower->pos.x - padPos.x, tower->pos.y - padPos.y) < 1.f) {
            towerMenu.close();
            return;
        }
    }
    auto t = std::make_unique<Tower>();
    t->pos = padPos;
    t->configure(type, level.tileSize);
    Tower* created = t.get();
    auto& towerTex = res.getTexture("assets/tower_base.png");
    created->setBaseTexture(towerTex, level.tileSize);
    if (type == TowerType::Normal) {
        auto& gunTex = res.getTexture("assets/conv_gun.png");
        created->setGunTexture(gunTex, level.tileSize);
    } else if (type == TowerType::Cryo) {
        auto& gunTex = res.getTexture("assets/cryo_gun.png");
        created->setGunTexture(gunTex, level.tileSize);
    } else if (type == TowerType::Poison) {
        auto& gunTex = res.getTexture("assets/poison_gun.png");
        created->setGunTexture(gunTex, level.tileSize);
    }
    auto& flashTex = res.getTexture("assets/conv_flash.png");
    created->setFlashTexture(flashTex, type);
    towers.push_back(std::move(t));
    selectedTower = created;
    coins -= 30;
    created->totalInvested = 30;
    towerMenu.close();
    upgradeMenu.close();
    upgradeTarget = nullptr;
    placementPadIndex = -1;
}


void Game::checkEnemyDefeat(Enemy& enemy){
    if (!enemy.alive && !enemy.rewardGranted) {
        enemy.rewardGranted = true;
        score += 10;
        coins += enemy.isBoss ? 15 : 5;
    }
}


Enemy* Game::findEnemyNear(const sf::Vector2f& pos, float radius) {
    float r2 = radius * radius;
    for (auto& e : enemies) {
        if (!e->alive) continue;
        float dx = e->pos.x - pos.x;
        float dy = e->pos.y - pos.y;
        if (dx*dx + dy*dy <= r2) return e.get();
    }
    return nullptr;
}


void Game::handleBeamDamage(const Projectile& projectile, Enemy& target){
    if (projectile.type == TowerType::Normal) {
        target.takeDamage(projectile.damage);
        checkEnemyDefeat(target);
    } else if (projectile.type == TowerType::Cryo) {
        target.takeDamage(projectile.damage);
        checkEnemyDefeat(target);
        if (target.alive) target.applyCryoHit(projectile.slowMultiplier, projectile.slowDuration, projectile.freezeDuration);
    } else if (projectile.type == TowerType::Poison) {
        target.takeDamage(projectile.damage);
        target.applyPoison(projectile.poisonTotalDamage, projectile.poisonDuration);
        checkEnemyDefeat(target);
    }
}


void Game::processBeamHit(const Projectile& projectile) {
    Enemy* target = findEnemyNear(projectile.end, 22.f);
    if (!target) return;
    handleBeamDamage(projectile, *target);
}


void Game::tryUpgradeBaseHp(){
    if (coins<30) return;
    base.maxHp += 20.f; base.hp = base.maxHp; coins -= 30;
    hud.update(score, coins, base.hp, base.maxHp);
}


void Game::update(float dt){
    if (state!=GameState::Playing) return;

    spawnTimer -= dt;
    if (spawnTimer<=0.f) {
        int spawnCount = 1 + waveCount / 64;
        float bossChance = std::min(0.15f, 0.02f + waveCount * 0.0015f);
        const float maxEnemySpeed = 220.f;
        auto& enemyTex = res.getTexture("assets/enemy.png");
        auto& bossTex = res.getTexture("assets/boss.png");
        for (int i = 0; i < spawnCount; ++i) {
            bool spawnBoss = (static_cast<float>(rng() % 10000) / 10000.f) < bossChance;
            auto e = std::make_unique<Enemy>();
            e->pos = level.spawnPos;
            e->setTexture(enemyTex, level.tileSize - 2.f);
            float baseHp = 10.f + score*0.04f;
            if (spawnBoss) {
                e->hp = baseHp * 3.f;
                float bossSpeed = (60.f + score*0.02f) * 0.6f;
                e->baseSpeed = std::min(maxEnemySpeed, bossSpeed);
                e->damage = (5.f + score*0.01f) * 2.f;
                e->makeBoss();
                e->setTexture(bossTex, level.tileSize * 1.3f);
            } else {
                e->hp = baseHp;
                float speed = 60.f + score*0.02f;
                e->baseSpeed = std::min(maxEnemySpeed, speed);
                e->damage = 5.f + score*0.01f;
            }
            for (auto wp : level.path.waypoints) e->path.push_back(wp);
            enemies.push_back(std::move(e));
        }
        waveCount++;
        float nextInterval;
        if (spawnCount >= 10) {
            nextInterval = std::min(10.f, std::max(1.f, static_cast<float>(waveCount)) * 0.1f);
            if (score >= 23000) {
                float t = std::clamp((score - 23000.f) / 7000.f, 0.f, 1.f);
                float reduction = (nextInterval - 1.f) * t;
                nextInterval -= reduction;
            }
        } else {
            nextInterval = std::max(0.8f, 0.12f * static_cast<float>(spawnCount));
        }
        spawnEvery = nextInterval;
        spawnTimer = spawnEvery;
    }

    for (auto& t : towers) {
        t->update(dt);
        auto p = t->tryShoot(enemies);
        if (p) {
            processBeamHit(*p);
            auto up = std::make_unique<Projectile>(*p);
            projectiles.push_back(std::move(up));
        }
    }

    for (auto& e : enemies) {
        e->update(dt);
        if (e->alive && e->path.empty()) {
            base.hp -= e->damage;
            e->alive = false;
            e->rewardGranted = true;
        }
        checkEnemyDefeat(*e);
    }

    for (auto& p : projectiles) {
        p->update(dt);
    }
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const std::unique_ptr<Projectile>& p){ return p->dead(); }), projectiles.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const std::unique_ptr<Enemy>& e){ return !e->alive; }), enemies.end());

    if (base.hp<=0.f) {
        records.add(score);
        recUI.setRecords(records.last);
        recUI.layout(worldView.getSize());
        state = GameState::GameOver;
    }

    hud.update(score, coins, base.hp, base.maxHp);
    base.update(dt);
}


void Game::draw(){
    if (!frameBufferReady) {
        frameBufferReady = frameBuffer.create(static_cast<unsigned>(worldView.getSize().x),
                                              static_cast<unsigned>(worldView.getSize().y));
        if (frameBufferReady) {
            frameBuffer.setSmooth(false);
        }
    }

    auto drawUiState = [&](auto&& drawer){
        window.clear(sf::Color(30,30,40));
        window.setView(uiView);
        drawer(window);
        window.display();
    };

    if (state==GameState::Menu) {
        drawUiState([&](sf::RenderTarget& target){ menu.draw(target); });
        return;
    }
    if (state==GameState::Records) {
        drawUiState([&](sf::RenderTarget& target){ recUI.draw(target); });
        return;
    }
    if (state==GameState::GameOver) {
        drawUiState([&](sf::RenderTarget& target){ over.draw(target); });
        return;
    }

    window.clear(sf::Color::Black);
    sf::RenderTarget* worldTarget = frameBufferReady ? static_cast<sf::RenderTarget*>(&frameBuffer)
                                                     : static_cast<sf::RenderTarget*>(&window);
    const sf::View& worldTargetView = frameBufferReady ? offscreenWorldView : worldView;
    worldTarget->setView(worldTargetView);
    worldTarget->clear(sf::Color(30,30,40));

    drawLevel(*worldTarget);
    base.draw(*worldTarget);
    for (auto& t: towers) {
        t->draw(*worldTarget);
        if (t.get() == selectedTower) drawSelection(*worldTarget, *t);
    }
    for (auto& e: enemies) e->draw(*worldTarget);
    for (auto& p: projectiles) p->draw(*worldTarget);
    drawTowerLevels(*worldTarget);

    auto blitWorld = [&]{
        frameBuffer.display();
        sf::Sprite frame(frameBuffer.getTexture());
        auto integerScale = [&](float availW, float availH, float texW, float texH) -> std::optional<float> {
            if (availW < texW || availH < texH) return std::nullopt;
            float scaleX = std::floor(availW / texW);
            float scaleY = std::floor(availH / texH);
            float scale = std::min(scaleX, scaleY);
            if (scale < 1.f) return std::nullopt;
            return scale;
        };
        sf::Vector2u texSize = frameBuffer.getSize();
        sf::Vector2u winSize = window.getSize();
        sf::FloatRect vp = worldView.getViewport();
        float availPixelW = vp.width * static_cast<float>(winSize.x);
        float availPixelH = vp.height * static_cast<float>(winSize.y);
        auto scaleOpt = integerScale(availPixelW, availPixelH,
                                     static_cast<float>(texSize.x),
                                     static_cast<float>(texSize.y));
        window.setView(window.getDefaultView());
        if (scaleOpt && *scaleOpt > 1.f) {
            float scale = *scaleOpt;
            float scaledWidth = texSize.x * scale;
            float scaledHeight = texSize.y * scale;
            float viewportOffsetX = vp.left * static_cast<float>(winSize.x);
            float viewportOffsetY = vp.top * static_cast<float>(winSize.y);
            float innerOffsetX = (availPixelW - scaledWidth) * 0.5f;
            float innerOffsetY = (availPixelH - scaledHeight) * 0.5f;
            frame.setScale(scale, scale);
            frame.setPosition(viewportOffsetX + innerOffsetX,
                              viewportOffsetY + innerOffsetY);
            window.draw(frame);
        } else {
            sf::View present;
            present.reset(sf::FloatRect(0.f, 0.f,
                                        static_cast<float>(texSize.x),
                                        static_cast<float>(texSize.y)));
            present.setViewport(vp);
            window.setView(present);
            window.draw(frame);
        }
    };

    if (frameBufferReady) {
        blitWorld();
    } else {
        window.setView(worldView);
    }

    window.setView(uiView);
    hud.draw(window);
    drawHelp(window);
    towerMenu.draw(window);
    upgradeMenu.draw(window);
    if (state == GameState::Paused) {
        pauseUI.draw(window);
    }
    window.display();
}


void Game::drawLevel(sf::RenderTarget& target){
    if (!level.hasGrid) {
        sf::Vector2f center = worldView.getCenter();
        float roadWidth = 600.f;
        float roadHeight = 24.f;
        sf::RectangleShape road({roadWidth, roadHeight});
        road.setFillColor(sf::Color(80,80,80));
        road.setPosition(center.x - roadWidth * 0.5f, center.y - roadHeight * 0.5f);
        target.draw(road);
        for (auto& pad : level.pads) {
            sf::CircleShape c(14.f); c.setOrigin(14,14);
            c.setFillColor(sf::Color(60,120,160));
            c.setPosition(pad.pos);
            target.draw(c);
        }
        return;
    }

    sf::RectangleShape tile({level.tileSize - 2.f, level.tileSize - 2.f});
    tile.setOutlineThickness(0.f);
    for (int y = 0; y < Level::GridSize; ++y) {
        for (int x = 0; x < Level::GridSize; ++x) {
            char c = level.grid[y][x];
            sf::Vector2f pos = {level.origin.x + x * level.tileSize + 1.f,
                                level.origin.y + y * level.tileSize + 1.f};
            tile.setPosition(pos);
            if (c == '@') tile.setFillColor(sf::Color(90, 90, 90));
            else if (c == '#') tile.setFillColor(sf::Color(60, 120, 160));
            else if (c == '^') tile.setFillColor(sf::Color(100, 200, 100));
            else tile.setFillColor(sf::Color(40, 48, 58));
            target.draw(tile);
        }
    }
}


void Game::drawTowerLevels(sf::RenderTarget& target){
    sf::Font f = font;
    for (auto& t : towers) {
        int displayLevel = std::min(12, t->totalUpgradeLevels + 1);
        sf::Text txt("Lv"+std::to_string(displayLevel), f, 16);
        txt.setPosition(t->pos.x-12, t->pos.y-28);
        sf::Vector2f pos = txt.getPosition();
        txt.setPosition(std::round(pos.x), std::round(pos.y));
        target.draw(txt);
    }
}


void Game::drawSelection(sf::RenderTarget& target, const Tower& tower) {
    float radius = level.hasGrid ? level.tileSize * 0.5f : 20.f;
    sf::CircleShape highlight(radius);
    highlight.setOrigin(radius, radius);
    highlight.setPosition(tower.pos);
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineThickness(2.f);
    highlight.setOutlineColor(sf::Color(250, 230, 90));
    target.draw(highlight);
}


void Game::drawHelp(sf::RenderTarget&) {
}


void Game::enterPause() {
    if (state != GameState::Playing) return;
    towerMenu.close();
    upgradeMenu.close();
    upgradeTarget = nullptr;
    selectedTower = nullptr;
    state = GameState::Paused;
}


void Game::resumeFromPause() {
    if (state == GameState::Paused) {
        state = GameState::Playing;
    }
}


void Game::exitToMenuFromPause() {
    towerMenu.close();
    upgradeMenu.close();
    selectedTower = nullptr;
    upgradeTarget = nullptr;
    state = GameState::Menu;
}


sf::Vector2f Game::worldToUI(const sf::Vector2f& world) const {
    sf::Vector2i pixel = window.mapCoordsToPixel(world, worldView);
    return window.mapPixelToCoords(pixel, uiView);
}
