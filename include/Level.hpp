#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <optional>
#include <queue>
#include <string>
#include <vector>


struct BuildPad {
    sf::Vector2f pos;
};


struct Path {
    std::vector<sf::Vector2f> waypoints;
};


struct Level {
    static constexpr int GridSize = 20;

    sf::FloatRect bounds{0,0,800,600};
    sf::Vector2f origin{100.f, 60.f};
    float tileSize{30.f};

    std::array<std::string, GridSize> grid{};
    bool hasGrid{false};

    std::vector<BuildPad> pads;
    Path path;
    sf::Vector2f basePos{740,300};
    sf::Vector2f spawnPos{60,300};

    bool loadFromFile(const std::string& file);

    sf::Vector2f cellCenter(int x, int y) const;

    bool isBuildAllowed(const sf::Vector2f& p) const;

    int nearestPadIndex(const sf::Vector2f& p) const;
};
